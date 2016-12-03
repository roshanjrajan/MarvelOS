#include "systemcall.h"


int32_t switchTerminal(uint8_t previousTerminal) {
	//Make sure we have valid new terminal index 
	if(previousTerminal > TERMINAL_2 || currentTerminal > TERMINAL_2) {
		return ERROR_VAL;
	}

	//Save current terminal output to respective terminal memory
	memcpy((char *) (VIDEO + (previousTerminal + 1) * FOUR_KB), (const char *)VIDEO, FOUR_KB);

	//Move new terminal memory to display memory
	memcpy((char *)VIDEO, (const char *) (VIDEO + (currentTerminal + 1) * FOUR_KB), FOUR_KB);

	//Set location for cursors in new terminal
	set_cursor(screen_x[currentTerminal], screen_y[currentTerminal]);

	//Update the physical addresses that the different terminal video pages correspond to
	unsigned int flags;
	cli_and_save(flags);
	user_page_table[USER_VIDEO_MEM_INDEX + previousTerminal].physical_address = (VIDEO_4KB_ALIGNED_ADDRESS +  (previousTerminal + 1) * FOUR_KB) >> PDE_PTE_ADDRESS_SHIFT;
	user_page_table[USER_VIDEO_MEM_INDEX + currentTerminal].physical_address = VIDEO_4KB_ALIGNED_ADDRESS >> PDE_PTE_ADDRESS_SHIFT;
	clearTLB();
	restore_flags(flags);
	//KBputc((char) currentTerminal);


////////////////////////////////////////
	//**************************TODO: Modify execute so that first 3 pids are for sure terminal processes 

	//pause the current process	
	//PCB_ptrs[cur_pid] -> pause_process_flag = 1;

	//save current eip	
	//uint32_t cur_eip = get_eip();
	//PCB_ptrs[cur_pid] -> current_eip = cur_eip;

	//New terminal has been launched before, and already has a child, so launch child
	//New terminal has been launched before, but doesn't have a child, so resume terminal

///////////////////////////////////////////
	return 0;
}

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
	int bytesRead = read_data(fdt[fd].inodeNum, fdt[fd].file_position, buf, nbytes);
	if(bytesRead == ERROR_VAL)
		return ERROR_VAL;
	fdt[fd].file_position += bytesRead;
	return bytesRead;
}


/* 
 * directoryRead
 *
 * DESCRIPTION: Reads a single file and outputs to buffer
 * INPUT: int32_t fd - index in the fdt
 *  	  void * buf - buffer to be filled that stories file name
 * 		  int32_t nbytes - size of buffer
 * OUTPUT: returns -1 if bad file index else size of file name
 * SIDE_EFFECTS: Fills up the buffer with a file name
 */
int32_t directoryRead(int32_t fd, void * buf, int32_t nbytes){
	
	// Check if valid fdt entry
	if(fd < 0 || fd > MAX_NUM_FDT_ENTRIES) {
		return ERROR_VAL;
	}

	// Make sure to read through all the files entries
	if(fdt[fd].file_position == fileSysBootBlock->numDirectories) {
		fdt[fd].file_position = 0;
		return 0;
	}

	dentry_t dentry;
	if(read_dentry_by_index (fdt[fd].file_position, &dentry) != 0)
		return ERROR_VAL; // Done with all of them
		
	strncpy((int8_t* )buf, (const int8_t*)(dentry.fileName), (uint32_t) MAX_FILENAME_LENGTH);
	fdt[fd].file_position++;
	return MAX_FILENAME_LENGTH;
}



/*  
 * initialize_fops
 *
 * DESCRIPTION: Setup the initiliztion of the file operations for each of the 5 file types
 * INPUT:  Nothing
 * OUTPUT: Nothing
 * SIDE_EFFECTS: Fills in fops for 5 file types
 */
void initialize_fops(){

	// Initialize each of the fops table
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
	
	// Initilize current pid
	cur_pid = -1;
}


