#ifndef CXCOMM_DISKH
#define CXCOMM_DISKH

#define SECTOR_SIZE	256
#define C_EOF		-1

typedef struct s_filespec
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
	char[8]			filename;
	char[3]			extension;
	char			file_type;
	char			ascii_flag;
	char 			num_bytes_first_granule;
	char[2]			num_bytes_last_sector;
	unsigned int	next_entry;
} direntry;
	
// return singly linked list of dir entries
direntry* get_directory();
char* find_files(char* filespec);

filespec* open_file(char* filename);
int get_next_sector(filespec* fs);

#endif

