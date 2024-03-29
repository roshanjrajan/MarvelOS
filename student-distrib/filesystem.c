#include "filesystem.h"

/* 
 * strlenFile
 *
 * DESCRIPTION: find the len of the string (must be less then input maxStrLen)
 * INPUT: const int8_t* s = string to take length of
 * 		  uint32_t maxStrLen = the maximum string length
 * OUTPUT: length of string s
 * SIDE_EFFECTS:  return length of string s
 */
uint32_t
strlenFile(const int8_t* s, uint32_t maxStrLen)
{
	register uint32_t len = 0;
	// Wait while null terminator of max string length
	while(s[len] != '\0'&& len < maxStrLen)
		len++;

	return len;
}

/* 
 * strcpyFile
 *
 * DESCRIPTION: Copies from src to dest for string (copy size must be less then input maxStrLen)
 * INPUT: int8_t* dest = destination string of copy
 *			const int8_t* src = source string of copy
 *			uint32_t maxStrLen max string length
 * OUTPUT:  Number of bytes copied
 * SIDE_EFFECTS:  return number of bytes copied
 */
uint32_t
strcpyFile(int8_t* dest, const int8_t* src, uint32_t maxStrLen)
{
	uint32_t i=0;
	uint32_t bytesCopied = 0;
	// Wait while null terminator of max string length
	while(src[i] != '\0' && i < maxStrLen) {
		dest[i] = src[i];
		i++;
	}
	bytesCopied = i;

	return bytesCopied;
}

/* 
 * read_dentry_by_name
 *
 * DESCRIPTION: Parse through the File System directory and check the file name. Fill in dentry with file info
 * INPUT: const uint8_t* fname = File Name to be searched
 * 		  dentry_t* dentry = pointer to dentry to be written to 
 * OUTPUT: returns -1 if file doesn't exist, else returns 0
 * SIDE_EFFECTS: reads file name and fills in dentry parameter
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{
	int i;
	// Check if input dentry is null
	if (dentry == NULL) return ERROR_VAL;
	
	// For each file in the directory
	for(i = 0; i < fileSysBootBlock->numDirectories; i++)
	{
		//Compare if the fname = current file name of directory entry
		if(strncmp((int8_t *)fname, (int8_t *)fileSysBootBlock->fileDirectory[i].fileName, MAX_FILENAME_LENGTH) == 0)
		{
				// Fill in dentry
				*dentry = fileSysBootBlock->fileDirectory[i];
				return 0;
		}
	}
	
	return ERROR_VAL;
}

/* 
 * read_dentry_by_name
 *
 * DESCRIPTION: Parse through the File System directory and check the index. Fill in dentry with file info
 * INPUT: const uint32_t index = Index to be found
 * 		  dentry_t* dentry = pointer to dentry to be written to 
 * OUTPUT: returns 0 if valid index, -1 for invalid index
 * SIDE_EFFECTS: reads file at index and fills in dentry parameter
 */ 
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
	// Check if input dentry is null
	if (dentry == NULL) return ERROR_VAL;
	
	// Check if input index is valid
	if(index >= fileSysBootBlock->numDirectories || index < 0) // Invalid Index
		return ERROR_VAL;
	else
	{
		//Fill in dentry of the index
		*dentry = fileSysBootBlock->fileDirectory[index];
		return 0;
	}
	
}

