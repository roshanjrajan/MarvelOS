# paging.s - set up page directory and page tables

#define ASM     1

#define MASK_B31 0x80000000
#define MASK_B4  0x00000010
#define PARAM_OFF 8

.text

#some global functions
.globl loadPageDirectory
.globl enablePaging
.globl enableMixedSizes
.globl clearTLB

#  
#  loadPageDirectory
# 
#  DESCRIPTION: Move page directory address into CR3
#  INPUTS: none.
#  OUTPUS: none.
#  SIDE_EFFECTS:  Page directory address is changed
#  
loadPageDirectory:
	pushl %ebp
	movl %esp, %ebp
	movl PARAM_OFF(%esp), %eax
	movl %eax, %cr3
	leave
	ret

#
#  enablePaging
# 
#  DESCRIPTION: Enable Paging
#  INPUTS: none.
#  OUTPUS: none.
#  SIDE_EFFECTS:  Paging is enabled
#  
enablePaging:
	pushl %ebp
	movl %esp, %ebp
	movl %cr0, %eax
	orl $MASK_B31, %eax
	movl %eax, %cr0
	leave
	ret

#
#  enableMixedSizes
# 
#  DESCRIPTION: Enable mixed size pages (i.e. both 4kB and 4MB pages)
#  INPUTS: none.
#  OUTPUS: none.
#  SIDE_EFFECTS:  Enabled different size pages
# 
enableMixedSizes: 
	pushl %ebp
	movl %esp, %ebp
	movl %cr4, %eax
	orl $MASK_B4, %eax
	movl %eax, %cr4
	leave
	ret

#
#  clear the TLB
# 
#  DESCRIPTION: Used to clear the Translation Lookaside Buffer
#  INPUTS: none.
#  OUTPUS: none.
#  SIDE_EFFECTS:  Flushed the TLB on paging structure change
# 
clearTLB:
	pushl %ebp
	movl %esp, %ebp
	movl %cr3, %eax
	movl %eax, %cr3
	leave
	ret
