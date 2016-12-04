#ifndef PIT_H
#define PIT_H

#include "lib.h"
#include "i8259.h"
#include "systemcall.h"
#include "paging.h"

#define PIT_IRQ 0
#define PIT_RATE 35
#define PIT_MODE_REG 0x43
#define PIT_DATA_REG 0x40
#define LOW_8 0xff
#define BITS_8 8
#define PIT_BASE_RATE 1193182
#define PIT_MODE_SET 0x34

uint32_t ESPstore[NUM_THREADS];
uint32_t EBPstore[NUM_THREADS];
uint32_t PIDstore[NUM_THREADS];


void PITwrapper();
void PIThandler();
void PITinit();




#endif
