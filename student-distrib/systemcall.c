#include "systemcall.h"
/* Set up our stack to prepare for switching to user mode */
void switch_to_user_mode()
{
   asm volatile("  \ 
     cli; \ 
     mov $USER_DS, %ax; \ 
     mov %ax, %ds; \ 
     mov %ax, %es; \ 
     mov %ax, %fs; \ 
     mov %ax, %gs; \ 
                   \ 
     mov %esp, %eax; \ 
     pushl $USER_CS; \ 
     pushl %eax; \ 
     pushf; \ 
     pushl $0x1B; \ 
     push $1f; \ 
     iret; \ 
   1: \ 
     ");
}