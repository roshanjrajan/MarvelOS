#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#include "x86_desc.h"
#include "filesystem.h"
#include "terminal.h"
#include "RTC.h"
#include "types.h"

#define RTC_DEVICE_FILETYPE 0
#define DIRECTORY_FILETYPE 1
#define REGULAR_FILE_FILETYPE 2

extern void switch_to_user_mode();
extern int32_t syscall();

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

extern void initialize_FDT();


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
	uint32_t file_position;
	uint32_t flags;
} file_descriptor_entry_t;

file_descriptor_entry_t fdt[MAX_NUM_FDT_ENTRIES];

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


