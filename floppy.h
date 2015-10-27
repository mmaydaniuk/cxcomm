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

#ifndef CXCOMM_DISKH
#define CXCOMM_DISKH

#include "dirent.h"


#define SECTOR_SIZE	256
#define C_EOF		-1

#define DSKCON_READ  2
#define DSKCON_WRITE 3

#define FAT_HEADER_LEN 6
#define FAT_DATA_LEN 158


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
	unsigned char access_count;				// count number of accesses in RAM
	unsigned char dirty_flag; 				// non-zero if RAM image modified
	unsigned char drive_number;				// the drive number for this FAT image, note we are 
	unsigned char granules[FAT_DATA_LEN];	// granule buffer.  Can be 68 to 158 bytes long (35 to 80 tracks)
	// TODO: would be a good idea to dynamically allocate the FAT DATA to save 100 bytes
} fat;
	


// store 4 fat buffers.  This is going to be a problem for drivewire
// and others that allow 255 drives... TODO
fat* fatbufs[4] = {0x0, 0x0, 0x0, 0x0};
	

char dskcon(char operation, char *buffer, char drive, char track, char sector);
fat* load_fat(char drive_number);
char init_disk_driver();
char close_disk_driver();
void granule_to_track(unsigned char granule, unsigned char* track, unsigned char* sector);


#endif

