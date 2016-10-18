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
#ifndef __UNISTDH
#define __UNISTDH

// newValue: word value.
//
#define setTimer(newValue) (* (uint16_t *) 0x112 = (newValue))

// Returns a word.
//
#define getTimer() (* (uint16_t *) 0x112)


// seconds: 0..1092.
//
// NB: shamelessly stolen from P Sarrazin's code
void sleep(int seconds)
{
    if (!seconds)
        return;
    uint16_t limit = 60 * (uint16_t) seconds;
    setTimer(0);
    while (getTimer() < limit)
        ;
}



#endif
