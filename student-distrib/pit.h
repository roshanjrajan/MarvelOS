#ifndef PIT_H
#define PIT_H

#include "lib.h"
#include "i8259.h"
#include "systemcall.h"
#include "paging.h"

#define PIT_IRQ 0
#define PIT_RATE 35

uint32_t ESPstore[NUM_THREADS];
uint32_t EBPstore[NUM_THREADS];
uint32_t PIDstore[NUM_THREADS];


void PITwrapper();
void PIThandler();
void PITinit();




#endif
