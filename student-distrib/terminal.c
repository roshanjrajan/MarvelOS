#include "terminal.h"

/*
 * ASCII scan codes for keyboard usage 
 */
static char ascii_scan[SIZE_SCODES] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * Keyboard buffer array
 */
static char KBbuf[BUF_SIZE];

static int upperFlag;

/*
 * KBhandler
 * Handler for keyboard input.
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: Prints typed character to the screen. 
 */
void KBhandler(){
	// As interrupt, save all general purpose registers
	asm volatile ("pusha");
	
	// select and push ascii code from table
	uint32_t scode;
	scode = inb(KB_DATA_PORT);
	if(ascii_scan[scode] != 0)
		putc(ascii_scan[scode]);
	
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
 * INPUT: none.
 * OUTPUT: 0 on success. 
 * SIDE_EFFECTS: clears input buffer. 
 */
extern int32_t terminalOpen(){
	int i;
	// Clearing buffer
	for(i=0; i<BUF_SIZE; i++){
		KBbuf[i] = 0;
	}
	return 0;
}

/*
 * terminalClose
 * DESCRIPTION: none.
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none. 
 */
extern int32_t terminalClose(){
	return 0;
}

/*
 * terminalRead
 * DESCRIPTION: none.
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none. 
 */
extern int32_t terminalRead(char *buf){
	return -1;
}

/*
 * terminalWrite
 * DESCRIPTION: none.
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none. 
 */
extern int32_t terminalWrite(char *buf, int32_t bytes){
	int i;
	unsigned int flags;
	cli_and_save(flags);
	for(i=0; i<bytes; i++){
		putc(buf[i]);
	}
	restore_flags(flags);
}

