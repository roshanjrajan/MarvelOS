#ifndef RTC_H
#define RTC_H

#include "lib.h"
#include "i8259.h"

#define INIT_FREQ 2
#define MIN_FREQ 2
#define MAX_FREQ 1024
#define BIT31 0x80000000
#define BASE_FREQ 32768
#define RTC_IRQ 8
#define RTC_PORT 0x70
#define RTC_DATA 0x71
#define BIT6_MASK 0x40
#define LOW4 0x0f
#define HI4 0xf0
#define RTC_A 0x0a
#define RTC_B 0x0b
#define RTC_C 0x0c
#define BIT8 0x80


extern int32_t RTCOpen(const uint8_t* filename);
extern int32_t RTCRead(int32_t fd, void* buf, int32_t nbytes);
extern int32_t RTCWrite(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t RTCClose(int32_t fd);
extern void RTC_init();
extern void RTChandler();
extern void startRTCTest();
extern void stopRTCTest();

#endif
