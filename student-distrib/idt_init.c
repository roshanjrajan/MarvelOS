#include "idt_init.h"

/*
 * eh0
 * DESCRIPTION: Exception handler 0 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh0(){
	printf("EXCEPTION: DIVIDE ERROR");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: NMI INTERRUPT");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: BREAKPOINT");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: OVERFLOW");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: BOUND RANGE EXCEEDED");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: INVALID OR UNDEFINED OPCODE");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: DEVICE NOT AVAILABLE");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
}

/*
 * eh8
 * DESCRIPTION: Exception handler 8 as specified in the IDT
 * mask interrupts, infinite while loop to freeze machine* INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh8(){
	printf("EXCEPTION: DOUBLE FAULT");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: COPROCESSOR SEGMENT OVERRUN");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: INVALID TSS");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: SEGMENT NOT PRESENT");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: STACK SEGMENT FAULT");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: GENERAL PROTECTION");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: PAGE FAULT");

	uint32_t* cr2_val;
	asm("movl %%cr2, %0;"
		:"=r"(cr2_val)
		:
		:"memory");
	printf("\nCR2_val:           %x", cr2_val);
	while(1);



	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: X87 FPU FLOATING POINT ERROR");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: ALIGNMENT CHECK");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
	printf("EXCEPTION: MACHINE CHECK");	
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
}

/*
 * eh19
 * DESCRIPTION: Exception handler 19 as specified in the IDT
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: none.
 */
void eh19(){
	printf("EXCEPTION: SIMD FLOATING POINT EXCEPTION");
	PCB_ptrs[cur_pid]->exception_flag = 1;
	sys_halt(255);
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
			idt[i].reserved3 = 0;
			idt[i].reserved2 = 1;
			idt[i].reserved1 = 1;
			idt[i].size = 1;
			idt[i].reserved0 = 0;
			idt[i].dpl = 0;
			idt[i].present = 1;
		}else{
			idt[i].seg_selector = KERNEL_CS;
			idt[i].reserved4 = 0;
			idt[i].reserved3 = 0;
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
		idt[i].reserved3 = 1;
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
