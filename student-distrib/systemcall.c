#include "systemcall.h"

void initialize_FDT() {
	int index;

	//Initialize all fdt entries
	for(index = 0; index < 8; index ++) {
		fdt[i].fops_pointer = NULL;
		fdt[i].inode_pointer = NULL;
		fdt[i].file_position = 0;
		fdt[i].flags=0;
	}

	//Initialize stdin/stdout fops pointers and flags
	fdt[0].fops_pointer = stdin_fops; 
	fdt[0].flags = 1;

	fdt[1].fops_pointer = stdout_fops;
	fdt[1].flags = 1;
}


int32_t sys_halt (uint8_t status) {

	// asm volatile()
	// asm volatile("jmp jump_point");
	return 0;

}

int32_t sys_execute (const uint8_t* command){
	// asm volatile ("jump_point:");
	// asm volatile ("ret");
	// return 10;
	return 0;
}

int32_t sys_read (int32_t fd, void* buf, int32_t bytes) {

	return 0;
}

int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes){

	return 0;
}

int32_t sys_open (const uint8_t* filename){
	if(strncmp(filename, "stdin", 5) == 0) {
		fdt[index].fops_pointer = &stdin_fops;
	}
	else if(strncmp(filename, "stdout", 6) == 0) {
		fdt[index].fops_pointer = &stdout_fops;
	}
	else {
		int index=2;
		int filetype;
		dentry_t new_dentry;

		//Check to make sure FDT isn't full
		while(index < MAX_NUM_FDT_ENTRIES && fdt[i].flags == 1) {
			index++;
		}

		//FDT is full
		if(index == 8) {
			return ERROR_VAL;
		}

		//Figure out the filetype
		if(read_dentry_by_name(filename, &new_dentry) == -1) {
			return ERROR_VAL;
		}
		filetype = new_dentry.type;

		//If we have a regular file, then update the inode pointer
		if(filetype == REGULAR_FILE_FILETYPE) {
			fdt[index].inode_pointer = (bootMemAddr + FILESYSTEM_BLOCKSIZE*(new_dentry.inodeNum + 1));		
		}
		
		//Mark as used
		fdt[index].flags = 1;

		//Call the respective open function
		switch(filetype) {
			case RTC_DEVICE_FILETYPE:
				fdt[index].fops_pointer = &RTC_fops;
				break;

			case DIRECTORY_FILETYPE:
				fdt[index].fops_pointer = &directory_fops;
				break;

			case REGULAR_FILE_FILETYPE: 
				fdt[index].fops_pointer = &regfile_fops;
				break;

			default:
				return ERROR_VAL;
		}	

	}
	(*fdt[index].fops_pointer.open)(filename);
	return 0;
}


int32_t sys_close (int32_t fd) {
	if(fd < 2 || fd > MAX_NUM_FDT_ENTRIES) {
		return ERROR_VAL;
	}

	(*fdt[fd].fops_pointer.close)(fd);
	fdt[fd].flags = 0;
	return 0;
}

int32_t sys_getargs (uint8_t* buf, int32_t nbytes){

	return 0;
}

int32_t sys_vidmap (uint8_t** screen start){

	return 0;
}

int32_t sys_sethandler (int32_t signum, void* handler_address){
	
	return 0;
}

int32_t sys_sigreturn (void){
	
	return 0;
}

int32_t dummy_read (int32_t fd, void* buf, int32_t nbytes){
	return ERROR_VAL;
}

int32_t dummy_write (int32_t fd, const void* buf, int32_t nbytes){
	return ERROR_VAL;
}

int32_t dummy_open (const uint8_t* filename){
	return ERROR_VAL;
}

int32_t dummy_close (int32_t fd){
	return ERROR_VAL;
}

void set_kernel_stack (uint32_t kernel_stack_ptr) {
    tss.esp0 = kernel_stack_ptr;
}
