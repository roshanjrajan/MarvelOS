#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#include "x86_desc.h"
#include "filesystem.h"
#include "terminal.h"
#include "RTC.h"
#include "types.h"
#include "paging.h"

#define RTC_DEVICE_FILETYPE 0
#define DIRECTORY_FILETYPE 1
#define REGULAR_FILE_FILETYPE 2
#define STDIN_INDEX_IN_FDT 0
#define STDOUT_INDEX_IN_FDT 1
#define MAX_PROCESSES 2
#define MAX_NUM_FDT_ENTRIES 8
#define EIGHT_MB 0x00800000
#define FOUR_MB 0x00400000
#define EIGHT_KB 0x00002000
#define PROCESS_PAGING_INDEX 32 // (128 MB / 4 MB) = 32 (indexing starts at 0)
#define PROCESS_BASE_4KB_ALIGNED_ADDRESS EIGHT_KB

extern void switch_to_user_mode();
extern void initialize_FDT(int32_t pid);
extern void initialize_PCB_pointers();

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
extern int32_t sys_vidmap (uint8_t** screen start);
extern int32_t sys_sethandler (int32_t signum, void* handler address);
extern int32_t sys_sigreturn (void);

//dummy functions
int32_t dummy_read (int32_t fd, void* buf, int32_t nbytes);
int32_t dummy_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t dummy_open (const uint8_t* filename);
int32_t dummy_close (int32_t fd);

typedef	struct __attribute__((packed)) fops_table
{
	int32_t (*open)(const uint8_t*);
	int32_t (*close)(int32_t);
	int32_t (*read)(int32_t, void*, int32_t);
	int32_t (*write)(int32_t, const void*, int32_t);
} fops_table_t;

typedef struct __attribute__((packed)) file_descriptor_entry
{
	fops_table_t * fops_pointer;
	uint32_t inode_pointer;
	int32_t file_position;
	uint32_t flags;
} file_descriptor_entry_t;

typedef struct __attribute__((packed)) PCB {
	int32_t pid;
	int32_t parent_pid;
	pde_desc_t pde;
	uint32_t esp;
	uint32_t ebp;
	file_descriptor_entry_t process_fdt[MAX_NUM_FDT_ENTRIES];
} PCB_t; 

PCB_t* PCB_ptrs[MAX_PROCESSES];
file_descriptor_entry_t * fdt;

//Populate our fops tables
fops_table_t stdin_fops = {
	.open = terminalOpen
	.close = terminalClose
	.read = terminalRead
	.write = dummy_write
};

fops_table_t stdout_fops = {
	.open = terminalOpen
	.close = terminalClose
	.read = dummy_read
	.write = terminalWrite
};

fops_table_t regfile_fops = {
	.open = fileOpen
	.close = fileClose
	.read = fileRead
	.write = fileWrite
};

fops_table_t directory_fops = {
	.open =  directoryOpen
	.close = directoryClose
	.read =  directoryRead
	.write = directoryWrite
};

fops_table_t RTC_fops = {
	.open =  RTCOpen
	.close = RTCClose
	.read =  RTCRead
	.write = RTCWrite
};

#endif 


