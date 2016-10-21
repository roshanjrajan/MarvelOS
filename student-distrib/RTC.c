#include "RTC.h"

volatile int RTC_read_flag = 0;	// Flag used during RTC reading

/* 
 * RTCOpen
 *
 * DESCRIPTION: Set the RTC device periodic interrupt frequency to 2Hz(default)
 * INPUT: none.
 * OUTPUT: 0 - succes, -1 - failure
 * SIDE_EFFECTS: RTC device will produce periodic interrupts at 2Hz. 
 */
int32_t RTCOpen(){
	return RTCWrite(INIT_FREQ);	// Default 2Hz
}

/* 
 * RTCRead
 *
 * DESCRIPTION: Block until next periodic interrupt from the RTC device. 
 * INPUT: none.
 * OUTPUT: 0 on success
 * SIDE_EFFECTS: 
 */
int32_t RTCRead(){
	RTC_read_flag = 1;
	while(RTC_read_flag)
		;
	return 0;
}

/* 
 * RTCWrite
 *
 * DESCRIPTION: Change the frequency of the periodic interrupt from the RTC device.
 * INPUT: freq - desired frequency (must be power of 2 between 2 and 1024)
 * OUTPUT: 0 on succes, -1 on failure
 * SIDE_EFFECTS: RTC device will start producing periodic interrupts at specified frequency(if valid). 
 */
int32_t RTCWrite(int32_t freq){
	if(freq >=MIN_FREQ && freq <= MAX_FREQ){
		int bitFlag = 0;
		uint32_t i;
		for(i = 1; i!= 0; i = (i << 1)){
			if(freq & i){
				bitFlag++;
			}
		}
		if(bitFlag == 1){
			// Do write to RTC device
			/*************/
			
			uint8_t rate = 1;
			uint16_t i;
			for(i = BASE_FREQ; i>freq; i = (i >> 1)){
				rate++;
			}
			// Changing interrupt rate (lower four bits in reg A)
			outb(BIT8+RTC_A, RTC_PORT);
			char prev = inb(RTC_DATA);
			outb (BIT8+RTC_A, RTC_PORT);
			outb((prev & HI4) | rate, RTC_DATA);
			
			outb(inb(RTC_PORT)&(~BIT8), RTC_PORT);	// Enable NMI
			return 0;
		}
	}
	return -1;
}

/* 
 * RTCClose
 *
 * DESCRIPTION: close the RTC device.
 * INPUT: none.
 * OUTPUT: returns 0
 * SIDE_EFFECTS: none. 
 */
int32_t RTCClose(){
	return 0;
}

/* 
 * RTC_init
 *
 * DESCRIPTION: initialize the RTC device.
 * INPUT: none.
 * OUTPUT: none.
 * SIDE_EFFECTS: RTC device will start producing periodic interrupts at 2Hz. 
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
	RTC_read_flag = 0;
	test_interrupts();
	send_eoi(RTC_IRQ);
	
	// restore general purpose register, return	
	asm volatile ("popa");
	asm volatile ("leave");
	asm volatile ("iret");
}
