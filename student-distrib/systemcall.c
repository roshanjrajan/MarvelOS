#include "systemcall.h"

void initialize_tss(uint16_t kernel_stack_segment, uint32_t kernel_stack_pointer) {
    SET_TSS_PARAMS(tss_desc_ptr, tss, tss_size);
    tss_desc_ptr.type = 9;////////
    tss_desc_ptr.sys = 0;/////
    tss_desc_ptr.dpl = 0;/////
    tss_desc_ptr.present = 1;
    tss_desc_ptr.opsize =0;
    tss_desc_ptr.granularity = 0;/////////
    tss.esp0 = kernel_stack_pointer;
    tss.ss0 = kernel_stack_segment;
    tss.cs = TSS_CS;
    tss.ss = tss.ds = tss.es = tss.fs = tss.gs = TSS_DS;
    flush_tss();
}

void set_kernel_stack (uint32_t kernel_stack_ptr) {
    tss.esp0 = kernel_stack_ptr;
}