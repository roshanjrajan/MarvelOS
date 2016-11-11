#ifndef FILESYSTEM_H
#define FILESYSTEM_H
 
#include "types.h"
#include "multiboot.h"
#include "lib.h"
#include "terminal.h"

// General Constants
#define MAX_FILENAME_LENGTH 32
#define FILESYSTEM_BLOCKSIZE 4096
#define NUM_DATABLOCKS_PER_INODE 1023
#define NUM_RESERVED_DENTRY 6
#define NUM_RESERVED_BOOT_BLOCK 13
#define MAX_NUM_DIRECTORY_ENTRIES 63
#define FILESYSTEM_DIRECTORIES 17
#define BUFFER_TEST_SIZE 1500
#define FNAME_L 12
#define ERROR_VAL -1

// Simple preprocessor functions
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// Structs used to seperate info in file system
typedef struct __attribute__((packed)) inode
{
	uint32_t length;
	uint32_t dataBlocks[NUM_DATABLOCKS_PER_INODE];
} inode_t;

typedef struct __attribute__((packed)) dentry
{
	uint8_t fileName[MAX_FILENAME_LENGTH];
	uint32_t fileType;
	uint32_t inodeNum;
	uint32_t reserved24[NUM_RESERVED_DENTRY];
} dentry_t;

typedef struct __attribute__((packed)) boot_block
{
	uint32_t numDirectories;
	uint32_t numInodes;
	uint32_t numDataBlocks;
	uint32_t reserved52[NUM_RESERVED_BOOT_BLOCK];
	dentry_t fileDirectory[MAX_NUM_DIRECTORY_ENTRIES];
} boot_block_t; 

// Structure that holds all the file system info
boot_block_t* fileSysBootBlock;

// Starting address of File System Image
uint32_t bootMemAddr;

// Functions used to define the file system and file system functionality
extern void fileSysInit(module_t* mod);
extern int32_t fileOpen(const uint8_t* filename);

extern int32_t fileReadIdx(uint32_t index, void * buf, int32_t nbytes);

extern int32_t fileWrite(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t fileClose(int32_t fd);

// Functions to do the directory stuff
extern int32_t directoryOpen(const uint8_t* filename);
extern int32_t directoryRead(int32_t fd, void * buf, int32_t nbytes);
extern int32_t directoryWrite(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t directoryClose(int32_t fd);

//functions used to read from the directory entries
extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

// test functions 
extern void testDirRead();
extern void testFileRead(uint8_t * fname);
extern void testFileIndex(uint32_t index);

extern int cur_pid;

#include "systemcall.h"

#endif
