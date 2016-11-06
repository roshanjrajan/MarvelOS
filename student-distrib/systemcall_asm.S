
#include "systemcall.h"
#define USER_CS 0x0023
#define USER_DS 0x002B
#define MAX_SYSCALL_NUM 9
#define RET_VAL_OFFSET 32
#define ERROR_VAL -1

.text

.globl 	syscall
.globl	switch_to_user_mode
.globl 	sys_halt, 
		sys_execute,
		sys_read,
		sys_write,
		sys_open,
		sys_close,
		sys_getargs,
		sys_vidmap,
		sys_sethandler,
		sys_sigreturn

#Dispatcher function for system calls
syscall:
	#initial bookeeping procedure
	pushl %ebp
	movl %esp, %ebp
	pusha

	#check for valid syscall number
	cmpl $MAX_SYSCALL_NUM, %eax
	ja invalid_syscall_num

	#push args to stack
	pushl %edx
	pushl %ecx
	pushl %ebx
	call jmp_table(, %eax, 4)	#make the dispatch call

	#teardown stack
	addl $12, %esp
	#movl %eax, RET_VAL_OFFSET(%ebp)
	popa
	leave
	iret

#Function for returning error for invalid system call number
invalid_syscall_num: 
	movl $ERROR_VAL, %eax
	#movl %eax, RET_VAL_OFFSET(%ebp)
	popa
	leave
	iret

jmp_table: 
	.long 	sys_halt, 
			sys_execute,
			sys_read,
			sys_write,
			sys_open,
			sys_close,
			sys_getargs,
			sys_vidmap,
			sys_sethandler,
			sys_sigreturn


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

