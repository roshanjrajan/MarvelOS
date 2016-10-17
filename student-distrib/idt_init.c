#include "idt_init.h"

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
 * eh0
 * DESCRIPTION: Exception handler 0 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh0(){
	cli();
	printf("EXCEPTION: DIVIDE ERROR");
	while(1);
}

/*
 * eh2
 * DESCRIPTION: Exception handler 2 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh2(){
	cli();
	printf("EXCEPTION: NMI INTERRUPT");
	while(1);
}

/*
 * eh3
 * DESCRIPTION: Exception handler 3 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh3(){
	cli();
	printf("EXCEPTION: BREAKPOINT");
	while(1);//
}

/*
 * eh4
 * DESCRIPTION: Exception handler 4 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh4(){
	cli();
	printf("EXCEPTION: OVERFLOW");
	while(1);
}

/*
 * eh5
 * DESCRIPTION: Exception handler 5 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh5(){
	cli();
	printf("EXCEPTION: BOUND RANGE EXCEEDED");
	while(1);
}

/*
 * eh6
 * DESCRIPTION: Exception handler 6 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh6(){
	cli();
	printf("EXCEPTION: INVALID OR UNDEFINED OPCODE");
	while(1);
}

/*
 * eh7
 * DESCRIPTION: Exception handler 7 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh7(){
	cli();
	printf("EXCEPTION: DEVICE NOT AVAILABLE");
	while(1);
}

/*
 * eh8
 * DESCRIPTION: Exception handler 8 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine* INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh8(){
	cli();
	printf("EXCEPTION: DOUBLE FAULT");
	while(1);
}

/*
 * eh9
 * DESCRIPTION: Exception handler 9 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh9(){
	cli();
	printf("EXCEPTION: COPROCESSOR SEGMENT OVERRUN");
	while(1);
}

/*
 * eh10
 * DESCRIPTION: Exception handler 10 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh10(){
	cli();
	printf("EXCEPTION: INVALID TSS");
	while(1);
}

/*
 * eh11
 * DESCRIPTION: Exception handler 11 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh11(){
	cli();
	printf("EXCEPTION: SEGMENT NOT PRESENT");
	while(1);
}

/*
 * eh12
 * DESCRIPTION: Exception handler 12 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh12(){
	cli();
	printf("EXCEPTION: STACK SEGMENT FAULT");
	while(1);
}

/*
 * eh13
 * DESCRIPTION: Exception handler 13 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh13(){
	cli();
	printf("EXCEPTION: GENERAL PROTECTION");
	while(1);
}

/*
 * eh14
 * DESCRIPTION: Exception handler 14 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh14(){
	cli();
	printf("EXCEPTION: PAGE FAULT");
	while(1);
}

/*
 * eh16
 * DESCRIPTION: Exception handler 16 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh16(){
	cli();
	printf("EXCEPTION: X87 FPU FLOATING POINT ERROR");
	while(1);
}

/*
 * eh17
 * DESCRIPTION: Exception handler 17 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh17(){
	cli();
	printf("EXCEPTION: ALIGNMENT CHECK");
	while(1);
}
/*
 * eh18
 * DESCRIPTION: Exception handler 18 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh18(){
	cli();
	printf("EXCEPTION: MACHINE CHECK");	
	while(1);
}

/*
 * eh19
 * DESCRIPTION: Exception handler 19 as specified in the IDT
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh19(){
	cli();
	printf("EXCEPTION: SIMD FLOATING POINT EXCEPTION");
	while(1);
}

/*
 * init_exceptions
 *
 * DESCRIPTION: Initialize Intel-defined exceptions in IDT, using 
 * structures and functions defined in x86_desc.h
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void init_exceptions(){
	/* Allocate memory for first exception entries in IDT */
	int i;
	for(i=0; i<NUM_EXCEPTIONS; i++){
		if(i != RESERVED_1 && i<RESERVED_20 && i != RESERVED_15){
			idt[i].seg_selector = KERNEL_CS;
			idt[i].reserved4 = 0;
			idt[i].reserved3 = 1;
			idt[i].reserved2 = 1;
			idt[i].reserved1 = 1;
			idt[i].size = 1;
			idt[i].reserved0 = 0;
			idt[i].dpl = 0;
			idt[i].present = 1;
		}else{
			idt[i].seg_selector = KERNEL_CS;
			idt[i].reserved4 = 0;
			idt[i].reserved3 = 1;
			idt[i].reserved2 = 1;
			idt[i].reserved1 = 1;
			idt[i].size = 1;
			idt[i].reserved0 = 1;
			idt[i].dpl = 0;
			idt[i].present = 0;
		}
	}
	
	/* Initialize pointers to exception handlers defined in this file */
	SET_IDT_ENTRY(idt[0], eh0);
	SET_IDT_ENTRY(idt[2], eh2);
	SET_IDT_ENTRY(idt[3], eh3);
	SET_IDT_ENTRY(idt[4], eh4);
	SET_IDT_ENTRY(idt[5], eh5);
	SET_IDT_ENTRY(idt[6], eh6);
	SET_IDT_ENTRY(idt[7], eh7);
	SET_IDT_ENTRY(idt[8], eh8);
	SET_IDT_ENTRY(idt[9], eh9);
	SET_IDT_ENTRY(idt[10], eh10);
	SET_IDT_ENTRY(idt[11], eh11);
	SET_IDT_ENTRY(idt[12], eh12);
	SET_IDT_ENTRY(idt[13], eh13);
	SET_IDT_ENTRY(idt[14], eh14);
	SET_IDT_ENTRY(idt[16], eh16);
	SET_IDT_ENTRY(idt[17], eh17);
	SET_IDT_ENTRY(idt[18], eh18);
	SET_IDT_ENTRY(idt[19], eh19);
}

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
 * RTC_init
 *
 * DESCRIPTION: initialize the RTC device.
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: RTC device will start producing periodic interrupts. 
 */
