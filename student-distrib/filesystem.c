#include "filesystem.h"

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
	// Wait while null terminator of max string length
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
	// Wait while null terminator of max string length
	while(src[i] != '\0' && i < maxStrLen) {
		dest[i] = src[i];
		i++;
	}

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
	// Check if input dentry is null
	if (dentry == NULL) return -1;
	
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
	// Check if input dentry is null
	if (dentry == NULL) return -1;
	
	// Check if input index is valid
	if(index >= fileSysBootBlock->numDirectories || index < 0) // Invalid Index
		return -1;
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
	if (buf == NULL) return -1;
	if(inode >= fileSysBootBlock->numInodes || inode < 0)
			return -1;
		
	// Current fileInode
	inode_t fileInode = *((inode_t *)(bootMemAddr + FILESYSTEM_BLOCKSIZE*(inode + 1)));
	
	// Check if offset is within file bounds
	if(offset < 0 || offset >= fileInode.length)
		return -1;
	

	// Variables used to calculate various checks
	uint32_t dataBlockIndex = offset/FILESYSTEM_BLOCKSIZE;
	uint32_t dataBlockOffset = offset%FILESYSTEM_BLOCKSIZE;
	uint32_t bytesCopied = 0;
	uint32_t copyLength = MIN(length, fileInode.length);
	
	
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
 * DESCRIPTION: Open nothing 
 * INPUT: dummy value.
 * OUTPUT: returns 0
 * SIDE_EFFECTS: none. 
 */
int32_t fileOpen(const uint8_t* filename){
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
		return -1; // BAD FILE NAME
	
	// return data read
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
int32_t fileWrite(int32_t fd, const void* buf, int32_t nbytes){
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
int32_t fileClose(int32_t fd){
	return 0;
}





//DIRECTORY FUNCTIONS


/* 
 * directoryOpen
 *
 * DESCRIPTION: Open nothing 
 * INPUT: dummy argument.
 * OUTPUT: returns 0
 * SIDE_EFFECTS: none. 
 */
int32_t directoryOpen(const uint8_t* filename){
	return 0;
}

/* 
 * directoryRead
 *
 * DESCRIPTION: Print all the files in the directory to terminal
 * INPUT: int32_t fd, void * buf, int32_t nbytes
 * 		  uint8_t * buf = buffer to be filled
 * OUTPUT: returns -1 if bad file index else fills in the buffer to be printed
 * SIDE_EFFECTS: Prints out the directory entry associated with directory files
 */
int32_t directoryRead(int32_t fd, void * buf, int32_t nbytes){
	
	// For each file in directory
	int index;
	dentry_t dentry;
	for(index = 0; index < fileSysBootBlock->numDirectories; index++)
	{
		//printf("1Index: %d", index);
		// Fill in dentry of index
		
		if(read_dentry_by_index (index, &dentry) != 0)
			return -1; // Done with all of them
		
		
		// copy filename to buffer
		strcpyFile((int8_t *)buf, (int8_t *)(dentry.fileName), MAX_FILENAME_LENGTH);
		
		//print file name to terminal
		//printf("2Index: %d\n", index);
		uint32_t bufLength = strlenFile((int8_t *)dentry.fileName, MAX_FILENAME_LENGTH);
		terminalWrite(0, (void *)buf, (int32_t)bufLength);
		terminalWrite(0, (void *)"\n", 1);
	}
	//printf("done");
	
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
int32_t directoryWrite(int32_t fd, const void* buf, int32_t nbytes){
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
int32_t directoryClose(int32_t fd){
	return 0;
}
















/* 
 * testDirRead
 *
 * DESCRIPTION: Read all the directory files
 * INPUT: none.
 * OUTPUT: Print directory files to the terminal
 * SIDE_EFFECTS: none. 

void testDirRead()
{
	//Read all files in directory
	uint8_t buf[MAX_FILENAME_LENGTH];
	directoryRead(0, (void *) buf, (int32_t) MAX_FILENAME_LENGTH);
	//printf("dirRead done");
}

 */



/* 
 * testFileRead
 *
 * DESCRIPTION: If valid file name, print contents of file to the terminal
 * INPUT: none.
 * OUTPUT: Print directory files contents to the terminal
 * SIDE_EFFECTS: none. 

void testFileRead(uint8_t * fname)
{
	// fill in buffer about file
	//uint8_t* fname = (uint8_t *)"frame0.txt";
	int32_t nbytes = BUFFER_TEST_SIZE;
	uint8_t buf[nbytes];
	int32_t bufLength = fileRead(fname, buf, nbytes);
	
	// Print file stuff
	terminalWrite(0, (void *)buf, bufLength);
	terminalWrite(0, (void *)"\nfile_name: ", FNAME_L);
	terminalWrite(0, (void *)fname, strlenFile((int8_t *)fname, MAX_FILENAME_LENGTH));
	terminalWrite(0, (void *)"\n", 1);
}
 */

/* 
 * testDirRead
 *
 * DESCRIPTION: If valid file name, print contents of file to the terminal
 * INPUT: none.
 * OUTPUT: Print directory files to the terminal
 * SIDE_EFFECTS: none. 

void testFileIndex(uint32_t index)
{
	// fill in buffer about file
	//uint8_t* fname = (uint8_t *)"frame0.txt";
	int32_t nbytes = BUFFER_TEST_SIZE;
	uint8_t buf[nbytes];
	int32_t bufLength = fileReadIdx(index, buf, nbytes);
	
	// Print file stuff
	terminalWrite(0, (void *)buf, bufLength);
	terminalWrite(0, (void *)"\n", 1);
}

 */





