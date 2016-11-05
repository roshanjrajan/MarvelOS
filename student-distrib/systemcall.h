include "x86_desc.h"

struct tss_entry
{
   uint16_t reserved1;
   uint16_t linked_list;   //when using hardware task switching, this used to form linked list
   uint32_t esp0;          // Stack pointer for kernel mode 
   uint16_t reserved2;
   uint16_t ss0;           //stack segment for when swirthcing to kernel mode
   uint32_t esp1;          //stack pointer for privilege level 1 (unused)
   uint16_t reserved3;
   uint16_t ss1;           //stack segment for privilege level 1 (unused)
   uint32_t esp2;          //stack pointer for privilege level 2 (unused)
   uint16_t reserved4;
   uint16_t ss2;           //stack segment for privilege level 1 (unused)
   uint32_t cr3;           //Control Register 3 (holds address of page directory)
   uint32_t eip;           //Instruction pointer
   uint32_t eflags;        //flags
   uint32_t eax;           
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint16_t reserved5;
   uint16_t es;            
   uint16_t reserved6;
   uint16_t cs;
   uint16_t reserved7;
   uint16_t ss;
   uint16_t reserved8;
   uint16_t ds;
   uint16_t reserved9;
   uint16_t fs;
   uint16_t reserved10;
   uint16_t gs;
   uint16_t reserved11;
   uint16_t ldtr;
   uint16_t iopb_offset;
   uint16_t reserved12;
} __attribute__((packed));

typedef struct tss_entry tss_entry_t;