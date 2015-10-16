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


include "xmodem.h"
include "ascii.h"

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned char sbyte;


void send_xmodem();
void receive_xmodem();
void crc16();
void checksum();

const char MODE_YMODEM 	= 0b00000001;
const char MODE_CRC 	= 0b00000010;
const char MODE_1KBLOCK	= 0b00000100;


byte x;
char sector_buffer[1024];

 
//int volatile * const old_disp_tbl = (int*) 0xE236;

void xm_check_block() {
	
	
	asm {
		; This is the traditional looping implementation: a lot shorter, but about
		; half as fast.  This algorithm can be found used everywhere.
		; 6809 implementation: Ciaran Anscomb
				eora	crc		; 
				ldb	#8			; 
				stb	crc			; 
				ldb	crc+1		; 
		loop:
				lslb			; 
				rola			;
				bcc	tag			; 
				eora	#$10	; 
				eorb	#$21	; 
		tag		dec	crc			; 
				bne	loop		; 
				std	crc			; 
				rts
		}
}

// send EOT's until acknowledged
// this is applicable to both YMODEM and XMODEM
int xm_done_transmit() {
	for (x= 0; x < 10; x++) {
		term_putc(EOT);
		if (term_peek_byte() == ACK) {
			return 0;
		}
	}
	return -1;
}



// get a block of data from disk.  
//
// returns a pointer to the data, C_EOF at end of file or error
char* xm_get_block_from_disk(filespec* fs, blocksize) {
	
}

void xm_send_xmodem(char* filename, void * (status_callback)(int blocknum, int code)) {
	filespec* fs;
	int ret;
	int blocknum = 1;
	int blocklen = 128;
	int mode = 0;
	int fileindex = 0; 
	int j = 0;
	
	printf("TX XMODEM");
	// find file in directory
	fs = open_file(filename);


	for (;;) {

		// get the next block from disk.  
		block = xm_get_block_from_disk(fs, blocklen);
		if (block == EOF) {
			xm_done_transmit();
			break;
		}
		

		// send the header block if it is XMODEM-1k or YMODEM
		if (blocknum == 0 && (mode & MODE_1KBLOCK == 0)) {
			// send header block	
			j = 0;
			while (c = *filename++) {
				term_putc(c);
				j++
			}
			// fill the first block with zeros.  is this needed?
			for (;j < 128; j++) {
				term_putc(0x00);
			}
			
		} else {
		
			// send a data block
			if (blocklen == 128) {
				term_putc(SOH);			// 128 byte blocks
			} else {	
				term_putc(STX);			// 1024 byte blocks
			}
			
			// send block number and its inverse
			term_putc(blocknum << 8);
			term_putc(~blocknum << 8);
			
			// send the block
			for (bufptr = sector_buffer; bufptr < sector_buffer+block_size; bufptr++) {
				term_putc(*bufptr);
				check = check_block(*bufptr, mode);
			}
			
			// send the checksum or crc depending on mode
			xm_check_block(check, mode);
		}
		
		// wait for response
		for (x = 0; x < 10; x++) {
			// wait for a response byte
			c = get_byte(2000);
			if (c < 0 || c == NAK) continue;
			
			// block is acknowledged, get next
			if (c == ACK) {
				if (blocknum == 0) {
					c = get_byte(2000);
				}
				blocknum++;
				break;
			} 
			
			// client cancelled, terminate
			if (c == CAN) {
				term_putc(ACK);
				return -1;
			}
		}		
	}
	// convert to sector

	// read sector
	
	// send first 128 bytes
	// calc CRC
	// send CRC
	
	// wait for ACK
	
	// send second 128 bytes
	// calc CRC
	// send CRC
	
	
}

void xm_receive_xmodem() {
	printf("RX XMODEM");
}

