#ifndef IDT_INIT_H
#define IDT_INIT_H
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "RTC.h"

// GENERAL CONSTANTS
#define KB_IRQ 1
#define SLAVE_IRQ 2
#define SCALL_LOC 0x80
#define SIZE_SCODES 256
#define RESERVED_1 1
#define RESERVED_15 15
#define RESERVED_20 20
#define USR_DPL 3

#define KB_DATA_PORT 0x60

// IDT FUNCTIONS
extern void init_idt();
extern void RTC_init();
extern void KBhandler();

#endif

