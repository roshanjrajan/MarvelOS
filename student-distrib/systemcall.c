#include "systemcall.h"


void set_kernel_stack (uint32_t kernel_stack_ptr) {
    tss.esp0 = kernel_stack_ptr;
}
