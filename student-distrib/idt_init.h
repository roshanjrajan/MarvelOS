#ifndef IDT_INIT_H
#define IDT_INIT_H
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "RTC.h"
#include "terminal.h"
#include "systemcall.h"
#include "pit.h"

// GENERAL CONSTANTS
#define SLAVE_IRQ 2
#define SCALL_LOC 0x80
#define RESERVED_1 1
#define RESERVED_15 15
#define RESERVED_20 20
#define USR_DPL 3

// IDT FUNCTIONS
extern void init_idt();

#endif

