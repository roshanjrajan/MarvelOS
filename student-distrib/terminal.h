#ifndef TERMINAL_H
#define TERMINAL_H

#include "lib.h"
#include "i8259.h"

#define KB_IRQ 1
#define KB_DATA_PORT 0x60
#define SIZE_SCODES 256
#define BUF_SIZE 128



extern void KBhandler();
extern int32_t terminalOpen();
extern int32_t terminalClose();
extern int32_t terminalRead(char *buf);
extern int32_t terminalWrite(char *buf, int32_t bytes);

#endif