/*  
 * initialize_PCB_pointers
 *
 * DESCRIPTION: Initialize PCB Table
 * INPUT:  Nothing
 * OUTPUT: Nothing
 * SIDE_EFFECTS: Fills in PCB table with each index as NULL
 */
void initialize_PCB_pointers() {
	int index;
	for(index = 0; index < MAX_PROCESSES; index ++) {
		PCB_ptrs[index] = NULL;
	}
}

/*  
 * initialize_FDT
 *
 * DESCRIPTION: Initialize the FDT for each process
 * INPUT:  int32_t pid - pid for current process
 * OUTPUT: Nothing
 * SIDE_EFFECTS: Initialize each entry in the fdt for each process
 */
void initialize_FDT(int32_t pid) {
	int index;
	fdt = PCB_ptrs[pid]->process_fdt;

	//Initialize all fdt entries
	for(index = 0; index < MAX_NUM_FDT_ENTRIES; index ++) {
		fdt[index].fops_pointer = NULL;
		fdt[index].inodeNum = NULL;
		fdt[index].file_position = -1;
		fdt[index].flags=UNUSED_FLAG;
	}

	//Initialize stdin/stdout fops pointers, flags, and file positions
	fdt[STDIN_INDEX_IN_FDT].fops_pointer = &stdin_fops; 
	fdt[STDIN_INDEX_IN_FDT].flags = STDIN_FLAG;
	fdt[STDIN_INDEX_IN_FDT].file_position = 0;

	fdt[STDOUT_INDEX_IN_FDT].fops_pointer = &stdout_fops;
	fdt[STDOUT_INDEX_IN_FDT].flags = STDOUT_FLAG;
	fdt[STDOUT_INDEX_IN_FDT].file_position = 0;
}

/*  
 * sys_halt
 *
 * DESCRIPTION: Halt the current process
 * INPUT:  uint8_t status - the status of the current process
 * OUTPUT: int32_t retVal - NOT USED -> stored in eax directly
 * SIDE_EFFECTS: End current process and setup for parent process
 */
int32_t sys_halt (uint8_t status) {
	uint32_t parents_pid;
	eax_val = (uint32_t) status + (uint32_t) PCB_ptrs[cur_pid]->exception_flag; // RET VAL
	int32_t index;
	uint32_t register_val_esp, register_val_ebp;

	// store current esp and ebp
	asm("movl %%esp, %0;"
		:"=r"(register_val_esp)
		:
		:"memory");

	asm("movl %%ebp, %0;"
		:"=r"(register_val_ebp)
		:
		:"memory");

	//Identify parent pid
	parents_pid = PCB_ptrs[cur_pid] -> parent_pid;
	
	//Close all relevant FD
	for(index = 0; index < MAX_NUM_FDT_ENTRIES; index ++) {
		if(fdt[index].flags != UNUSED_FLAG) {
			(*fdt[index].fops_pointer->close)(index);
		}
	}

	//We are ending highest level process
	if(parents_pid == -1) {
		// Restart shell
		PCB_ptrs[cur_pid] = NULL; //we no longer need the PCB for the current process
		cur_pid = -1;
		sys_execute((uint8_t *)"shell");
	}

	//We are ending a child process
	else{
		//Restore parent EBP and ESP
		register_val_esp = PCB_ptrs[cur_pid] -> esp;
		register_val_ebp = PCB_ptrs[cur_pid] -> ebp;

		// restore parent process info
		page_directory[PROCESS_PAGING_INDEX] = PCB_ptrs[parents_pid] -> pde; //Restore paging for parent process
		clearTLB();
		fdt = PCB_ptrs[parents_pid] -> process_fdt; //Update global fdt pointer
		PCB_ptrs[cur_pid] = NULL; //we no longer need the PCB for the current process
		cur_pid = parents_pid;

		// Update TSS stack pointer to parent stack pointer
 		tss.esp0 = (PROCESS_BASE_4KB_ALIGNED_ADDRESS - cur_pid * EIGHT_KB) - 4;
	}

	
	// Store esp and ebp
	asm("movl %0, %%esp;"
		:
		:"r"(register_val_esp)
		:"memory");

	asm("movl %0, %%ebp;"
		:
		:"r"(register_val_ebp)
		:"memory");	

	// Jump to end of execute
	asm volatile("jmp jump_point");

	//If we have reached this point, then there is an error
	return ERROR_VAL;
}

