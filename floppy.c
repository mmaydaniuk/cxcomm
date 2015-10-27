#include "disk.h"
/* disk.h
 * 
 * Low-level coco disk routines
 */

// store 4 fat buffers.  This is going to be a problem for drivewire
// and others that allow 255 drives... TODO
fat* fatbufs[4] = {0x0, 0x0, 0x0, 0x0};


// operation: DSKCON operation code
// dest: non null pointer to a 256-byte region.
// drive: 0..3.
// track: 0..34.
// sector: 1..18 (sic).
// Returns !0 for success, 0 for failure.
//
char dskcon(char operation, char *buffer, char drive, char track, char sector)
{
    //printf("- dskcon(%u, 0x%04x, %u, %u, %u)\n", operation, buffer, drive, track, sector);
    if (operation != DSKCON_READ && operation != DSKCON_WRITE)
        return 0;
    if (buffer == 0)
        return 0;
    if (sector == 0)
        return 0;
    if (sector > 18)
        return 0;

    // Fill DSKCON input variables.
    * (char *) 0x00EA = operation;  // DCOPC
    * (char *) 0x00EB = drive;      // DCDRV
    * (char *) 0x00EC = track;      // DCTRK
    * (char *) 0x00ED = sector;     // DCSEC
    * (unsigned int *) 0x00EE = buffer;       // DCBPT

    asm("PSHS", "U,Y,X,A");  // protect against BASIC routine
    asm("JSR", "[$C004]");  // call DSKCON
    asm("PULS", "A,X,Y,U");

    return (* (char *) 0x00F0) == 0;  // zero in DCSTA means success
}


char read_sector(char *dest, char drive, char track, char sector)
{
    return dskcon(DSKCON_READ, dest, drive, track, sector);
}


char write_sector(char *src, char drive, char track, char sector)
{
    return dskcon(DSKCON_WRITE, src, drive, track, sector);
}


/*
 * load_fat
 * 
 * Load the FAT for the specified drive into RAM
 */
char load_fat(char drive_number) 
{
	char* buf[256];
	
	if (fatbufs[drive_number]) {
		fat* fatbuf = (fat*) malloc(FAT_DATA_LEN+FAT_HEADER_LEN);
		if (fat == 0)
			return 0;
	
		if !dskcon(DSKCON_READ, &buf, drive_number, 17, 2) 
			return 0;
	}
	
	memcpy(fat, &buf, FAT_HEADER_LEN+FAT_DATA_LEN);
	fatbufs[drive_number] = fat;
}

char init_disk_driver() {
	memset(fatbufs, 0, sizeof(fatbufs));
}

/*
 * clean up any allocated memory
 */
char close_disk_driver() {
	memset(fatbufs, 0, sizeof(fatbufs));
}