void RTC_init(){
	// Enable periodic interrupt (bit 6 in reg B)
	outb(BIT8+RTC_B, RTC_PORT);
	char prev = inb(RTC_DATA);
	outb(BIT8+RTC_B, RTC_PORT);
	outb(prev | BIT6_MASK, RTC_DATA);
	
	// Changing interrupt rate (lower four bits in reg A)
	outb(BIT8+RTC_A, RTC_PORT);
	prev = inb(RTC_DATA);
	outb (BIT8+RTC_A, RTC_PORT);
	outb((prev & HI4) | LOW4, RTC_DATA);
	
	outb(inb(RTC_PORT)&(~BIT8), RTC_PORT);	// Enable NMI
	
	// Read data from reg C
	outb(RTC_C, RTC_PORT);
	inb(RTC_DATA);
}

/* 
 * RTChandler
 *
 * DESCRIPTION: Interrupt handler for the RTC device.
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void RTChandler(){
	// As interrupt, save all general purpose registers
	asm volatile ("pusha");
	
	outb(RTC_C, RTC_PORT);
	inb(RTC_DATA);
	test_interrupts();
	send_eoi(RTC_IRQ);
	
	// restore general purpose register, return	
	asm volatile ("popa");
	asm volatile ("leave");
	asm volatile ("iret");
}

/* 
 * init_interrupts
 *
 * DESCRIPTION: Initialize interrupts section in IDT
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void init_interrupts(){
	// allocate space in IDT just for interrupts, after exceptions
	int i; 
	for(i=NUM_EXCEPTIONS; i<NUM_INTERRUPTS+NUM_EXCEPTIONS; i++){
		idt[i].seg_selector = KERNEL_CS;
		idt[i].reserved4 = 0;
		idt[i].reserved3 = 0;
		idt[i].reserved2 = 1;
		idt[i].reserved1 = 1;
		idt[i].size = 1;
		idt[i].reserved0 = 0;
		idt[i].dpl = 0;
		idt[i].present = 0;
	}
	
	// put keyboard handler into IDT
	SET_IDT_ENTRY(idt[KB_IRQ+NUM_EXCEPTIONS], KBhandler);
	idt[KB_IRQ+NUM_EXCEPTIONS].present = 1;
	
	// put RTC handler into IDT
	SET_IDT_ENTRY(idt[RTC_IRQ+NUM_EXCEPTIONS], RTChandler);
	idt[RTC_IRQ+NUM_EXCEPTIONS].present = 1;
}

/* 
 * syscall
 *
 * DESCRIPTION: System call handler - not yet implemented
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void syscall(){
	/* TODO */
	cli();
	printf("System Call Handler not yet implemented!");
	while(1);
}

/*
 * init_syscall
 * DESCRIPTION: System call initialization in IDT
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void init_syscall(){
	// enter data for single system call entry in IDT
	idt[SCALL_LOC].seg_selector = KERNEL_CS;
	idt[SCALL_LOC].reserved4 = 0;
	idt[SCALL_LOC].reserved3 = 1;
	idt[SCALL_LOC].reserved2 = 1;
	idt[SCALL_LOC].reserved1 = 1;
	idt[SCALL_LOC].size = 1;
	idt[SCALL_LOC].reserved0 = 0;
	idt[SCALL_LOC].dpl = USR_DPL;
	idt[SCALL_LOC].present = 1;
	SET_IDT_ENTRY(idt[SCALL_LOC], syscall);
}

/*
 * init_idt
 * DESCRIPTION: Initialize the IDT in parts defined in this file
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void init_idt(){
	init_exceptions();
	init_interrupts();
	init_syscall();
	lidt(idt_desc_ptr);
}
