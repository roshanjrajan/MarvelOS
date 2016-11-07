#include "systemcall.h"

void initialize_PCB_pointers() {
	int index;
	for(index = 0; index < MAX_PROCESSES; index ++) {
		PCB_ptrs[index] = NULL;
	}
}

void initialize_FDT(int32_t pid) {
	int index;
	fdt = PCB_ptrs[pid]->process_fdt;

	//Initialize all fdt entries
	for(index = 0; index < MAX_NUM_FDT_ENTRIES; index ++) {
		fdt[index].fops_pointer = NULL;
		fdt[index].inode_pointer = NULL;
		fdt[index].file_position = -1;
		fdt[index].flags=0;
	}

	//Initialize stdin/stdout fops pointers, flags, and file positions
	fdt[STDIN_INDEX_IN_FDT].fops_pointer = stdin_fops; 
	fdt[STDIN_INDEX_IN_FDT].flags = 1;
	fdt[STDIN_INDEX_IN_FDT].file_position = 0;

	fdt[STDOUT_INDEX_IN_FDT].fops_pointer = stdout_fops;
	fdt[STDOUT_INDEX_IN_FDT].flags = 1;
	fdt[STDOUT_INDEX_IN_FDT].file_position = 0;
}


int32_t sys_halt (uint8_t status) {

	// asm volatile()
	// asm volatile("jmp jump_point");
	return 0;

}

int32_t sys_execute (const uint8_t* command){
	int pid;

	//Make sure arg is valid
	if(command == NULL) {
		return ERROR_VAL;
	}

	//Make sure our file is valid
	dentry_t new_dentry;
	if(read_dentry_by_name(command, &new_dentry) == -1) {
		return ERROR_VAL;
	}

	//Find the process id 
	while(pid < MAX_PROCESSES && PCB_ptrs[pid] != NULL) {
		pid++;
	}
	
	//Check if we are already running the maximum number of processes
	if(pid == MAX_PROCESSES){
		return ERROR_VAL;
	}

	//Start populating PCB data
	PCB_ptrs[pid] = EIGHT_MB - ((pid + 1) * EIGHT_KB);	//Making space at top of process's kernel stack
	PCB_ptrs[pid] -> pid = pid;
	initialize_FDT(pid); //This will populate the corresponding process_fdt field of PCB_ptrs[pid]

	//Initialize Paging for the process image (corresponds to virtual address 128 MB)
	page_directory[PROCESS_PAGING_INDEX].page_table_address =  (PROCESS_BASE_4KB_ALIGNED_ADDRESS + pid * FOUR_MB)>> PDE_PTE_ADDRESS_SHIFT;
	page_directory[PROCESS_PAGING_INDEX].open_bits = 0;
	page_directory[PROCESS_PAGING_INDEX].reserved_1 = 0;
	page_directory[PROCESS_PAGING_INDEX].page_size	= 1;
	page_directory[PROCESS_PAGING_INDEX].reserved_2 = 0;
	page_directory[PROCESS_PAGING_INDEX].accessed = 0;
	page_directory[PROCESS_PAGING_INDEX].cache_disable = 0;
	page_directory[PROCESS_PAGING_INDEX].write_through = 0;
	page_directory[PROCESS_PAGING_INDEX].user_supervisor = 0;
	page_directory[PROCESS_PAGING_INDEX].read_write_permissions = 1; 
	page_directory[PROCESS_PAGING_INDEX].present = 1;

	PCB_ptrs[pid] -> pde = page_directory[PROCESS_PAGING_INDEX]; //***********Is this what we're looking for? **********************

	//**************We need to figure out how to implement the following 3 things**********************
	PCB_ptrs[pid] -> parent_pid = 0;
	PCB_ptrs[pid] -> esp = 0; //Inline assembly for this and following?
	PCB_ptrs[pid] -> ebp = 0;

	//****** Refer to Piazza diagram to finish rest of execute **********************

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
	int index;
	if(strncmp(filename, "stdin", 5) == 0) {
		index =0; 
		fdt[index].fops_pointer = &stdin_fops;
	}
	else if(strncmp(filename, "stdout", 6) == 0) {
		index = 1;
		fdt[index].fops_pointer = &stdout_fops;
	}
	else {
		index = 2;
		int filetype;
		dentry_t new_dentry;

		//Check to make sure FDT isn't full
		while(index < MAX_NUM_FDT_ENTRIES && fdt[index].flags == 1) {
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
			fdt[index].inode_pointer = (bootMemAddr + FILESYSTEM_BLOCKSIZE*(new_dentry.inodeNum + 1));///*************using inode?
		}
		else {
			fdt[index].inode_pointer = NULL;
		}
		
		//update the file position
		fdt[index].file_position =0; ///***********************

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
	return index;
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
