#include <cmoc.h>
typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned char sbyte;

#include <disk.h>

void send_xmodem();
void receive_xmodem();
void crc16();
void checksum();


#define COMVEC	0x120
#define STANDARD_CMD_COUNT 23
#define ADDL_CMD_COUNT 2

unsigned int disp_tbl[25];
void* old_disp_tbl;

byte x;
 
//int volatile * const old_disp_tbl = (int*) 0xE236;

// new commands
// sx - send xmodem
// rx - receive xmodem
char new_cmd_dict[] = { 'S',0xD8,'R',0xD8 };

void send_xmodem() {
	printf("TX XMODEM");
	// find file in directory
	// get first granule
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

void receive_xmodem() {
	printf("RX XMODEM");
}

/*
 install the patched commands into memory so they can be called via command interpreter.
 
 Color basic uses two tables, a dictionary table of commands, and a dispatch table of addresses.
 There is no memory available between the dictionary and dispatch table in the CC3 rom, so we 
 copy the dispatch table to a new address (disp_tbl) and we tack the new commands onto the existing
 dictionary.
 
 We then update the pointer to the dispatch table, and a few counter variables in the ROM.
*/
void install(unsigned int* disp_tbl) {
	old_disp_tbl = 0xe236;

	// copy the old dispatch table, to a new one we have declared
	memcpy((void*) disp_tbl, old_disp_tbl, 46);
	
	// add our new routines to the end of the table
	disp_tbl[23] = send_xmodem;
	disp_tbl[24] = receive_xmodem;
	
	// copy the new dictionary commands to the end of the old dictionary table
	memcpy(old_disp_tbl, (void*) new_cmd_dict, 4); 
	
	
	asm {
	
		; we work by patching enhanced basic dispatch tables and tokenizer.
		; we add two new commands to the token list, and copy the dispatch 
		; table to a new area of memory, and add the dispatch links to it.
		;
		pshs 	a,x
		
		lda		#0x19		; there are 2 new commands
		sta		$e162		; patch the enhanced basic command vector table
		
		lda		#0xfa		; tokens now go up to $fa
		sta 	$e197		; patch th command dispatch routine
		ldx		disp_tbl	
		stx		$e1a1		; change the pointer to the new copy
		
		puls	x,a
	}	
}


int main() {
	install(disp_tbl);
	printf("Hello World");
	return 0;
}
	
