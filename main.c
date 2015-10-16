#include <cmoc.h>
#include "xmodem.h"


#define COMVEC	0x120
#define STANDARD_CMD_COUNT 23
#define ADDL_CMD_COUNT 2

unsigned int disp_tbl[28];
void* old_disp_tbl;

// new commands
// sx - send xmodem
// rx - receive xmodem
// host - turn on or off output to RS232
// ls - nice directory listing
// redraw - redraw terminal screen
char new_cmd_dict[] = { 'S',0xD8,
						'R',0xD8,
						'H','O','S', 0xD4,
						'L',0xD3,
						'R','E','D','R','A',0xD8};

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
	disp_tbl[25] = term_host;
	disp_tbl[26] = disk_ls;
	disp_tbl[27] = term_redraw;
	
	// copy the new dictionary commands to the end of the old dictionary table
	memcpy(old_disp_tbl, (void*) new_cmd_dict, 10); 
	
	
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
	
