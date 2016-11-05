#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#include "x86_desc.h"
#include "types.h"

extern void switch_to_user_mode();
extern fastcall void syscall(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3);

#endif 