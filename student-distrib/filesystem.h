
#ifndef FILESYSTEM_H
#define FILESYSTEM_H
 
#include "types.h"
#include "multiboot.h"
#include "lib.h"
#include "terminal.h"

#define MAX_FILENAME_LENGTH 32
#define FILESYSTEM_BLOCKSIZE 4096
#define NUM_DATABLOCKS_PER_INODE 1023
#define NUM_RESERVED_DENTRY 6
#define NUM_RESERVED_BOOT_BLOCK 13
#define MAX_NUM_DIRECTORY_ENTRIES 63
#define FILESYSTEM_DIRECTORIES 17
#define BUFFER_TEST_SIZE 1500
#define FNAME_L 12

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


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

boot_block_t* fileSysBootBlock;

extern void fileSysInit(module_t* mod);
extern int32_t fileOpen();
extern int32_t fileRead(const uint8_t* fname, void * buf, int32_t nbytes);
extern int32_t fileReadIdx(uint32_t index, void * buf, int32_t nbytes);
extern int32_t fileWrite();
extern int32_t fileClose();

extern int32_t directoryOpen();
extern int32_t directoryRead(int32_t fd, void * buf, int32_t nbytes);
extern int32_t directoryWrite();
extern int32_t directoryClose();

extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

extern void testDirRead();
extern void testFileRead(uint8_t * fname);
extern void testFileIndex(uint32_t index);

#endif
