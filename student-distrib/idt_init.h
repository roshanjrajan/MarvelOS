#ifndef IDT_INIT_H
#define IDT_INIT_H
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

// GENERAL CONSTANTS
#define KB_IRQ 1
#define RTC_IRQ 8
#define SCALL_LOC 0x80
#define SIZE_SCODES 256
#define RESERVED_1 1
#define RESERVED_15 15
#define RESERVED_20 20
#define USR_DPL 3
#define RTC_PORT 0x70
#define RTC_DATA 0x71
#define BIT6_MASK 0x40
#define LOW4 0x0f
#define HI4 0xf0
#define RTC_A 0x0a
#define RTC_B 0x0b
#define RTC_C 0x0c
#define BIT8 0x80
#define KB_DATA_PORT 0x60

// IDT FUNCTIONS
extern void init_idt();
extern void RTC_init();
extern void KBhandler();

#endif

