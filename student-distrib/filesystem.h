
#ifndef FILESYSTEM
#define FILESYSTEM
 
#include "types.h"
#include "multiboot.h"
#include "lib.h"


typedef struct __attribute__((packed)) inode
{
	uint32_t length;
	uint32_t dataBlocks[1023];
} inode_t;

typedef struct __attribute__((packed)) dentry
{
	uint8_t fileName[32];
	uint32_t fileType;
	uint32_t inodeNum;
	uint32_t reserved24[6];
	
} dentry_t;

typedef struct __attribute__((packed)) boot_block
{
	uint32_t numDirectories;
	uint32_t numInodes;
	uint32_t numDataBlocks;
	uint32_t reserved52[13];
	dentry_t fileDirectory[63];
} boot_block_t; 

boot_block_t* fileSysBootBlock;

extern void fileSysInit(module_t* mod);
extern int32_t fileOpen();
extern int32_t fileRead(const uint8_t* fname, uint8_t * buf, uint32_t length);
extern int32_t fileWrite();
extern int32_t fileClose();

extern int32_t directoryOpen();
extern int32_t directoryRead(uint32_t index, uint8_t * buf);
extern int32_t directoryWrite();
extern int32_t directoryClose();

extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

extern void testDirRead();
extern void testFileRead();

#endif
