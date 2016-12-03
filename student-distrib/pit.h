#ifndef PIT_H
#define PIT_H

#include "lib.h"
#include "i8259.h"
#include "systemcall.h"

#define PIT_IRQ 0
#define NUM_THREADS 3
#define PIT_RATE 35

uint32_t ESPstore[NUM_THREADS];
uint32_t EBPstore[NUM_THREADS];
uint32_t PIDstore[NUM_THREADS];
uint32_t shellStarted[NUM_THREADS];	// 1 = started, 0 = not started



void PIThandler();
void PITinit();




#endif
