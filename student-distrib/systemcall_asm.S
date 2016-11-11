
//#include "systemcall.h"
#define USER_CS 0x0023
#define USER_DS 0x002B
#define MAX_SYSCALL_NUM 9
#define RET_VAL_OFFSET 32
#define ERROR_VAL -1
#define FIRST_ARG_OFFSET 8

.text

.globl 	syscall
.globl	switch_to_user_mode
.globl 	sys_halt
.globl  sys_execute
.globl  sys_read
.globl	sys_write
.globl	sys_open
.globl	sys_close
.globl	sys_getargs
.globl	sys_vidmap
.globl	sys_sethandler
.globl	sys_sigreturn

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
	call *jmp_table(, %eax, 4)	#make the dispatch call

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
	.long 	syscall
	.long	switch_to_user_mode
	.long 	sys_halt
	.long   sys_execute
	.long   sys_read
	.long	sys_write
	.long	sys_open
	.long	sys_close
	.long	sys_getargs
	.long	sys_vidmap
	.long	sys_sethandler
	.long	sys_sigreturn


#Set up our iret context and switch to user mode
switch_to_user_mode:

	#initial bookeeping procedure
	pushl %ebp
	movl %esp, %ebp

	cli
	movl FIRST_ARG_OFFSET(%ebp), %ecx
	popl %ebp
	mov $USER_DS, %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	mov %esp, %eax
	pushl $USER_DS
	pushl %eax
	pushf
	pushl $USER_CS
	push %ecx
	iret
