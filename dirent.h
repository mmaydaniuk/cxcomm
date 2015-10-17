#ifndef _DIRENT_H
#define _DIRENT_H
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
#define DT_BINARY 1
#define DT_ASCII 2
#define DT_UNKNOWN 3



typedef struct dirent {
	char 			d_name[9];
	char			d_ext[4];
	unsigned char 	d_type;
	unsigned char	is_ascii;
	unsigned char   first_granule;
	unsigned int	last_sector_bytes;
	char			unused[16];
} dirent;


typedef struct DIR {
	unsigned char 	cur_offset;
	unsigned char 	cur_sector;
	unsigned char   drive_number;
	char			buffer[256];
} DIR;



DIR* opendir(char path);
void closedir(DIR* dp);
dirent* readdir(DIR* dirstream);


#endif