/*  
 * init_process_paging
 *
 * DESCRIPTION: Initiliazes paging for new process
 * INPUT:  int pid - process id
 * OUTPUT: NONE
 * SIDE_EFFECTS: Sets up page directory for program, user space (vidmap)
 */
static void init_process_paging(int pid) {
	//Initialize paging for the process image (corresponds to virtual address 128 MB)
	page_directory[PROCESS_PAGING_INDEX].page_table_address = (PROCESS_BASE_4KB_ALIGNED_ADDRESS + pid * FOUR_MB)>> PDE_PTE_ADDRESS_SHIFT;
	page_directory[PROCESS_PAGING_INDEX].open_bits = 0;
	page_directory[PROCESS_PAGING_INDEX].reserved_1 = 0;
	page_directory[PROCESS_PAGING_INDEX].page_size = 1;
	page_directory[PROCESS_PAGING_INDEX].reserved_2 = 0;
	page_directory[PROCESS_PAGING_INDEX].accessed = 0;
	page_directory[PROCESS_PAGING_INDEX].cache_disable = 0;
	page_directory[PROCESS_PAGING_INDEX].write_through = 0;
	page_directory[PROCESS_PAGING_INDEX].user_supervisor = 1;
	page_directory[PROCESS_PAGING_INDEX].read_write_permissions = 1;
	page_directory[PROCESS_PAGING_INDEX].present = 1;

	//Flush the TLB for each new program
	clearTLB();
}

/*  
 * sys_execute
 *
 * DESCRIPTION: Initialize new process
 * INPUT:  uint8_t* command - filename + args
 * OUTPUT: int32_t retVal - NOT USED -> stored in eax directly
 * SIDE_EFFECTS: Begin and initialize new process
 */
