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

/*
 * dirent.c
 * 
 * Try to simulate the POSIX dirent functions on the TRS-80
 */

#include "dirent.h"
#include "disk.h"
#include "malloc.h"

#ifndef NULL
#define NULL 0
#endif

dirent* dir_entry;

/*
 * opendir - open up the directory
 * 
 */
DIR* opendir(char path) 
{
		
	DIR* d = (DIR*) malloc(sizeof(DIR));
	
	memset(d,0,sizeof(DIR));
	
	dir_entry = (dirent*) malloc(sizeof(dirent));
	
	
	if (!read_sector(d->buffer, path, 17, 3))
		return NULL;
	
	d->cur_offset = 0;
	d->cur_sector = 3;
	d->drive_number = path;
	
	return d;	
}

/*
 * closedir
 * 
 * close dir and free memory
 */
void closedir(DIR* dp) {
	if (dp != NULL) 
		free((char*) dp);
	if (dir_entry != NULL)
		free((char*) dir_entry);
}

/*
 * readdir
 * 
 * return next entry from directory
 */
dirent* readdir(DIR* dir) 
{
	char* entry;

	entry = dir->buffer + dir->cur_offset;
	
	// last directory entry
	// shouldn't have to cast 0xFF should I? Is it a compiler issue 
	// or programmer one?
	if (*entry == (char) 0xFF) {
		return NULL;
	}
	
	// if this a valid entry 
	if (*entry != 0x00) {
		memset(dir_entry, 0, sizeof(dirent));
		
		char* p = (char*) dir_entry;
		char* src = (char*) entry;
		
		memcpy(p,src, 8);
		p += 9;
		src += 8;
		memcpy(p,src, 3);
		p += 4;
		src += 3;
		memcpy(p,src,21);
			
		dir->cur_offset += 32;   // point to the next entry
		
		// get next sector, note relies on cur_offset being 256 bytes
		if (dir->cur_offset == 0 && ++dir->cur_sector < 19) {
			read_sector(dir->buffer, dir->drive_number, 17, dir->cur_sector);
		}
		return dir_entry;
	}
			
}

#define CXCOMM_TEST 1
#ifdef CXCOMM_TEST

int dirent_test() {
	DIR *dp;
	dirent *ep;
	
	dp = opendir(0);
	int i = 0;


	if (dp != NULL) {
		while ((ep = readdir(dp)) != NULL) {
			if (i > 2) return 0;
			printf("%s.%s\n",ep->d_name,ep->d_ext);
			++i;
		}
		closedir (dp);
	} else {
		printf("Failed to open");
	}
	
	return 0;
}
		
#endif	
	
	
	
