#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#include "x86_desc.h"
#include "filesystem.h"
#include "terminal.h"
#include "RTC.h"
#include "types.h"
#include "paging.h"
#include "lib.h"

// General Constants
#define RTC_DEVICE_FILETYPE 0
#define DIRECTORY_FILETYPE 1
#define REGULAR_FILE_FILETYPE 2
#define STDIN_INDEX_IN_FDT 0
#define STDOUT_INDEX_IN_FDT 1
#define EXECUTABLE_CHECK_BUFFER_SIZE 4
#define EIGHT_MB 0x00800000
#define FOUR_MB 0x00400000
#define EIGHT_KB 0x00002000
#define FIVE_HUNDRED_TWELVE_MB EIGHT_MB * 64
#define ONE_HUNDRED_TWENTY_EIGHT_MB FIVE_HUNDRED_TWELVE_MB/ 4
#define ONE_HUNDRED_THIRTY_TWO_MB ONE_HUNDRED_TWENTY_EIGHT_MB + FOUR_MB
#define PROGRAM_INIT_VIRTUAL_ADDR 0x08048000
#define PROCESS_PAGING_INDEX 32 // (128 MB / 4 MB) = 32 (indexing starts at 0)
#define USER_VIDMAP_ADDR FIVE_HUNDRED_TWELVE_MB
#define PROCESS_BASE_4KB_ALIGNED_ADDRESS EIGHT_MB
#define FNAME_SIZE 32
#define ARG_SIZE_WITHOUT_NULL_TERMINATOR 127
#define ARG_NULL_TERMINATOR_INDEX 127
#define FIRST_BYTE 0
#define SECOND_BYTE 1
#define THIRD_BYTE 2
#define FOURTH_BYTE 3
#define NOT_IN_OUT 2
#define LONG_BYTES 4

// FLAGS FOR FDT
#define UNUSED_FLAG 0
#define STDIN_FLAG 1
#define STDOUT_FLAG 2
#define RTC_FLAG 3
#define DIRECTORY_FLAG 4
#define REGFILE_FLAG 5

// BYTES TO CHECK EXECUTABLE
#define EXECUTABLE_FIRST_BYTE 0x7F
#define EXECUTABLE_SECOND_BYTE 0x45
#define EXECUTABLE_THIRD_BYTE 0x4C
#define EXECUTABLE_FOURTH_BYTE 0x46
#define EXECUTABLE_INSTRUCTION_START_BYTE 24

// Functions used
extern void switch_to_user_mode(uint32_t starting_addr);
extern void initialize_FDT(int32_t pid);
extern void initialize_PCB_pointers();
extern uint32_t get_eip();
int32_t fileRead(int32_t fd, void * buf, int32_t nbytes);
int32_t directoryOpen(const uint8_t* filename);
void initialize_fops();

//System call dispatcher function
extern int32_t syscall();

//Handlers for various system calls
extern int32_t sys_halt (uint8_t status);
extern int32_t sys_execute (const uint8_t* command);
extern int32_t sys_read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t sys_open (const uint8_t* filename);
extern int32_t sys_close (int32_t fd);
extern int32_t sys_getargs (uint8_t* buf, int32_t nbytes);
extern int32_t sys_vidmap (uint8_t** screen_start);
extern int32_t sys_sethandler (int32_t signum, void* handler_address);
extern int32_t sys_sigreturn (void);

//dummy functions
int32_t dummy_read (int32_t fd, void* buf, int32_t nbytes);
int32_t dummy_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t dummy_open (const uint8_t* filename);
int32_t dummy_close (int32_t fd);

// Global variables used
//PCB_t* PCB_ptrs[MAX_PROCESSES];
file_descriptor_entry_t * fdt;
//int cur_pid;
uint32_t eax_val;

//Here are our fops tables
fops_table_t stdin_fops;
fops_table_t stdout_fops;
fops_table_t regfile_fops;
fops_table_t directory_fops;
fops_table_t RTC_fops;

#endif 