int32_t sys_execute (const uint8_t* command){
	int pid=0;
	uint8_t args[ARG_SIZE];
	uint8_t fname[FNAME_SIZE];
	int i;

	//Make sure arg is valid
	if(command == NULL) {
		return ERROR_VAL;
	}

	//Find the next available process id 
	while(pid < MAX_PROCESSES && PCB_ptrs[pid] != NULL) {
		pid++;
	}

	//if(pid<3 && pid!=0)
	//	pid=3;
	
	//Check if we are already running the maximum number of processes
	if(pid == MAX_PROCESSES){
		return ERROR_VAL;
	}

	// Initializing fname to all '\0'
	for(i=0; i<FNAME_SIZE; i++){
		fname[i] = '\0';
	}
	
	// Fill in filename
	i=0;
	while(command[i] != '\0' && command[i] != ' ' && i<FNAME_SIZE){
		fname[i] = command[i];
		i++;
	}
	
	// Ignore spaces between name and args
	//i=0;
	while(command[i] == ' '){
		i++;
	}
	
	// If there are args, set the args ptr to be used later in the PCB
	if(command[i] != '\0'){
		strncpy((int8_t *) args, (const int8_t *)&command[i], (uint32_t)ARG_SIZE_WITHOUT_NULL_TERMINATOR);
		args[ARG_NULL_TERMINATOR_INDEX]='\0';
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
	if(testBuffer[FIRST_BYTE] != EXECUTABLE_FIRST_BYTE || testBuffer[SECOND_BYTE] != EXECUTABLE_SECOND_BYTE
		|| testBuffer[THIRD_BYTE] != EXECUTABLE_THIRD_BYTE || testBuffer[FOURTH_BYTE] != EXECUTABLE_FOURTH_BYTE) {
		return ERROR_VAL;
	}

	//Determine the offset from which we start reading instructions in the program
	uint32_t program_eip;
	if(read_data(new_dentry.inodeNum, EXECUTABLE_INSTRUCTION_START_BYTE, (uint8_t *) &program_eip, EXECUTABLE_CHECK_BUFFER_SIZE) == -1) {
		return ERROR_VAL;
	}

	// Initilaze all pages for current process 
	init_process_paging(pid);

	//Start populating PCB data
	PCB_ptrs[pid] = (PCB_t *) (EIGHT_MB - ((pid + 1) * EIGHT_KB));	//Making space at top of process's kernel stack
	PCB_ptrs[pid] -> pid = pid;

	
	PCB_ptrs[pid] -> parent_pid = cur_pid;
	if(PCB_ptrs[pid] -> parent_pid == -1){
		PCB_ptrs[pid] -> parent_terminal = curThread;
	}else{
		PCB_ptrs[pid] -> parent_terminal = PCB_ptrs[PCB_ptrs[pid] -> parent_pid] -> parent_terminal;
	}
	PCB_ptrs[pid] -> has_child_flag = 0;
	
	//We are running a process within a terminal
	//if(pid > TERMINAL_2) {
	//	PCB_ptrs[pid] -> parent_pid = cur_pid;
	//	PCB_ptrs[pid] -> parent_terminal = currentTerminal;
	//	PCB_ptrs[currentTerminal] -> has_child_flag = 1;
	//}
	//We are running the first shell on a new terminal
	//else{
	//	PCB_ptrs[pid] -> parent_pid = cur_pid;
	//	PCB_ptrs[pid] -> parent_terminal = PCB_ptrs[PCB_ptrs[pid] -> parent_pid] -> parent_terminal;
	//	PCB_ptrs[pid] -> has_child_flag = 0;
	//}

	cur_pid = pid;
	PCB_ptrs[pid] -> exception_flag = 0;
	PCB_ptrs[pid] -> pause_process_flag = 0;

	//save current eip	
	//uint32_t cur_eip = get_eip();
	//PCB_ptrs[cur_pid] -> current_eip = cur_eip;

	PCB_ptrs[pid] -> pde = page_directory[PROCESS_PAGING_INDEX];
	strncpy((int8_t *) PCB_ptrs[pid] -> arg_ptr, (const int8_t *) args, ARG_SIZE);

	initialize_FDT(pid); //This will populate the corresponding process_fdt field of PCB_ptrs[pid]

	//Perform loading procedure
	if(read_data(new_dentry.inodeNum, 0, (uint8_t *) PROGRAM_INIT_VIRTUAL_ADDR, FOUR_MB) == -1) {
		return ERROR_VAL;
	}

	// Perform context switch
 	tss.esp0 = (PROCESS_BASE_4KB_ALIGNED_ADDRESS - pid * EIGHT_KB) - LONG_BYTES;//update the process's kernel-mode stack pointer

 	// Store current esp and ebp
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

	// Switch to user program
 	switch_to_user_mode(program_eip);
	
	// End of execture
	asm volatile ("jump_point:");
	asm volatile ("leave");
	// Load EAX with the status value
	asm("movl %0, %%eax;"
		:
		:"r"(eax_val)
		:"memory");
	asm volatile ("ret");

	//If we have reached this point, then there is an error
	return ERROR_VAL;
}

/*  
 * sys_read
 *
 * DESCRIPTION: Read depending on file type
 * INPUT:  int32_t fd - index into fdt
 *		   void* buf - buffer to store data
 * 		   int32_t nbytes - number of bytes to be copied
 * OUTPUT: int32_t retVal - number of bytes copied
 * SIDE_EFFECTS: Read the different types of files
 */
int32_t sys_read (int32_t fd, void* buf, int32_t nbytes) {
	/* Check if fd is valid index */
	if(fd == STDIN_FLAG || fd > MAX_NUM_FDT_ENTRIES || fd < 0) return ERROR_VAL;
	
	/* Check valid buf and nbytes */
	if(buf == NULL || nbytes < 0) return ERROR_VAL;

	/* Ensure file is in use */
	if (fdt[fd].flags == UNUSED_FLAG) return ERROR_VAL;

	/* Call respective function */
	return (fdt[fd].fops_pointer->read)(fd, buf, nbytes);
}

/*  
 * sys_write
 *
 * DESCRIPTION: Write data in buffer depending on file type
 * INPUT:  int32_t fd - index into fdt
 *		   const void* buf - buffer to be written
 * 		   int32_t nbytes - number of bytes to be written
 * OUTPUT: int32_t retVal - number of bytes written
 * SIDE_EFFECTS: Write the buffer depending on the file type
 */
int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes){
	/* Check if fd is valid index */
	if(fd <= 0|| fd > MAX_NUM_FDT_ENTRIES) return ERROR_VAL;
	
	/* Check valid buf and nbytes */
	if(buf == NULL || nbytes < 0) return ERROR_VAL;

	/* Ensure file is in use */
	if (fdt[fd].flags == UNUSED_FLAG) return ERROR_VAL;

	/* Call respective function */
	return (fdt[fd].fops_pointer->write)(fd, buf, nbytes);
}

