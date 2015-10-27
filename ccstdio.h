
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
#ifndef _CCSTDIO
#define _CCSTDIO
#include "stdint.h"

#define EOF 0x01
#define EMFILE 0x10
#define ENAMETOOLONG 0x20
#define ENFILE 0x30
#define ENOENT 0x40
#define EINVAL 0x50
#define ENOMEM 0x60
#define EINTR 0x70

#define	ACCESS_MODE_READ     0x01
#define ACCESS_MODE_WRITE    0x02
#define ACCESS_MODE_APPEND 	 0x04
#define ACCESS_MODE_TRUNCATE 0x08

#define SEEK_SET 0x01
#define SEEK_CUR 0x02
#define SEEK_END 0x03

typedef struct FILE {
	uint8_t 	drive_number;				// drive the file is on
	uint8_t  	dir_entry_number;			// number of the file in the directory
	uint8_t  	first_granule;				// first granule number
	uint8_t 	current_granule;			// the current granule pointer
	uint8_t 	current_track;				// current track number
	uint8_t 	current_sector;				// the current sector pointer
	uint8_t  	current_sector_offset;		// current offset within current sector
	uint16_t	file_offset;				// dword file offset TODO: 64k file size limit is implied
	uint16_t	last_sector_bytes;			// bytes used in the last sector
	uint8_t		access_mode;				// the access mode for the file
	fat *		fat_buffer;					// pointer to the FAT buffer
	uint8_t  	buffer[256];				// i/o buffer
	uint8_t  	error_code;					// error or EOF code hi nibble is error code, lo nibble is status flag (EOF)
} FILE;

typedef struct fname {
	char	filename[9];
	char 	ext[4];
	char	drive_number;
} fname;

FILE 	*fopen(char *filename, char *mode);
void	fclose(FILE* fp);
int 	feof(FILE *stream);
int 	ferror(FILE* stream);

size_t   	fread(char * ptr, size_t size, size_t nmemb, FILE *stream);
int 		parse_filename(fname* fn, char* filename);
void 		rewind(FILE* stream);
int      	fseek(FILE* stream, uint16_t offset, uint8_t whence);
size_t 		fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream);

//int      getc(FILE *);
//size_t   fwrite(char *, size_t, size_t, FILE *);

#endif

