#ifndef CXCOMM_DISKH
#define CXCOMM_DISKH

#define SECTOR_SIZE	256
#define C_EOF		-1

#define DSKCON_READ  2
#define DSKCON_WRITE 3

#define FAT_HEADER_LEN 6
#define FAT_DATA_LEN 158

typedef struct filespec
{
    char drive;
    char first_granule;  
    int num_bytes_last_sector;  // 0..256
    int len[2];  // 32-bit word giving length of file
    char cur_granule;  // 0..67, 255 means at EOF
    char cur_sector;  // 1..9 (relative to current granule)
    int cur_granule_len;  // 0..2304
    int offset[2];  // 32-bit reading/writing offset
    int sector_offset;  // 0..256: index into curSector[] (256 means beyond sector)
    char sector_buffer[256];
    int sector_buffer_free_bytes;  // number valid bytes in curSector[] (0..256)
    char next_granule;
} filspec;


/* FILE ALLOCATION TABLE FORMAT
* THE FILE ALLOCATION TABLE (FAT) CONTAINS THE STATUS OF THE GRANULES ON A DISKETTE.
* THE FAT CONTAINS 6 CONTROL BYTES FOLLOWED BY 68 DATA BYTES (ONE PER GRANULE). ONLY THE
* FIRST TWO OF THE SIX CONTROL BYTES ARE USED. A VALUE OF $FF IS SAVED IN UNALLOCATED
* GRANULES. IF BITS 6 & 7 OF THE DATA BYTE ARE SET, THE GRANULE IS THE LAST GRANULE
* IN A FILE AND BITS 0-5 ARE THE NUMBER OF USED SECTORS IN THAT GRANULE. IF BITS 6 & 7
* ARE NOT SET, THE DATA BYTE CONTAINS THE NUMBER OF THE NEXT GRANULE IN THE FILE
*/
typedef struct fat 
{
	char access_count;				// count number of accesses in RAM
	char dirty_flag; 				// non-zero if RAM image modified
	char drive;						// the drive number for this FAT image, note we are 
									// re-using some of the unused bytes in the FAT here
	char unused[3];				
	char granules[FAT_DATA_LEN];	// granule buffer.  Can be 68 to 158 bytes long (35 to 80 tracks)
	// TODO: would be a good idea to dynamically allocate the FAT DATA to save 100 bytes
} fat;
	
	


/*
    Byte         Description

    0—7        Filename, which is left justified and blank, filled. If byte0 is 0,
               then the file has been ‘KILL’ed and the directory entry is available
               for use. If byte0 is $FF, then the entry and all following entries
               have never been used.
    8—10       Filename extension
    11         File type: 0=BASIC, 1=BASIC data, 2=Machine language, 3= Text editor
               source
    12         ASCII flag: 0=binary or crunched BASIC, $FF=ASCII
    13         Number of the first granule in the file
    14—15      Number of bytes used in the last sector of the file
   16—31      Unused (future use)
 */
typedef struct s_direntry
{
	char			filename[8];
	char			extension[3];
	char			file_type;
	char			ascii_flag;
	char 			num_bytes_first_granule;
	char			num_bytes_last_sector[2];
	unsigned int	next_entry;
} direntry;
	
// return singly linked list of dir entries
direntry* get_directory();
char* find_files(char* filespec);
char dskcon(char operation, char *buffer, char drive, char track, char sector);


//filespec* open_file(char* filename);
//int get_next_sector(filespec* fs);

#endif