/*  
 * sys_open
 *
 * DESCRIPTION: Open the file type
 * INPUT:  const uint8_t* filename - name of file to be opened
 * OUTPUT: int32_t retVal - error or index in fdt
 * SIDE_EFFECTS: Initilize behavior of fdt entries
 */
int32_t sys_open (const uint8_t* filename){
	int index;
	fdt = PCB_ptrs[cur_pid]->process_fdt;
	
	/* Check valid filename */
	if(filename == NULL) return ERROR_VAL;

	// Check if opening 'stdin', which is 5 characters long
	if(strncmp((const int8_t *) filename,(const int8_t *) "stdin", 5) == 0) {
		index = 0; 
	}
	// Check if opening 'stdout', which is 6 characters long
	else if(strncmp((const int8_t *) filename, (const int8_t *) "stdout", 6) == 0) {
		index = 1;
	}
	else {
		index = NOT_IN_OUT;
		int filetype;
		dentry_t new_dentry;

		//Check to make sure FDT isn't full
		while(index < MAX_NUM_FDT_ENTRIES && fdt[index].flags != UNUSED_FLAG) {
			index++;
		}

		//FDT is full
		if(index == MAX_NUM_FDT_ENTRIES) {
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
		// Call the open function
		(*fdt[index].fops_pointer->open)(filename);
	}
	return index;
}

/*  
 * sys_close
 *
 * DESCRIPTION: Close file
 * INPUT:  int32_t fd - index in fdt
 * OUTPUT: int32_t retVal - error or not
 * SIDE_EFFECTS: Call the close function and reset flag
 */
int32_t sys_close (int32_t fd) {
	file_descriptor_entry_t * fdt = PCB_ptrs[cur_pid]->process_fdt;
	if(fd < NOT_IN_OUT || fd > MAX_NUM_FDT_ENTRIES) {
		return ERROR_VAL;
	}
	
	/* Ensure file is in use */
	if(fdt[fd].flags == UNUSED_FLAG)
		return ERROR_VAL;

	(*fdt[fd].fops_pointer->close)(fd);
	fdt[fd].flags = UNUSED_FLAG;
	return 0;
}

/*  
 * sys_getargs
 *
 * DESCRIPTION: Get the arguments 
 * INPUT:  uint8_t* buf - buffer to copy args into
 *		   int32_t nbytes - number of bytes to copy
 * OUTPUT: 0 (success), ERROR_VAL for error(buffer not big enough)
 * SIDE_EFFECTS: Get the arguments and copy 
 */
int32_t sys_getargs (uint8_t* buf, int32_t nbytes){
	
	/* Check valid buf and nbytes */
	if(buf == NULL || nbytes < 0) return ERROR_VAL;
	
	strncpy((int8_t *) buf, (const int8_t *) (PCB_ptrs[cur_pid] -> arg_ptr), ARG_SIZE);
	if(buf[ARG_NULL_TERMINATOR_INDEX] != '\0')
		return ERROR_VAL;
	return 0;
}


/*  
 * sys_vidmap
 *
 * DESCRIPTION: Enables user access to video memory
 * INPUT:  uint8_t** screen_start - pointer to user virtual video memory
 * OUTPUT: 0 (success), ERROR_VAL for error(invalid argument)
 * SIDE_EFFECTS: Changes value of * screen_start
 */
int32_t sys_vidmap (uint8_t** screen_start){
	//Make sure our argument is not null
	if(screen_start == NULL) {
		return ERROR_VAL;
	}

	//Make sure that we aren't writing to 0-8MB range (kernel space)
	if( (uint32_t) (screen_start) < EIGHT_MB) {
		return ERROR_VAL;
	}

	//Make sure paging is enabled for screen_start address
	if((uint32_t) screen_start < ONE_HUNDRED_TWENTY_EIGHT_MB || (uint32_t) screen_start >= ONE_HUNDRED_THIRTY_TWO_MB) {
		return ERROR_VAL;
	}

	//Save our new address back into the user space
	uint32_t new_address = USER_VIDMAP_ADDR + (PCB_ptrs[cur_pid] -> parent_terminal) * FOUR_KB;
	(*screen_start) = (uint8_t*) new_address;	

	return 0;
}

/*  
 * sys_sethandler
 *
 * DESCRIPTION: Changes default action when signal received
 * INPUT:  NOT USED
 * OUTPUT: 0 (success), ERROR_VAL for error(invalid argument)
 * SIDE_EFFECTS: NOTHING
 */
int32_t sys_sethandler (int32_t signum, void* handler_address){
	
	return ERROR_VAL;
}

/*  
 * sys_sigreturn
 *
 * DESCRIPTION: Copies hardware context from user to processor
 * INPUT:  NONE
 * OUTPUT: 0 (success), ERROR_VAL for error(invalid argument)
 * SIDE_EFFECTS: NOTHING
 */
int32_t sys_sigreturn (void){
	
	return ERROR_VAL;
}

/*  
 * dummy_read
 *
 * DESCRIPTION: DOES NOTHING
 * INPUT:  int32_t fd - index into fdt
 *		   void* buf - buffer to store data
 * 		   int32_t nbytes - number of bytes to be copied
 * OUTPUT: ERROR
 * SIDE_EFFECTS: NONE
 */
int32_t dummy_read (int32_t fd, void* buf, int32_t nbytes){
	return ERROR_VAL;
}

/*  
 * dummy_write
 *
 * DESCRIPTION: DOES NOTHING
 * INPUT:  int32_t fd - index into fdt
 *		   const void* buf - buffer to be written
 * 		   int32_t nbytes - number of bytes to be written
 * OUTPUT: ERROR
 * SIDE_EFFECTS: NONE
 */
int32_t dummy_write (int32_t fd, const void* buf, int32_t nbytes){
	return ERROR_VAL;
}

/*  
 * dummy_open
 *
 * DESCRIPTION: DOES NOTHING
 * INPUT:  const uint8_t* filename - name of file to be opened
 * OUTPUT: ERROR
 * SIDE_EFFECTS: NONE
 */
int32_t dummy_open (const uint8_t* filename){
	return ERROR_VAL;
}

/*  
 * dummy_close
 *
 * DESCRIPTION: DOES NOTHING
 * INPUT:  int32_t fd - index in fdt
 * OUTPUT: ERROR
 * SIDE_EFFECTS: NONE
 */
int32_t dummy_close (int32_t fd){
	return ERROR_VAL;
}
