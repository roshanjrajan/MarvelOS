#include "systemcall.h"

// NEED TO DOUBLE CHECK THIS 
PCB_t* PCB_ptrs[MAX_PROCESSES];
file_descriptor_entry_t * fdt;
int cur_pid;

/* 
 * fileRead
 *
 * DESCRIPTION: Reads a file using a file descriptor and fills in the buffer
 * INPUT: int32_t fd = entry in file descriptor table
 * 		  void *  buf = buffer to be filled
 * 		  int32_t nbytes = length of buffer
 * OUTPUT: returns -1 if bad file name, else returns bytes copied or errors from read_data
 * SIDE_EFFECTS: Fills in buffer to be printed
 */
int32_t fileRead(int32_t fd, void * buf, int32_t nbytes){
	//Make sure we have file descriptor
	if(fd < 0 || fd > MAX_NUM_FDT_ENTRIES) {
		return ERROR_VAL;
	}

	file_descriptor_entry_t * current_process_fdt = PCB_ptrs[cur_pid]->process_fdt;
	return read_data (current_process_fdt[fd].inodeNum, current_process_fdt[fd].file_position, buf, nbytes);
}

void initialize_fops(){
	stdin_fops.open = terminalOpen;
	stdin_fops.close = terminalClose;
	stdin_fops.read = terminalRead;
	stdin_fops.write = dummy_write;
	
	stdout_fops.open = terminalOpen;
	stdout_fops.close = terminalClose;
	stdout_fops.read = dummy_read;
	stdout_fops.write = terminalWrite;
	
	regfile_fops.open = fileOpen;
	regfile_fops.close = fileClose;
	regfile_fops.read = fileRead;
	regfile_fops.write = fileWrite;
	
	directory_fops.open = directoryOpen;
	directory_fops.close = directoryClose;
	directory_fops.read = directoryRead;
	directory_fops.write = directoryWrite;
	
	RTC_fops.open = RTCOpen;
	RTC_fops.close = RTCClose;
	RTC_fops.read = RTCRead;
	RTC_fops.write = RTCWrite;
	
	cur_pid = -1;
}

void initialize_PCB_pointers() {
	int index;
	for(index = 0; index < MAX_PROCESSES; index ++) {
		PCB_ptrs[index] = NULL;
	}
}

void initialize_FDT(int32_t pid) {
	int index;
	file_descriptor_entry_t * fdt = PCB_ptrs[pid]->process_fdt;

	//Initialize all fdt entries
	for(index = 0; index < MAX_NUM_FDT_ENTRIES; index ++) {
		fdt[index].fops_pointer = NULL;
		fdt[index].inodeNum = NULL;
		fdt[index].file_position = -1;
		fdt[index].flags=0;
	}

	//Initialize stdin/stdout fops pointers, flags, and file positions
	fdt[STDIN_INDEX_IN_FDT].fops_pointer = &stdin_fops; 
	fdt[STDIN_INDEX_IN_FDT].flags = STDIN_FLAG;
	fdt[STDIN_INDEX_IN_FDT].file_position = 0;
	(*fdt[STDIN_INDEX_IN_FDT].fops_pointer->open)(NULL);

	fdt[STDOUT_INDEX_IN_FDT].fops_pointer = &stdout_fops;
	fdt[STDOUT_INDEX_IN_FDT].flags = STDOUT_FLAG;
	fdt[STDOUT_INDEX_IN_FDT].file_position = 0;
	(*fdt[STDOUT_INDEX_IN_FDT].fops_pointer->open)(NULL);
}


int32_t sys_halt (uint8_t status) {
	uint32_t parents_pid;
	uint32_t EAX_VAL = (uint32_t) status + (uint32_t) PCB_ptrs[cur_pid]->exception_flag; // RET VAL

	//Identify parent pid
	parents_pid = PCB_ptrs[cur_pid] -> parent_pid;
	
	//We are ending highest level process
	if(parents_pid == -1) {

		//TODO: AT this point, we just have to launch the shell again ******************************


		//Disable paging for virtual program space
		// page_directory[PROCESS_PAGING_INDEX].present = 0;
		// fdt = NULL;
		// PCB_ptrs[cur_pid] = NULL; //we no longer need the PCB for the current process
		// cur_pid = -1;
	}

	//We are ending a child process
	else{
		page_directory[PROCESS_PAGING_INDEX] = PCB_ptrs[parents_pid] -> pde; //Restore paging for parent process
		fdt = PCB_ptrs[parents_pid] -> process_fdt; //Update global fdt pointer
		PCB_ptrs[cur_pid] = NULL; //we no longer need the PCB for the current process
		cur_pid = parents_pid;

		//Restore EBP and ESP
		uint32_t register_val;
		register_val = PCB_ptrs[cur_pid] -> esp;
		//TODO: Double check the inline assembly
		asm("movl %0, %%esp;"
			:
			:"r"(register_val)
			:"memory");
		register_val = PCB_ptrs[cur_pid] -> ebp;
		asm("movl %0, %%ebp;"
			:
			:"r"(register_val)
			:"memory");
	}

	// TODO: need to close all relevant FD

	// TODO: fix Exception handlers to call sys_halt

	// Load EAX with the status value

		asm("movl %0, %%eax;"
			:
			:"r"(EAX_VAL)
			:"memory");
	
	asm volatile("jmp jump_point");

	//If we have reached this point, then there is an error
	return ERROR_VAL;
}

