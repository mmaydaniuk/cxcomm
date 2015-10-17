#ifndef _CCSTDIO
#define _CCSTDIO

typedef struct FILE {
	char 	dir_entry_number;		// number of the file in the directory
	char 	first_granule;			// first granule number
	char	current_granule;		// the current granule pointer
	char	current_sector;			// the current sector pointer
	char 	current_offset;			// current offset within current sector
	char 	buffer[256];			// i/o buffer
	char 	granule_data;			// the granule data from the FAT
} FILE;

FILE 	*fopen(const char *filename, const char *mode);
size_t   fread(char *, size_t, size_t, FILE *);
int      fseek(FILE *, long int, int);
int      getc(FILE *);
size_t   fwrite(const char *, size_t, size_t, FILE *);

#endif
/*
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
*/
