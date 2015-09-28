#include <cmoc.h>
#include <coco.h>

void init();
void send_xmodem();
void receive_xmodem();
void crc16(crc,c);
void checksum(data)

void asm dispatch();
void asm crunch();
void asm uncrunch();

char commandDictionary = "s\xD8r\xD8";
char jumpTable[] = {send_xmodem, receive_xmodem};

/*
       0726                                * BASIC 2.0 COMMAND INTERPRETATION VECTOR TABLE
       0727     E162   17                  EBCOMTAB FCB   23                               23 BASIC 2.0 COMMANDS
       0728     E163   E1   C5             LE163    FDB   COMDIC20                         BASIC 2.0'S COMMAND DICTIONARY
       0729     E165   E1   92             LE165    FDB   ALINK4                           COMMAND PROCESSING ROUTINE ENTRY POINT
       0730     E167   05                  LE167    FCB   5                                5 BASIC 2.0 FUNCTIONS
       0731     E168   E2   64             LE168    FDB   FUNDIC20                         FUNCTION DICTIONARY TABLE
       0732     E16A   E1   A6             LE16A    FDB   ALINK5                           FUNCTION PROCESSING ROUTINE ENTRY POINT
       0733     E16C   00   00 00 00 00 00 LE16C    FCB   $00,$00,$00,$00,$00,$00          DUMMY SPACE USED TO SIMULATE AN EMPTY COMMAND INTERP.
     
*/   
char vectorTable = {0x02,commandDictionary,dispatch,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

char buffer[128];

  
       
/* patch routine to dispatch new BASIC commands

   original source from ROM is here:
   0751                               * BASIC 2.0 COMMAND PROCESSING ROUTINE ENTRY POINT PATCH ENTERED FROM $8150
   0752     E192   81   E2            ALINK4   CMPA #$E2                               TOKEN NUMBER OF FIRST ENHANCED BASIC COMMAND
   0753     E194   25   04                     BCS   LE19A                             BRANCH TO DOS TABLE IF LESS THAN ENHANCED TOKEN
   0754     E196   81   F8                     CMPA #$F8                               COMPARE TO THE HIGHEST ENHANCED BASIC TOKEN
   0755     E198   23   04                     BLS   LE19E                             BRANCH IF ENHANCED BASIC TOKEN
   0756     E19A   6E   9F 01 37      LE19A    JMP   [COMVEC+23]                       GO TO DISK BASIC'S COMMAND HANDLER
   0757     E19E   80   E2            LE19E    SUBA #$E2                               SUBTRACT OUT THE NON-ENHANCED BASIC TOKENS
   0758     E1A0   8E   E2 36                  LDX   #COMDIS20                         POINT X TO ENHANCED BASIC'S COMMAND DISPATCH TABLE
   0759     E1A3   7E   AD D4                  JMP   LADD4 
*/

#define COMVEC	0x120

void asm dispatch() {
	asm {
					cmpa	#$f9		; first token in patch
					bcs 	$E192		; branch to enhanced table if less than patch token 
					cmpa	#$fa		; compare to highest token in patch
					bls		patchtok	; its a valid token from the patch
					jmp		[COMVEC+23]	; otherwise go to disk basic's command handler (error?)
		patchtok:	suba 	#$f9		; sub out token #
					ldx		jumpTable	; point to the patch jump table
					jmp		$add4		; return to basic's command processor
		}
}


/* crunch - patch routine to crush basic line
	   0707     E138   0D   41            ALINK2   TST   V41                                    CHECK THE TOKEN FLAG
       0708     E13A   26   16                     BNE   LE152                                  BRANCH IF IT IS A FUNCTION TOKEN
       0709     E13C   96   42                     LDA   V42                                    GET THE TOKEN COUNTER
       0710     E13E   81   62                     CMPA #$62                                    COMPARE TO THE FIRST ENHANCED BASIC TOKEN
       0711     E140   23   06                     BLS   LE148                                  BRANCH IF BEFORE FIRST TOKEN
       0712     E142   CE   01   1B                LDU   #COMVEC-5                              POINT U TO EXTENDED COLOR BASIC'S INTERPRETATION TABLE, NEXT ROUTINE ADDS 10 TO U TO GET # OF TOKENS      
       0713     E145   7E   B8   D7                JMP   LB8D7                                  RE-ENTER THE MAIN STREAM CODE
       0714     E148   86   62            LE148    LDA   #$62                                   FORCE THE TOKEN COUNTER TO THE FIRST ENHANCED BASIC TOKEN NUMBER
       0715     E14A   CE   E1   58                LDU   LE158                                  POINT TO ENHANCED BASIC'S COMMAND INTERPRETATION TABLE
       0716     E14D   97   42            LE14D    STA   V42                                    SAVE THE NEW TOKEN COUNTER
       0717     E14F   7E   B8   9D                JMP   LB89D                                  RE-ENTER THE MAIN STREAM CODE
       0718     E152   96   42            LE152    LDA   V42                                    GET THE TOKEN COUNTER
       0719     E154   81   29                     CMPA #$29                                    COMPARE TO THE FIRST ENHANCED FUNCTION TOKEN NUMBER
       0720     E156   23   03                     BLS   LE15B                                  BRANCH IF LESS THAN ENHANCED TOKEN NUMBER
       0721     E158   7E   B8   D7       LE158    JMP   LB9D7                                  RE-ENTER THE MAIN STREAM CODE
       0722     E15B   86   29            LE15B    LDA   #$29                                   FORCE COUNTER TO FIRST ENHANCED FUNCTION
       0723     E15D   CE   E1   5D       LE15D    LDU   #LE15D                                 POINT TO THE ENHANCED FUNCTION INTERPRETATION TABLE
       0724     E160   20   EB                     BRA   LE14D
*/

void crunch() {

}



/* uncrunch - patch routine to uncrunch a basic line
 	   0735                               * UNCRUNCH A TOKEN PATCH ENTERED FROM $B7F3
       0736     E172   33   4A            ALINK3   LEAU 10,U                                    SKIP TO THE NEXT COMMAND INTERPRETATION TABLE
       0737     E174   6D   C4                     TST   ,U                                     IS THIS A VALID TABLE?
       0738     E176   10   26 D6 7F               LBNE LD67F                                   YES - RE-ENTER THE MAIN STREAM CODE
       0739     E17A   30   1F                     LEAX $-01,X                                  UNNECESSARY INSTRUCTION; NEXT ONE SHOULD JUST BE LDA -1,X
       0740     E17C   A6   80                     LDA   ,X+                                    GET THE TOKEN FROM BASIC'S INPUT LINE
       0741     E17E   84   7F                     ANDA #$7F                                    STRIP OFF THE $80 COMMAND TOKEN BIAS
      0742     E180   81   62                     CMPA #$62                                    FIRST LEGAL BASIC 2.0 COMMAND TOKEN NUMBER
       0743     E182   25   07                     BCS   LE18B                                  BRANCH IF LEGAL TOKEN
       0744     E184   80   62                     SUBA #$62                                    ADJUST BASIC 2.0 TOKENS TO START AT 0
       0745     E186   CE   E1 58                  LDU   #LE158                                 POINT TO ENHANCED BASIC'S COMMAND INTERPRETATION TABLE
       0746     E189   20   E7                     BRA   ALINK3
       0747     E18B   80   29            LE18B    SUBA #$29                                    SUBTRACT OUT THE FIRST ENHANCED FUNCTION TABLE
       0748     E18D   CE   E1 5D                  LDU   #LE15D                                 POINT U TO BE ABLE TO SEARCH FOR AN ENHANCED FUNCTION TOKEN
       0749     E190   20   E0                     BRA   ALINK3
*/
void uncrunch() {

}

void install() {
	asm {
		; change the vectors around to hit our routines first
		
		; edit $8150 - Command Dispatcher
	
		push	x
		ldx 	$8150
		stx		,dispatch
		
		; edit $B8D4 - Crunch command
		ldx 	$b8d4
		stx		,crunch
		
		; edit $B7F3 - Uncrunch command
		ldx 	$b7f3
		stx		,uncrunch
		
		pop		x
	}	
}


int init() {
	memset(buffer, '\x0', 128);
}

int main() {
	initCoCoSupport();
	printf("Hello World");
}
	