int32_t sys_execute (const uint8_t* command){
	int pid=0;
	uint8_t * args = NULL;
	uint8_t fname[32];
	int i;
	//Make sure arg is valid
	if(command == NULL) {
		return ERROR_VAL;
	}

	// Initializing fname to all '\0'
	for(i=0; i<32; i++){
		fname[i] = '\0';
	}
	
	// Fill in filename
	i=0;
	while(command[i] != '\0' && command[i] != ' ' && i<32){
		fname[i] = command[i];
		i++;
	}
	
	// Ignore spaces between name and args
	i=0;
	while(command[i] == ' '){
		i++;
	}
	
	// If there are args, set the args ptr to be used later in the PCB
	if(command[i] != '\0'){
		args = (uint8_t *) &command[i];
	}
	
	//Make sure our file is valid
	dentry_t new_dentry;
	if(read_dentry_by_name(fname, &new_dentry) == -1) {
		return ERROR_VAL;
	}

	//Check to make sure file is an executable
	uint8_t testBuffer[EXECUTABLE_CHECK_BUFFER_SIZE];
	if(read_data(new_dentry.inodeNum, 0, testBuffer, EXECUTABLE_CHECK_BUFFER_SIZE) == -1) {
		return ERROR_VAL;
	}
	if(testBuffer[0] != EXECUTABLE_FIRST_BYTE || testBuffer[1] != EXECUTABLE_SECOND_BYTE
		|| testBuffer[2] != EXECUTABLE_THIRD_BYTE || testBuffer[3] != EXECUTABLE_FOURTH_BYTE) {
		return ERROR_VAL;
	}

	//Determine the offset from which we start reading instructions in the program
	uint32_t program_eip = 0;
	if(read_data(new_dentry.inodeNum, EXECUTABLE_INSTRUCTION_START_BYTE, (uint8_t *) program_eip, EXECUTABLE_CHECK_BUFFER_SIZE) == -1) {
		return ERROR_VAL;
	}

	//Find the next available process id 
	while(pid < MAX_PROCESSES && PCB_ptrs[pid] != NULL) {
		pid++;
	}
	
	//Check if we are already running the maximum number of processes
	if(pid == MAX_PROCESSES){
		return ERROR_VAL;
	}

	//Start populating PCB data
	PCB_ptrs[pid] = (PCB_t *) EIGHT_MB - ((pid + 1) * EIGHT_KB);	//Making space at top of process's kernel stack
	PCB_ptrs[pid] -> pid = pid;
	PCB_ptrs[pid] -> parent_pid = cur_pid;
	cur_pid = pid;
	PCB_ptrs[pid] -> exception_flag = 0;
	PCB_ptrs[pid] -> pde.page_table_address = (PROCESS_BASE_4KB_ALIGNED_ADDRESS + pid * FOUR_MB)>> PDE_PTE_ADDRESS_SHIFT;
	PCB_ptrs[pid] -> pde.open_bits = 0;
	PCB_ptrs[pid] -> pde.reserved_1 = 0;
	PCB_ptrs[pid] -> pde.page_size = 1;
	PCB_ptrs[pid] -> pde.reserved_2 = 0;
	PCB_ptrs[pid] -> pde.accessed = 0;
	PCB_ptrs[pid] -> pde.cache_disable = 0;
	PCB_ptrs[pid] -> pde.write_through = 0;
	PCB_ptrs[pid] -> pde.user_supervisor = 0;
	PCB_ptrs[pid] -> pde.read_write_permissions = 1;
	PCB_ptrs[pid] -> pde.present = 1;
	PCB_ptrs[pid] -> arg_ptr = args;
	initialize_FDT(pid); //This will populate the corresponding process_fdt field of PCB_ptrs[pid]

	//Initialize paging for the process image (corresponds to virtual address 128 MB)
	page_directory[PROCESS_PAGING_INDEX] = PCB_ptrs[pid] -> pde;

	//Perform loading procedure
	if(read_data(new_dentry.inodeNum, 0, (uint8_t *) PROGRAM_INIT_VIRTUAL_ADDR, FOUR_MB) == -1) {
		return ERROR_VAL;
	}

	//Flush the TLB for each new program
	clearTLB();

	// Perform context switch
 	tss.esp0 = (PROCESS_BASE_4KB_ALIGNED_ADDRESS + (pid + 1) * FOUR_MB) - 4;//update the process's kernel-mode stack pointer
	uint32_t ptr;
	asm("movl %%esp, %0;"
		:"=r"(ptr)
		:
		:"memory");
	PCB_ptrs[pid] -> esp = ptr;
	asm("movl %%ebp, %0;"
		:"=r"(ptr)
		:
		:"memory");
	PCB_ptrs[pid] -> ebp = ptr;

 	switch_to_user_mode(program_eip);
	
	asm volatile ("jump_point:");
	asm volatile ("leave");
	asm volatile ("ret");

	//If we have reached this point, then there is an error
	return ERROR_VAL;
}

