include "x86_desc.h"

#define TSS_CS KERNEL_CS + 0x03	//Setting RPL to 3
#define TSS_DS KERNEL_DS + 0x03	

extern void switch_to_user_mode();
extern void flush_tss();
