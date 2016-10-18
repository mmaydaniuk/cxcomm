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
**` but WITHOUT ANY WARRANTY; without even the implied warranty of
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
 
#include "floppy.h"
#include "malloc.h"
#include "debug.h"
#include "unistd.h"

/* floppy.h
 * 
 * Low-level coco disk routines
 */



// operation: DSKCON operation code
// dest: non null pointer to a 256-byte region.
// drive: 0..3.
// track: 0..34.
// sector: 1..18 (sic).
// Returns !0 for success, 0 for failure.
//
// Taken from P. Sarrazin's CMOC code
//
char 
dskcon(char operation, char *buffer, char drive, char track, char sector)
{
    printf("- dskcon( OP = %u, BUF= 0x%04x, DRIVE = %u, TRACK= %u, SECTOR=%u)\n", operation, buffer, drive, track, sector);
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

char sector_buf[256];

/*
 * load_fat
 * 
 * Load the FAT for the specified drive into RAM
 */
 
fat* 
load_fat(char drive_number) 
{

	fat* fatbuf;

	if (fatbufs[drive_number] == 0x0) {
		
		fatbuf = (fat*) malloc(sizeof(fat));
		
		if (fatbuf == 0)
			return 0;

		if (!dskcon(DSKCON_READ, (char*) &fatbuf->granules, drive_number, 17, 2))
			return 0;
		fatbufs[drive_number] = fatbuf;
	
	} else {
		fatbuf = fatbufs[drive_number];
	}
	
	fatbuf->drive_number = drive_number;
	hex_dump("FAT Table", &fatbuf->granules, 68);
	
	return fatbuf;
}

/*
 * initialize the disk driver.
 */
char 
init_disk_driver() {
	memset(fatbufs, 0, sizeof(fatbufs));
	return 1;
}

/*
 * clean up any allocated memory
 */
char 
close_disk_driver() {
	memset(fatbufs, 0, sizeof(fatbufs));
	return 1;
}

/*
 * Convert a granule to a sector/track
 * 
 * inspired by P Sarrazin's code
 */
void granule_to_track(unsigned char granule, unsigned char* track, unsigned char* sector)
{
	*track = granule >> 1;
	if (*track >= 17) 
		(*track)++;

	*sector = granule & 0x1;

	if (*sector != 0) {
		(*sector) = 9;
	} else { 
		*sector = 1;
	}

}

	
	
	




