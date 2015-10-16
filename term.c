#include "term.h"

void host() {
}

// install the terminal hook onto console
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

void term_install() {
}

// character output uses the code in the ROM DLOAD routine.   This is 
// relatively common to overwrite.   If your environment has already
// gotten rid of DLOAD, then define NO_DLOAD
#ifndef NO_DLOAD
void term_putc(char c) {
	asm {
		lda c			; put the character in ACCA
		jsr $8e0C		; call RS-232 out routine from DLOAD
	}
}


int term_getc() {
	int c;
	
	asm {
				jsr $8dbc		; get char from DLOAD RS-232 service routine
				ldx c			; get the address of C
				sta ,x			; store value of a in x
				ldx ,x+1		; increment x
				lda	0			; clear A
				beq tgetc_done	; if z=0 we got a char
				lda 0xFF		; store FF in the high byte if no chars
		tgetc_done:
				sta	,x			; store the high byte into memory
				
	}
	if (c > 16384) c = -1;
	
	return c;
}


#else
// implent char put routine if DLOAD has been overwritten
#endif


	


	


