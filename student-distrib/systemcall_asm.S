#define ASM     1

#define USER_CS 0x0023
#define USER_DS 0x002B
#define MAX_SYSCALL_NUM 10
#define ERROR_VAL -1
#define FIRST_ARG_OFFSET 4
#define USER_ESP 0x83FFFFC
#define IF_MASK 0x200
#define THREE_LONGS 12


.text

#some global functions
.globl 	syscall
.globl	switch_to_user_mode
.globl 	get_eip


# syscall
# DESCRIPTION: Dispatcher function for system calls
# INPUT: int system call number - stored directly into eax
# OUTPUT: returns -1 if invalid system call number; else returns output of system call handler
# SIDE_EFFECTS: Calls a system handler
# 
syscall:
	#initial bookeeping procedure

	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esi
	pushl %edi

	#check for valid syscall number
	cmpl $MAX_SYSCALL_NUM, %eax
	ja invalid_syscall_num
	
	cmpl $0x0, %eax
	je invalid_syscall_num

	#push args to stack
	pushl %edx
	pushl %ecx
	pushl %ebx
	call *jmp_table(, %eax, 4)	#make the dispatch call

	#teardown stack
	addl $THREE_LONGS, %esp
	popl %edi
	popl %esi
	popl %edx
	popl %ecx
	popl %ebx
	iret

	#Return error for invalid system call number
	invalid_syscall_num: 
		movl $ERROR_VAL, %eax
		popl %edi
		popl %esi
		popl %edx
		popl %ecx
		popl %ebx
		iret

#Jump table for system call handler functions
jmp_table: 
	.long 	0x0
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


# switch_to_user_mode
# DESCRIPTION: Set up our iret context and switch to user mode
# INPUT: uint32_t program_eip - correct eip for program to execute in user mode
# OUTPUT: none
# SIDE_EFFECTS: Pushes iret context to stack and calls iret to enter user mode
# 
switch_to_user_mode:
	
	#mask interrupts
	cli

	#move program_eip into ECX
	movl FIRST_ARG_OFFSET(%esp), %ecx
	
	#Store user data segment in ds
	mov $USER_DS, %eax
	mov %ax, %ds

	#push the user data segment
	pushl $USER_DS

	#push the address of the bottom of the program page
	pushl $USER_ESP
	
	#reenable IF and push to stack
	pushf
	popl %eax
	orl $IF_MASK, %eax
	pushl %eax
	
	#push user code segment to stack
	pushl $USER_CS

	#push program eip to stack
	push %ecx
	iret

get_eip:
	movl (%esp), %eax
	ret
