/***********************************************************************
**
** This file is part of the CXCOMM project.
**
** Copyright (C) 2015 Michael Maydaniuk.  All rights reserved.
** 
**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file gpl-2.0.txt included in the
** packaging of this file.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
** 02110-1301  USA
**
** 
**
**********************************************************************/

#include "ccstdio.h"
#include "stdint.h"




/*
 * take a string and convert it into a structure with separate name, extension and drive # fields
 */
int 
parse_filename(fname* fn, char* filename) {
	char* DEFDRV = (char*) 0x095A;	
	
	memset(fn, 0x20, sizeof(fname));
	fn->filename[8] = 0x0;
	fn->ext[3] = 0x0;
	fn->drive_number = *DEFDRV;
	
	// copy filename
	int i = 0;
	while (*filename != '.' && *filename != '\0' && *filename != ':') {
		fn->filename[i] = (char) toupper(*(filename++));
		i++;
	}
	
	// copy extension
	i = 0;
	while (*filename != ':' && *filename != '\0') {
		if (*filename != '.') {
			fn->ext[i] = (char) toupper(*filename);
			i++;
		}
		filename++;
	}
	
	// if a drive number specified, set it.  otherwise use default
	// from RSDOS (*DEFDRV)
	if (*filename == ':') {
		fn->drive_number = -48 + *(++filename);
	}
	
	return 1;
	
}

/* open a file and return a pointer to a FILE structure. 
 *
 *  The function fread() reads nmemb items of data, each size bytes long,
 *  from the stream pointed to by stream, storing them at the location
 *  given by ptr.
 * 
 * tries to emulate POSIX fopen */
	
FILE*	
fopen(char *filename,char *mode) 
{
	DIR *dp;
	dirent *ep;
	FILE* fp = 0;
	fat* fat_table;
	
	
	fname fn;
	if (!parse_filename(&fn, filename)) 
		return 0;
	
	// get the fat and first gran
	if (!(fat_table = load_fat(fn.drive_number))) {
		return 0;
	}
	
	// open the directory entry
	if (!(dp = opendir(fn.drive_number))) {
		printf("Failed to open\n");
		return 0;
	}
	
	
	
	unsigned char i = 0;
	while ((ep = readdir(dp)) != NULL) {

		if (!strcmp(ep->d_name, fn.filename) && !strcmp(ep->d_ext, fn.ext)) {
	
			fp = (FILE*) malloc(sizeof(FILE));
			
			// I've stripped down the mode names to save a few bytes.
			// define STDIO_ALL_MODE_NAMES if you need the alternate forms
			#ifdef STDIO_ALL_MODE_NAMES
			if (!strcmp(mode, "r") || !strcmp(mode, "rb")) {
				fp->access_mode |= ACCESS_MODE_READ;
			} else if (!strcmp(mode, "w")) {
				fp->access_mode |= ACCESS_MODE_WRITE;
			} else if (!strcmp(mode, "a") || !strcmp(mode, "ab")) {
				fp->access_mode |= ACCESS_MODE_WRITE | ACCESS_MODE_APPEND;
			} else if (!strcmp(mode, "r+") || !strcmp(mode, "rb+") || !strcmp(mode,"r+b")) {
				fp->access_mode |= ACCESS_MODE_READ | ACCESS_MODE_WRITE;
			} else if (!strcmp(mode, "w+") || strcmp(mode,"wb+") || strcmp(mode, "w+b")) {
				fp->access_mode |= ACCESS_MODE_WRITE | ACCESS_MODE_TRUNCATE;
			} else if (!strcmp(mode, "a+") || strcmp(mode,"ab+") || strcmp(mode, "a+b")) {
				fp->access_mode |= ACCESS_MODE_APPEND | ACCESS_MODE_WRITE;
			}
			#else
			if (!strcmp(mode, "r")) {
				fp->access_mode |= ACCESS_MODE_READ;
			} else if (!strcmp(mode, "w")) {
				fp->access_mode |= ACCESS_MODE_WRITE;
			} else if (!strcmp(mode, "a")) {
				fp->access_mode |= ACCESS_MODE_WRITE | ACCESS_MODE_APPEND;
			} else if (!strcmp(mode, "r+")) {
				fp->access_mode |= ACCESS_MODE_READ | ACCESS_MODE_WRITE;
			} else if (!strcmp(mode, "w+")) {
				fp->access_mode |= ACCESS_MODE_WRITE | ACCESS_MODE_TRUNCATE;
			} else if (!strcmp(mode, "a+")) {
				fp->access_mode |= ACCESS_MODE_APPEND | ACCESS_MODE_WRITE;
			}		
			#endif

			
			
			
			fp->dir_entry_number = i;
			fp->first_granule = ep->first_granule;
			fp->drive_number = fn.drive_number;
			fp->fat_buffer = fat_table;
			
			fp->current_granule = ep->first_granule;
			//fp->next_granule = fat_table->granules[ep->first_granule];
			
			fp->current_track = 0;
			fp->current_sector = 0;
			fp->current_sector_offset = 0;
			fp->file_offset = 0;
			fp->error_code = 0;
			fp->last_sector_bytes = ep->last_sector_bytes;
			memset(fp->buffer,0x72,256);

			break;
		}
		i++;
	}
	
	closedir (dp);
	
	return fp;
	
}



