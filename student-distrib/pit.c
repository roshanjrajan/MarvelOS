#include "pit.h"

/* 
 * PITwrapper
 *
 * DESCRIPTION: wrapper function for the PIT interrupt handler
 * INPUT: NONE
 * OUTPUT: NONE
 * SIDE_EFFECTS: calls PIThandler
 */
void PITwrapper(){
	// As interrupt, save all general purpose registers
	asm volatile ("pusha");
	
	PIThandler();

	// restore general purpose registers	
	asm volatile ("popa");

	asm volatile ("leave");
	asm volatile ("iret");
}

/* 
 * PIThandler
 *
 * DESCRIPTION: Interrupt handler for the PIT
 * INPUT: NONE
 * OUTPUT: NONE
 * SIDE_EFFECTS: changes the curThread and cur_pid global variables
 */
void PIThandler(){
	
	// Chck if shell has been started for current thread yet and take appropriate action
	if(shellStarted[curThread] == 0){
		cur_pid = -1;
		
		shellStarted[curThread] = 1;
		
		// Send EOI
		send_eoi(PIT_IRQ);
		
		sys_execute((uint8_t *)"shell");
		return;
	}
		
	// store current esp and ebp
	asm("movl %%esp, %0;"
		:"=r"(ESPstore[curThread])
		:
		:"memory");

	asm("movl %%ebp, %0;"
		:"=r"(EBPstore[curThread])
		:
		:"memory");
		
	// store current PID
	PIDstore[curThread] = cur_pid;
	
	// We are moving to the next thread
	curThread = (curThread+1)%NUM_THREADS;
	
	// Check if thread being switched to has had shell execute yet
	if(shellStarted[curThread] == 0){
		cur_pid = -1;
		
		shellStarted[curThread] = 1;
		
		// Send EOI
		send_eoi(PIT_IRQ);
		
		sys_execute((uint8_t *)"shell");
		return;
	}

	// update cur_pid to reflect the thread switch
	cur_pid = PIDstore[curThread];

	//Update our global fdt pointer
	fdt = PCB_ptrs[cur_pid]->process_fdt;

	//Update the physical address for the program
	page_directory[PROCESS_PAGING_INDEX].page_table_address = (PROCESS_BASE_4KB_ALIGNED_ADDRESS + cur_pid * FOUR_MB)>> PDE_PTE_ADDRESS_SHIFT;
	
	//Flush the TLB for the new program
	clearTLB();

	// Perform context switch
 	tss.esp0 = (PROCESS_BASE_4KB_ALIGNED_ADDRESS - cur_pid * EIGHT_KB) - LONG_BYTES;//update the process's kernel-mode stack pointer

	// Send EOI
	send_eoi(PIT_IRQ);
		
	// Populate ESP/EBP with values for task to switch to
	asm("movl %0, %%esp;"
		:
		:"r"(ESPstore[curThread])
		:"memory");
	
	asm("movl %0, %%ebp;"
		:
		:"r"(EBPstore[curThread])
		:"memory");
		
	return;
}


/* 
 * PITinit
 *
 * DESCRIPTION: initialize the PIT device.
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: PIT will start producing periodic interrupts 
 */
void PITinit(){
	// Set up values to indicate starting thread and that no shells have been started
	int i;
	for(i=0; i<NUM_THREADS; i++){
		shellStarted[i]=0;
	}
	curThread = 0;

	// Set up PIT device to actually generate interrupts
	outb(PIT_MODE_SET, PIT_MODE_REG);
	
	uint32_t pit_val = PIT_BASE_RATE/PIT_RATE;
	outb(pit_val & LOW_8, PIT_DATA_REG);
	outb(pit_val >> BITS_8, PIT_DATA_REG);
	
	return;
}

