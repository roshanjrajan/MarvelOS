/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

// logic high mask for PIC initialization
#define MASK_BYTE_HIGH	0xFF

/* 
 * i8259_init
 *
 * DESCRIPTION: Initialize the 8259 PIC: Send 4 control
 * words to the master and slave, following the necessary
 * initialization sequence.
 * INPUTS: none.
 * OUTPUS: none.
 * SIDE_EFFECTS: PIC masks initialized to zeroes
 */
void
i8259_init(void)
{
	/* Perform initialization sequence with PICs */

	/* ICW 1 */
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW1, SLAVE_8259_PORT);

	/* ICW 2 */
	outb(ICW2_MASTER, MASTER_8259_PORT_DATA);
	outb(ICW2_SLAVE, SLAVE_8259_PORT_DATA);

	/* ICW 3 */
	outb(ICW3_MASTER, MASTER_8259_PORT_DATA);
	outb(ICW3_SLAVE, SLAVE_8259_PORT_DATA);

	/* ICW 4 */
	outb(ICW4, MASTER_8259_PORT_DATA);
	outb(ICW4, SLAVE_8259_PORT_DATA);

	/* Initialize our masks to zeros by default */
	master_mask = MASK_BYTE_HIGH;
	slave_mask = MASK_BYTE_HIGH;

	outb(MASK_BYTE_HIGH, MASTER_8259_PORT_DATA);
	outb(MASK_BYTE_HIGH, SLAVE_8259_PORT_DATA);
}


/* 
 * disable_irq
 *
 * DESCRIPTION: Disable (mask) the specified IRQ
 * line, 0 - 7, 8 IR lines total.
 * INPUTS: none.
 * OUTPUS: none.
 * SIDE_EFFECTS: none.
 */
void
disable_irq(uint32_t irq_num)
{
	/* Check for invalid IRQ number, should be 0 - 15 */
	if((irq_num < 0 ) || (irq_num > IRQ_MAX)) {
		return;
	}

	/* Use the Master PIC */
	if(irq_num < NUM_IRQS_PER_PIC) {
		master_mask = master_mask | (1 << irq_num);
		outb(master_mask, MASTER_8259_PORT_DATA);

	}

	/* Use the Slave PIC */
	else {
		irq_num -= NUM_IRQS_PER_PIC;
		slave_mask = slave_mask | (1 << irq_num);
		outb(slave_mask, SLAVE_8259_PORT_DATA);
	}	
}


/* 
 * enable_irq
 *
 * DESCRIPTION: Enable (unmask) the specified IRQ
 * line, 0 - 7, 8 IR lines total.
 * INPUTS: none.
 * OUTPUS: none.
 * SIDE_EFFECTS: none.
 */
void
enable_irq(uint32_t irq_num)
{
	/* Check for invalid IRQ number */
	if((irq_num < 0 ) || (irq_num > IRQ_MAX)) {
		return;
	}

	/* Use the Master PIC */
	if(irq_num < NUM_IRQS_PER_PIC) {
		master_mask = master_mask & (~(1 << irq_num));
		outb(master_mask, MASTER_8259_PORT_DATA);
	}
	
	/* Use the Slave PIC */
	else {
		irq_num -= NUM_IRQS_PER_PIC;
		slave_mask = slave_mask & (~(1 << irq_num));
		outb(slave_mask, SLAVE_8259_PORT_DATA);
	}
}


/* 
 * send_eoi
 *
 * DESCRIPTION: Send end-of-interrupt signal 
 * for the specified IRQ line, 0 - 7, 8 IR lines total.
 * INPUTS: none.
 * OUTPUS: none.
 * SIDE_EFFECTS: none.
 */
void
send_eoi(uint32_t irq_num)
{
	unsigned char port_output; 

	/* Check for invalid IRQ number */
	if((irq_num < 0 ) || (irq_num > IRQ_MAX)) {
		return;
	}
	
	/* Use the Master PIC */
	if(irq_num < NUM_IRQS_PER_PIC) {
		port_output = irq_num | EOI;
		outb(port_output, MASTER_8259_PORT);
	}

	/* Use the Slave PIC */
	else {
		/* Send EOI for IRQ channel 2 on Master PIC */
		port_output = SLAVE_IRQ_PIN_ON_MASTER | EOI;
		outb(port_output, MASTER_8259_PORT);

		/* Send EOI from Slave PIC */
		irq_num -= NUM_IRQS_PER_PIC;
		port_output = irq_num | EOI;
		outb(port_output, SLAVE_8259_PORT);
	}	
}