/*
 * This is a horrible implementation of the algorithm to get the track,sector and offset of the next byte.
 * The idea though is to get this to work, then optimize it.
 */
int get_offset_location(FILE* stream, uint16_t offset,  uint8_t* track, uint8_t* sector, uint8_t* sector_offset) {
	uint16_t sector_idx;
	uint16_t granule_offset;
	
	// 3 divides.. yes there are better ways to do this.   
	*sector_offset =  (uint8_t) (offset % 256) & 0xFF;
		
	if (offset > 0) 
		sector_idx = offset/256;
	else
		sector_idx = 0;
	
	if (sector_idx > 0) 	
		granule_offset = (sector_idx / 9);
	else 
		granule_offset = 0;
	
	*sector  = (uint8_t) (sector_idx % 9) & 0xFF;
	(*sector)++;
		 
	// traverse list of granules... again not the best to iterate this with each byte read
	unsigned char next_gran;
	unsigned char current_gran;
	
	current_gran = stream->first_granule;
	next_gran = stream->fat_buffer->granules[stream->first_granule];
	
	int i = 0;
	while (next_gran < 0xC0 && i <= granule_offset) {
		// && i <= granule_offset) {
		current_gran = next_gran;
		next_gran = stream->fat_buffer->granules[next_gran];
		i++;
	}

	uint8_t sssector;
	
	granule_to_track(current_gran, track, &sssector);
	if (sssector == 9) {
		*sector += 8;
	}
	
	// if last granule
	if (next_gran >= 0xC0) {
		// if last sector
		int last_sector = next_gran & 0x0F;
	
		if (*sector == last_sector) {
			// if last byte
			if ((stream->last_sector_bytes-1 == *sector_offset) || (*sector_offset == 255)) {
				stream->error_code = stream->error_code ^ EOF;
			}
		}
	}
	
	return 1;
	
}

/* get a character from the currently open stream */
char getc(FILE* stream) {
	uint8_t track;
	uint8_t sector;
	uint8_t granule;
	uint8_t sector_offset;
	
	get_offset_location(stream, stream->file_offset, &track, &sector, &sector_offset);
	
	if (sector != stream->current_sector) {
		
		stream->current_sector = sector;
		stream->current_track = track;
		
		if (!dskcon(DSKCON_READ, (char*)stream->buffer, stream->drive_number, 
				stream->current_track, stream->current_sector)) {
				stream->error_code = stream->error_code ^ EINTR;
				return 0;
		}
	}
	stream->file_offset++;
	return stream->buffer[sector_offset];
}

size_t 
fread(char *ptr, size_t size, size_t nmemb, FILE *stream)
{
	int records_read = 0;
	unsigned int bytes_requested = size * nmemb;
	
	int i = 0;
	
	while (i < bytes_requested && !feof(stream)) {
		i++;
		*ptr = getc(stream);
		if (ferror(stream)) {
				return 0;
		}
			
		ptr++;
	}
	return i;  // TODO returning bytes note s.b. records
}

size_t
fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	if (!(stream->access_mode & ACCESS_MODE_WRITE)) {
		return 0;
	}
		
}



void clearerr(FILE *stream) {
	stream->error_code &= 0x0F;
}

/*
 *  The  function  feof() tests the end-of-file indicator for the stream pointed to by stream,
 *  returning nonzero if it is set. 
 */     
int feof(FILE *stream) {
	return (stream->error_code & EOF);
}

/*
 * The function ferror() tests the error indicator for the stream pointed to by stream, 
 * returning nonzero  if  it  is set.  The error indicator can be reset only by the clearerr() function.
 */ 
int ferror(FILE *stream) {
	return (stream->error_code & 0xF0);
}

/* The rewind() function sets the file position indicator for the stream
       pointed to by stream to the beginning of the file.  It is equivalent
       to:

              (void) fseek(stream, 0L, SEEK_SET)

       except that the error indicator for the stream is also cleared (see
       clearerr(3)).
*/
void rewind(FILE* stream) {
	clearerr(stream);
	stream->file_offset = 0;
}

/* The fseek() function sets the file position indicator for the stream
 * pointed to by stream.  The new position, measured in bytes, is
 * obtained by adding offset bytes to the position specified by whence.
 * If whence is set to SEEK_SET, SEEK_CUR, or SEEK_END, the offset is
 * relative to the start of the file, the current position indicator, or
 * end-of-file, respectively.  A successful call to the fseek() function
 * clears the end-of-file indicator for the stream and undoes any
 * effects of the ungetc(3) function on the same stream.
*/
int 
fseek(FILE* stream, uint16_t offset, uint8_t whence) {
	switch (whence) {
		case SEEK_SET:
			stream->file_offset = offset;
		break;
		
		case SEEK_CUR:
			stream->file_offset += offset;
		break;
		
		case SEEK_END:
			// if file is read, then set eof and seek to end of file
			//stream->file_offset += filesize(stream);
			// if file is rw then expand file as needed
		break;
	}
	return 0;
		
}



	

