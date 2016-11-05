
#define KERNEL_TSS 0x0030
#define USER_CS 0x0023
#define USER_DS 0x002B

.text

.globl switch_to_user_mode
.globl flush_tss

#Set up our stack to prepare for switching to user mode
switch_to_user_mode:
	cli
	mov $USER_DS, %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	mov %esp, %eax

	pushl %eax
	pushf
	pushl $USER_CS
	push $1F
	iret
	1:

flush_tss: 
	mov KERNEL_TSS, %eax
	ltr %eax
	ret


