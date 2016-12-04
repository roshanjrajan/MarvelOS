#include "terminal.h"

/*
 * ASCII scan codes for keyboard usage 
 */
static char ascii_scan[ALL_SCODES] = {
	// No shift, no capsLock
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' ', 0, 0, 0, 0, 0, 0,
	// Only capsLock
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', 0, 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', ',', '.', '/', 0, 0, 0, ' ', 0, 0, 0, 0, 0, 0,
	// Only shift
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' ', 0, 0, 0, 0, 0, 0,
	// Shift and capsLock
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n', 0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~', 0, '|', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', '<', '>', '?', 0, 0, 0, ' ', 0, 0, 0, 0, 0, 0
};

char KBbuf [NUM_TERMINALS] [BUF_SIZE]; //Keyboard buffer array
int buf_loc[NUM_TERMINALS]; //Location of next character in the buffer

//Flags for indicating if various buttons are pressed
int capsFlag;
int shiftFlagL;
int shiftFlagR;
int ctrlFlagL;
int ctrlFlagR;
int altFlagL;
int altFlagR;

//Arrays to indicate if each terminal is ready or waiting to read
int readWaiting[NUM_TERMINALS];
int readReady[NUM_TERMINALS];

/*
 * KBhandler
 * Handler for keyboard input.
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: Prints typed character to the screen and populates the input buffer. 
 */
void KBhandler(){
	// As interrupt, save all general purpose registers
	asm volatile ("pusha");
	
	// Read in scan code
	uint8_t scode;
	scode = inb(KB_DATA_PORT);
	
	// Do stuff with scan code
	
	// Handle 2 byte scans so we don't block up the data register
	if(scode == READ_AGAIN){
		scode = inb(KB_DATA_PORT);
		
		// Handle cntrlR
		if(scode == CTM || scode == CTB){
			ctrlFlagR = (ctrlFlagR+1)%MOD2;	// Toggle flag
		}

		//Handle altR
		if(scode == ALTM || scode == ALTB){
			altFlagR = (altFlagR+1)%MOD2;	// Toggle flag
		}
	}
	
	// Handle cntrlL
	else if(scode == CTM || scode == CTB){
		ctrlFlagL = (ctrlFlagL+1)%MOD2;	// Toggle flag
	}
	
	//Handle altL
	else if(scode == ALTM || scode == ALTB){
		altFlagL = (altFlagL+1)%MOD2;	// Toggle flag
	}

	// Control modifier is pressed
	else if(ctrlFlagL || ctrlFlagR){
		// Ctrl-L means clear display and input buffer
		if(scode == CLEAR){
			clear();
			// Write things that are currently in the buffer
			int i = 0;
			for(i=0; i<buf_loc[currentTerminal]; i++)
				KBputc(KBbuf[currentTerminal][i]);
		}
	}

	//Alt modifier is pressed
	else if(altFlagL || altFlagR){
		int validTerminal = 0;
		int previousTerminal = currentTerminal;

		//F1 means to switch to terminal 0
		if(scode == TERMINAL_0_SCANCODE && previousTerminal != TERMINAL_0) {
			currentTerminal = TERMINAL_0;
			validTerminal = 1;
		}

		//F2 means to switch to terminal 1
		else if(scode == TERMINAL_1_SCANCODE && previousTerminal != TERMINAL_1) {
			currentTerminal = TERMINAL_1;
			validTerminal = 1;
		}
		
		//F3 means to switch to terminal 2		
		else if(scode == TERMINAL_2_SCANCODE && previousTerminal != TERMINAL_2) {
			currentTerminal = TERMINAL_2;
			validTerminal = 1;
		}

		//Call an externally defined function to perform the video and context switch
		if(validTerminal) {
			switchTerminal(previousTerminal);
		}
	}

	// Handle caps lock
	else if(scode == CAPS){
		capsFlag = (capsFlag+1)%MOD2;	// Toggle flag
	}
	
	// Handle shiftL
	else if(scode == SLM || scode == SLB){
		shiftFlagL = (shiftFlagL+1)%MOD2;	// Toggle flag
	}
	
	// Handle shiftR
	else if(scode == SRM || scode == SRB){
		shiftFlagR = (shiftFlagR+1)%MOD2;	// Toggle flag
	}
	
	// Handle backspace
	else if(scode == BKSP){
		// Move cursor back, erase, and remove from buffer
		if(buf_loc[currentTerminal] != 0){
			buf_loc[currentTerminal]--;
			KBbuf[currentTerminal][buf_loc[currentTerminal]] = 0;
			
			// Erase on screen
			erase_char();
		}
	}
	
	// Characters to output from ascii table
	else if((scode & BIT8) == 0 && ascii_scan[scode]!=0 && scode < SIZE_SCODES && (buf_loc[currentTerminal] < BUF_SIZE-1 || scode == ENTER)){
		char c = ascii_scan[scode + capsFlag*CAPS_OFF + (shiftFlagL | shiftFlagR)*SHIFT_OFF];
		KBputc(c);
		
		// Newline will clear buffer or get ready for reading, other values just populate buffer
		if(c == '\n'){
			if(readWaiting[currentTerminal]){
				KBbuf[currentTerminal][buf_loc[currentTerminal]] = c;
				buf_loc[currentTerminal]++;
				readReady[currentTerminal] = 1;
			}else{
				// Clear buffer
				while(buf_loc[currentTerminal] > 0){
					buf_loc[currentTerminal]--;
					KBbuf[currentTerminal][buf_loc[currentTerminal]] = 0;
				}
			}
		}else{
			// Add to buffer
			KBbuf[currentTerminal][buf_loc[currentTerminal]] = c;
			buf_loc[currentTerminal]++;
		}
	}
	
	
	// end of interrupt signal
	send_eoi(KB_IRQ);

	// restore general purpose register, return
	asm volatile ("popa");
	asm volatile ("leave");
	asm volatile ("iret");
}