int32_t sys_read (int32_t fd, void* buf, int32_t bytes) {
	file_descriptor_entry_t * fd_table = PCB_ptrs[cur_pid]->process_fdt;

	/* Check if fd is valid index */
	if(fd == 1|| fd > MAX_NUM_FDT_ENTRIES) return ERROR_VAL;

	/* Ensure file is in use */
	if (fd_table[fd].flags == 0) return ERROR_VAL;

	/* Call respective function */
	return (fdt[fd].fops_pointer->read)(fd, buf, nbytes);
}

int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes){
	file_descriptor_entry_t * fd_table = PCB_ptrs[cur_pid]->process_fdt;

	/* Check if fd is valid index */
	if(fd == 1|| fd > MAX_NUM_FDT_ENTRIES) return ERROR_VAL;

	/* Ensure file is in use */
	if (fd_table[fd].flags == 0) return ERROR_VAL;

	/* Call respective function */
	return (fdt[fd].fops_pointer->write)(fd, buf, nbytes);
}

int32_t sys_open (const uint8_t* filename){
	int index;
	file_descriptor_entry_t * fdt = PCB_ptrs[cur_pid]->process_fdt;
	if(strncmp((const int8_t *) filename,(const int8_t *) "stdin", 5) == 0) {
		index = 0; 
	}
	else if(strncmp((const int8_t *) filename, (const int8_t *) "stdout", 6) == 0) {
		index = 1;
	}
	else {
		index = 2;
		int filetype;
		dentry_t new_dentry;

		//Check to make sure FDT isn't full
		while(index < MAX_NUM_FDT_ENTRIES && fdt[index].flags > 0) {
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
		filetype = new_dentry.fileType;

		//If we have a regular file, then update the inode pointer
		if(filetype == REGULAR_FILE_FILETYPE) {
			fdt[index].inodeNum = new_dentry.inodeNum;
		}
		else {
			fdt[index].inodeNum = NULL;
		}
		
		//update the file position
		fdt[index].file_position =0;

		//Mark as used

		//Call the respective open function
		switch(filetype) {
			case RTC_DEVICE_FILETYPE:
				fdt[index].fops_pointer = &RTC_fops;
				fdt[index].flags = RTC_FLAG;
				break;

			case DIRECTORY_FILETYPE:
				fdt[index].fops_pointer = &directory_fops;
				fdt[index].flags = DIRECTORY_FLAG;
				break;

			case REGULAR_FILE_FILETYPE: 
				fdt[index].fops_pointer = &regfile_fops;
				fdt[index].flags = REGFILE_FLAG;
				break;

			default:
				return ERROR_VAL;
		}	
		(*fdt[index].fops_pointer->open)(filename);
	}
	return index;
}


int32_t sys_close (int32_t fd) {
	file_descriptor_entry_t * fdt = PCB_ptrs[cur_pid]->process_fdt;
	if(fd < 2 || fd > MAX_NUM_FDT_ENTRIES) {
		return ERROR_VAL;
	}

	(*fdt[fd].fops_pointer->close)(fd);
	fdt[fd].flags = 0;
	return 0;
}

int32_t sys_getargs (uint8_t* buf, int32_t nbytes){
	strncpy((int8_t *) buf, (const int8_t *) PCB_ptrs[cur_pid] -> arg_ptr, nbytes);
	return 0;
}

int32_t sys_vidmap (uint8_t** screen_start){

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