/* 
 * read_data
 *
 * DESCRIPTION: Parse through the dentry and check the file name
 * INPUT: uint32_t inode = indoe number of file to be read
 *		  uint32_t offset = offset in the file to be read
 * 		  uint8_t* buf = buffer that is filled in with text
 * 		  uint32_t length = length of data that needs to be filled into the buffer
 * OUTPUT: returns -1 if buf is null, invalid inode, invalid offset else returns number of bytes copied
 * SIDE_EFFECTS: 
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	// Check if input buf is null or invalid inode input
	if (buf == NULL) return ERROR_VAL;
	if(inode >= fileSysBootBlock->numInodes || inode < 0)
			return ERROR_VAL;
		
	if(length < 0) {
		return ERROR_VAL;
	}

	// Current fileInode
	inode_t fileInode = *((inode_t *)(bootMemAddr + FILESYSTEM_BLOCKSIZE*(inode + 1)));
	
	//Check if we are at the end of the file
	if(offset == fileInode.length)
		return 0;
	
	// Check if offset is within file bounds
	if(offset < 0 || offset > fileInode.length)
		return ERROR_VAL;
	

	// Variables used to calculate various checks
	uint32_t dataBlockIndex = offset/FILESYSTEM_BLOCKSIZE;
	uint32_t dataBlockOffset = offset%FILESYSTEM_BLOCKSIZE;
	uint32_t bytesCopied = 0;
	uint32_t copyLength = MIN(length, fileInode.length-offset);
	
	
	// Start of current copy
	uint32_t copy_start;
	uint32_t copy_len;
	
	// the number of bytes copied is less then the length
	while(bytesCopied < copyLength)
	{
		// beginning of copy 
		copy_start = bootMemAddr + FILESYSTEM_BLOCKSIZE * (fileSysBootBlock->numInodes + 1 + 
			fileInode.dataBlocks[dataBlockIndex]) + dataBlockOffset;
			
		// amount to be copied
		copy_len = MIN((FILESYSTEM_BLOCKSIZE - dataBlockOffset), copyLength - bytesCopied);
		
		// Copy into buffer
		memcpy((void *)(buf + bytesCopied), (void *)copy_start, copy_len);
		
		// Increment block Index, bytes copied, and reset offset in block
		dataBlockIndex++;
		bytesCopied += copy_len;
		dataBlockOffset = 0;
	}
	
	// reutrn number of bytes copied
	return bytesCopied;
}




// FILE SYSTEM STUFF

/* 
 * fileSysInit
 *
 * DESCRIPTION: Initialize boot block information
 * INPUT: module representing start of the file system.
 * OUTPUT: none
 * SIDE_EFFECTS: All necessary info about the boot block is filled
 */
void fileSysInit(module_t *mod)
{
	// store beginning address of file system
	bootMemAddr = mod->mod_start;
	
	// store struct info of the boot block
	fileSysBootBlock = (boot_block_t *)(mod->mod_start);
}

/* 
 * fileOpen
 *
 * DESCRIPTION: Open the file
 * INPUT: dummy value.
 * OUTPUT: returns 0
 * SIDE_EFFECTS: none. 
 */
int32_t fileOpen(const uint8_t* filename){
	return 0;
}



/* 
 * fileWrite
 *
 * DESCRIPTION: Write to the file 
 * INPUT: dummy value.
 * OUTPUT: returns -1
 * SIDE_EFFECTS: none. 
 */
int32_t fileWrite(int32_t fd, const void* buf, int32_t nbytes){
	return ERROR_VAL;
}


/* 
 * fileClose
 *
 * DESCRIPTION: close the file 
 * INPUT: dummy value.
 * OUTPUT: returns 0
 * SIDE_EFFECTS: none. 
 */
int32_t fileClose(int32_t fd){
	return 0;
}





//DIRECTORY FUNCTIONS


/* 
 * directoryOpen
 *
 * DESCRIPTION: Open directory
 * INPUT: dummy argument.
 * OUTPUT: returns 0
 * SIDE_EFFECTS: none. 
 */
int32_t directoryOpen(const uint8_t* filename){
	return 0;
}



/*  
 * directoryWrite
 *
 * DESCRIPTION: Write to the directory
 * INPUT: dummy argument.
 * OUTPUT: returns -1
 * SIDE_EFFECTS: none. 
 */
int32_t directoryWrite(int32_t fd, const void* buf, int32_t nbytes){
	return ERROR_VAL;
}


/* 
 * directoryClose
 *
 * DESCRIPTION: Close directory
 * INPUT: dummy argument.
 * OUTPUT: returns 0
 * SIDE_EFFECTS: none. 
 */
int32_t directoryClose(int32_t fd){
	return 0;
}



/* 
 * fileReadIdx
 *
 * DESCRIPTION: Reads a file using the file name and fills in the buffer
 * INPUT: const uint8_t* fname = file name of file to be read
 * 		  void *  buf = buffer to be filled
 * 		  int32_t nbytes = length of buffer
 * OUTPUT: returns -1 if bad file name, else returns bytes copied or errors from read_data
 * SIDE_EFFECTS: Fills in buffer to be printed
 */
int32_t fileReadIdx(uint32_t index, void * buf, int32_t nbytes){
	
	dentry_t dentry;
	
	// Check that file exists and fill dentry
	if(read_dentry_by_index (index, &dentry) != 0)
		return ERROR_VAL; // BAD FILE NAME
	
	// return data read
	return read_data (dentry.inodeNum, 0, buf, nbytes);
}