/*
 * terminalOpen
 * DESCRIPTION: Called when we want to start using terminal. 
 * INPUT: filename - ignored. 
 * OUTPUT: 0 on success. 
 * SIDE_EFFECTS: clears input buffer and terminal driver flags. 
 */
extern int32_t terminalOpen(const uint8_t* filename){
	int i,j;

	//The default terminal will be terminal 0
	currentTerminal = TERMINAL_0;

	// Clearing flags and buffers
	capsFlag = 0;
	shiftFlagL = 0;
	shiftFlagR = 0;
	ctrlFlagL = 0;
	ctrlFlagR = 0;
	altFlagL = 0;
	altFlagR = 0;
	for(j=0; j<NUM_TERMINALS; j++) {
		readWaiting[j] = 0;
		readReady[j] = 0;
		buf_loc[j] = 0;
		for(i=0; i<BUF_SIZE; i++){
			KBbuf[j][i] = 0;
		}
	}	
	return 0;
}

/*
 * terminalClose
 * DESCRIPTION: Called when we are done with terminal. 
 * INPUT: fd - ignored. 
 * OUTPUT: 0 on success. 
 * SIDE_EFFECTS: none. 
 */
extern int32_t terminalClose(int32_t fd){
	return 0;
}

/*
 * terminalRead
 * DESCRIPTION: Reads one line from the input buffer. 
 * INPUT: 	fd - ignored
			buf - buffer to read into
			nbytes - size of buffer
 * OUTPUT: none.
 * SIDE_EFFECTS: reads one line into the specified buffer, but reads less if the buffer gets filled
 */
extern int32_t terminalRead(int32_t fd, void* buf, int32_t nbytes){
	if(nbytes < 0)
		return -1;
	readWaiting[PCB_ptrs[cur_pid] -> parent_terminal] = 1;
	while(!readReady[PCB_ptrs[cur_pid] -> parent_terminal]){
		;
	}

	int bytes;
	int flags;

	cli_and_save(flags);
	
	for(bytes = 0; bytes<nbytes; bytes++){
		((char *)buf)[bytes] = KBbuf[PCB_ptrs[cur_pid] -> parent_terminal][bytes];
		if(((char *)buf)[bytes] == '\n'){
			bytes++;
			break;
		}
	}
	memset(KBbuf[PCB_ptrs[cur_pid] -> parent_terminal], 0, BUF_SIZE);
	readReady[PCB_ptrs[cur_pid] -> parent_terminal] = 0;
	readWaiting[PCB_ptrs[cur_pid] -> parent_terminal] = 0;
	buf_loc[PCB_ptrs[cur_pid] -> parent_terminal] = 0;
	restore_flags(flags);
	return bytes;
}

/*
 * terminalWrite
 * DESCRIPTION: none.
 * INPUT: 	fd - ignored
			buf - buffer to write from
			nbytes - bytes to write
 * OUTPUT: none.
 * SIDE_EFFECTS: none. 
 */
extern int32_t terminalWrite(int32_t fd, const void* buf, int32_t nbytes){
	int i;
	unsigned int flags;
	cli_and_save(flags);
	for(i=0; i<nbytes; i++){
		putc(((char *)buf)[i]);		
	}
	restore_flags(flags);
	return nbytes;
}

