#include "filesystem.h"

// Starting address of File System Image
uint32_t bootMemAddr;


/*
* uint32_t strlen(const int8_t* s);
*   Inputs: const int8_t* s = string to take length of
*   Return Value: length of string s
*	Function: return length of string s
*/

uint32_t
strlenFile(const int8_t* s, uint32_t maxStrLen)
{
	register uint32_t len = 0;
	while(s[len] != '\0'&& len < maxStrLen)
		len++;

	return len;
}

/*
* int8_t* strcpy(int8_t* dest, const int8_t* src)
*   Inputs: int8_t* dest = destination string of copy
*			const int8_t* src = source string of copy
*			uint32_t maxStrLen max string length
*   Return Value: pointer to dest
*	Function: copy the source string into the destination string (max length of maxStrLen)
*/

int8_t*
strcpyFile(int8_t* dest, const int8_t* src, uint32_t maxStrLen)
{
	int32_t i=0;
	while(src[i] != '\0' && i < maxStrLen) {
		dest[i] = src[i];
		i++;
	}

	dest[i] = '\0';
	return dest;
}




// READING DATA/DIRECTORY ENTRIES HELPER FUNCTIONS
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
	if (dentry == NULL) return -1;
	for(i = 0; i < fileSysBootBlock->numDirectories; i++)
	{
		if(strncmp((int8_t *)fname, (int8_t *)fileSysBootBlock->fileDirectory[i].fileName, MAX_FILENAME_LENGTH) == 0)
		{
				*dentry = fileSysBootBlock->fileDirectory[i];
				return 0;
		}
	}
	
	return -1;
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
	if (dentry == NULL) return -1;
	if(index >= fileSysBootBlock->numDirectories || index < 0) // Invalid Index
		return -1;
	else
	{
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
	// Check cases
	if (buf == NULL) return -1;
	if(inode >= fileSysBootBlock->numInodes || inode < 0)
			return -1;
		
	inode_t fileInode = *((inode_t *)(bootMemAddr + FILESYSTEM_BLOCKSIZE*(inode + 1)));
	
	if(offset < 0 || offset >= fileInode.length)
		return -1;
	
	// Copy inode data
	
	uint32_t dataBlockIndex = offset/FILESYSTEM_BLOCKSIZE;
	uint32_t dataBlockOffset = offset%FILESYSTEM_BLOCKSIZE;
	uint32_t bytesCopied = 0;
	uint32_t copyLength = MIN(length, fileInode.length);
	
	
	uint32_t copy_start;
	uint32_t copy_len;
	
	while(bytesCopied < copyLength)
	{
		copy_start = bootMemAddr + FILESYSTEM_BLOCKSIZE * (fileSysBootBlock->numInodes + 1 + 
			fileInode.dataBlocks[dataBlockIndex]) + dataBlockOffset;
		copy_len = MIN((FILESYSTEM_BLOCKSIZE - dataBlockOffset), copyLength - bytesCopied);
		
		memcpy((void *)(buf + bytesCopied), (void *)copy_start, copy_len);
		
		dataBlockIndex++;
		bytesCopied += copy_len;
		dataBlockOffset = 0;
	}
	
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
	bootMemAddr = mod->mod_start;
	fileSysBootBlock = (boot_block_t *)(mod->mod_start);
}

/* 
 * fileOpen
 *
 * DESCRIPTION: Open nothing 
 * INPUT: none.
 * OUTPUT: returns 0
 * SIDE_EFFECTS: none. 
 */
int32_t fileOpen(){
	return 0;
}

/* 
 * fileRead
 *
 * DESCRIPTION: Reads a file using the file name and fills in the buffer
 * INPUT: const uint8_t* fname = file name of file to be read
 * 		  uint8_t * buf = buffer to be filled
 * 		  uint32_t length = length of file
 * OUTPUT: returns -1 if bad file name, else returns bytes copied or errors from read_data
 * SIDE_EFFECTS: Fills in buffer to be printed
 */
int32_t fileRead(const uint8_t* fname, void * buf, int32_t nbytes){
	// DO SOMETHING With NAME
	dentry_t dentry;
	
	if(read_dentry_by_name (fname, &dentry) != 0)
		return -1; // BAD FILE NAME
	
	return read_data (dentry.inodeNum, 0, buf, nbytes);
}

/* 
 * fileWrite
 *
 * DESCRIPTION: Write to the file 
 * INPUT: none.
 * OUTPUT: returns -1
 * SIDE_EFFECTS: none. 
 */
int32_t fileWrite(){
	return -1;
}


/* 
 * fileClose
 *
 * DESCRIPTION: Write to the file 
 * INPUT: none.
 * OUTPUT: returns -1
 * SIDE_EFFECTS: none. 
 */
int32_t fileClose(){
	return 0;
}





//DIRECTORY FUNCTIONS


/* 
 * directoryOpen
 *
 * DESCRIPTION: Open nothing 
 * INPUT: none.
 * OUTPUT: returns 0
 * SIDE_EFFECTS: none. 
 */
int32_t directoryOpen(){
	return 0;
}

/* 
 * directoryRead
 *
 * DESCRIPTION: Write to the file 
 * INPUT: uint32_t index = file index of file to be read
 * 		  uint8_t * buf = buffer to be filled
 * OUTPUT: returns -1 if bad file index else fills in the buffer to be printed
 * SIDE_EFFECTS: Prints out the directory entry associated with file
 */
int32_t directoryRead(int32_t fd, void * buf, int32_t nbytes){
	
	int index;
	for(index = 0; index < fileSysBootBlock->numDirectories; index++)
	{
		dentry_t dentry;
		if(read_dentry_by_index (index, &dentry) != 0)
			return -1; // Done with all of them
		
		strcpyFile((int8_t *)buf, (int8_t *)(&dentry.fileName), MAX_FILENAME_LENGTH);
		
		uint32_t bufLength = strlenFile((int8_t *)buf, MAX_FILENAME_LENGTH);
		terminalWrite(0, (void *)buf, (int32_t)bufLength);
		terminalWrite(0, (void *)"\n", 1);
	}
	
	return 0;
}

/*  
 * directoryWrite
 *
 * DESCRIPTION: Write to the file 
 * INPUT: none.
 * OUTPUT: returns -1
 * SIDE_EFFECTS: none. 
 */
int32_t directoryWrite(){
	return -1;
}


/* 
 * directoryClose
 *
 * DESCRIPTION: Write to the file 
 * INPUT: none.
 * OUTPUT: returns -1
 * SIDE_EFFECTS: none. 
 */
int32_t directoryClose(){
	return 0;
}



// TEST FUNCTIONS FOR CODE
void testDirRead()
{
	uint32_t length = MAX_FILENAME_LENGTH;
	uint8_t buf[length];
	
	directoryRead(0, (void *) buf, (int32_t) length);
}

void testFileRead() {
	uint8_t* fname = (uint8_t *)"frame1.txt\0";
	//uint8_t* fname = "verylargetextwithverylongname.txt";
	uint8_t buf[FILESYSTEM_BLOCKSIZE];
	
	int32_t bufLength = fileRead(fname, buf, FILESYSTEM_BLOCKSIZE);
	
	terminalWrite(0, (void *)buf, bufLength);
}







