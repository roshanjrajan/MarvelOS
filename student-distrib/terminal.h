#ifndef TERMINAL_H
#define TERMINAL_H

#include "lib.h"
#include "i8259.h"
#include "paging.h"

// General Constants used
#define KB_IRQ 1
#define KB_DATA_PORT 0x60
#define SIZE_SCODES 64
#define ALL_SCODES 256
#define BUF_SIZE 128
#define READ_AGAIN 0xe0
#define CTM 0x1d
#define CTB 0x9d
#define CLEAR 0x26
#define ALTM 0x38
#define ALTB 0xb8
#define TERMINAL_0_SCANCODE 0x3b
#define TERMINAL_1_SCANCODE 0x3c
#define TERMINAL_2_SCANCODE 0x3d
#define CAPS 0x3a
#define SLM 0x2a
#define SLB 0xaa
#define SRM 0x36
#define SRB 0xb6
#define BIT8 0x80
#define ENTER 0x1c
#define BKSP 0x0e
#define MOD2 2
#define CAPS_OFF 64
#define SHIFT_OFF 128

// Different Terminal Numbers
#define TERMINAL_0 0
#define TERMINAL_1 1
#define TERMINAL_2 2

// function used for switching between terminals
extern int32_t switchTerminal(uint8_t previousTerminal);

// Functions used to define the keyboard and terminal
extern void KBhandler();
extern int32_t terminalOpen(const uint8_t* filename);
extern int32_t terminalClose(int32_t fd);
extern int32_t terminalRead(int32_t fd, void* buf, int32_t nbytes);
extern int32_t terminalWrite(int32_t fd, const void* buf, int32_t nbytes);

#endif
