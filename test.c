#include <cmoc.h>
#include "stdint.h"
#include "floppy.c"
#include "dirent.c"
#include "malloc.c"
#include "ccstdio.c"





uint8_t textScreenWidth  = 32;
uint8_t textScreenHeight = 16;


// Returns true for success, false if arg is invalid.
//
uint8_t width(uint8_t columns)
{
    if (columns != 32 && columns != 40 && columns != 80)
        return 0;

    asm("PSHS", "U,Y");  // protect against BASIC routine
    asm("LDB", columns);
    asm("JSR", "$F643");  // inside the WIDTH command
    asm("PULS", "Y,U");

    textScreenWidth = columns;
    textScreenHeight = (columns == 32 ? 16 : 24);

    return 1;
}


int main() {
	width(80);
	unsigned char track;
	unsigned char sector;
	unsigned char buffer[512];
	
	/*printf("Testing dirent()\n");
	dirent_test();

	granule_to_track(1,&track, &sector);
	printf("Track = %u Sector=%u",track,sector);
	
	printf("\nTesting loadfat()\n");
	//load_fat(0);
	*/
	init_disk_driver();
	
	FILE * fp = fopen("messages.dat:0","r");
	if (fp == 0) {
		printf("File not found!!\n");
	}
		
	int b;
	
	while (!feof(fp)) {
		b = fread((char*) buffer, 1, 512, fp);
		hex_dump("Buffer contents",buffer, b);
	}
	
	return 0;
}
