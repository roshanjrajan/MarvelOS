#include "pit.h"

void PIThandler(){
	// As interrupt, save all general purpose registers
	asm volatile ("pusha");
	
	
	
	// Chck if shell has been started for current thread yet and take appropriate action
	if(shellStarted[curThread] == 0){
		cur_pid = -1;
		
		shellStarted[curThread] = 1;
		
		// Send EOI
		send_eoi(PIT_IRQ);
		
		sys_execute((uint8_t *)"shell");
		return;
	}
	
	//int prevT = curThread;
	//int nextT = (curThread+1)%NUM_THREADS;
	
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

	// Perform context switch
 	tss.esp0 = (PROCESS_BASE_4KB_ALIGNED_ADDRESS - cur_pid * EIGHT_KB) - LONG_BYTES;//update the process's kernel-mode stack pointer

	// Send EOI
	send_eoi(PIT_IRQ);
	
	// restore general purpose registers	
	asm volatile ("popa");
	
	// Populate ESP/EBP with values for task to switch to
	asm("movl %0, %%esp;"
		:
		:"r"(ESPstore[curThread])
		:"memory");
	
	asm("movl %0, %%ebp;"
		:
		:"r"(EBPstore[curThread])
		:"memory");
		
	//return
	asm volatile ("leave");
	asm volatile ("iret");
}

void PITinit(){
	// Set up values to indicate starting thread and that no shells have been started
	int i;
	for(i=0; i<NUM_THREADS; i++){
		shellStarted[i]=0;
	}
	curThread = 0;

	// Set up PIT device to actually generate interrupts
	outb(0x34, 0x43);
	
	uint32_t pit_val = 1193182/PIT_RATE;
	outb(pit_val & 0xff, 0x40);
	outb(pit_val >> 8, 0x40);
	
	return;
}

