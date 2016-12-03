#include "paging.h"


/* 
 * init_paging
 *
 * DESCRIPTION: Initialize the  Paging Directory and Table
 * 				and fills in the used fields for vmem and kernel
 * INPUTS: none.
 * OUTPUS: none.
 * SIDE_EFFECTS: Paging Table is initialized
 */
void init_paging() {
	int i;

	/* Load the page directory into cr3 */
	loadPageDirectory((uint32_t *) page_directory);

	/* Initialize paging */
	enableMixedSizes();
	clearTLB();

	/* All the page directory entries are initially zero */
	for(i = 0; i < NUM_PAGE_TABLES; i++) {
		page_directory[i].present= 0;
	}

	/* Initialize the video memory entries of PDE */
	page_directory[KERNEL_PAGE_TABLE_INDEX].page_table_address = ((uint32_t) kernel_page_table) >> PDE_PTE_ADDRESS_SHIFT;
	page_directory[KERNEL_PAGE_TABLE_INDEX].open_bits = 0;
	page_directory[KERNEL_PAGE_TABLE_INDEX].reserved_1 = 0;
	page_directory[KERNEL_PAGE_TABLE_INDEX].page_size	= 0;
	page_directory[KERNEL_PAGE_TABLE_INDEX].reserved_2 = 0;
	page_directory[KERNEL_PAGE_TABLE_INDEX].accessed = 0;
	page_directory[KERNEL_PAGE_TABLE_INDEX].cache_disable = 0;
	page_directory[KERNEL_PAGE_TABLE_INDEX].write_through = 0;
	page_directory[KERNEL_PAGE_TABLE_INDEX].user_supervisor = 0;
	page_directory[KERNEL_PAGE_TABLE_INDEX].read_write_permissions = 1; 
	page_directory[KERNEL_PAGE_TABLE_INDEX].present = 1;

	/* Now initialize the page directory for the kernel memory entry */
	page_directory[KERNEL_MEM_INDEX].page_table_address = KERNEL_4KB_ALIGNED_ADDRESS >> PDE_PTE_ADDRESS_SHIFT;
	page_directory[KERNEL_MEM_INDEX].open_bits = 0;
	page_directory[KERNEL_MEM_INDEX].reserved_1 = 0;
	page_directory[KERNEL_MEM_INDEX].page_size	= 1;
	page_directory[KERNEL_MEM_INDEX].reserved_2 = 0;
	page_directory[KERNEL_MEM_INDEX].accessed = 0;
	page_directory[KERNEL_MEM_INDEX].cache_disable = 0;
	page_directory[KERNEL_MEM_INDEX].write_through = 0;
	page_directory[KERNEL_MEM_INDEX].user_supervisor = 0;
	page_directory[KERNEL_MEM_INDEX].read_write_permissions = 1; 
	page_directory[KERNEL_MEM_INDEX].present = 1;

	/* Initialize all the page table entries to unused  */
	for(i=0; i< NUM_PAGES_IN_TABLE; i++) {
		kernel_page_table[i].present = 0;
	}

	/* initialize the pages in the page table for video memory entries */
	for(i=0; i<=NUM_TERMINALS; i++) {
		kernel_page_table[VIDEO_MEM_INDEX+ i].physical_address = (VIDEO_4KB_ALIGNED_ADDRESS + i * FOUR_KB) >> PDE_PTE_ADDRESS_SHIFT;
		kernel_page_table[VIDEO_MEM_INDEX+ i].open_bits = 0;
		kernel_page_table[VIDEO_MEM_INDEX+ i].global = 1;
		kernel_page_table[VIDEO_MEM_INDEX+ i].reserved_1 = 0;
		kernel_page_table[VIDEO_MEM_INDEX+ i].dirty	= 0;
		kernel_page_table[VIDEO_MEM_INDEX+ i].accessed = 0;
		kernel_page_table[VIDEO_MEM_INDEX+ i].cache_disable = 0;
		kernel_page_table[VIDEO_MEM_INDEX+ i].write_through = 0;
		kernel_page_table[VIDEO_MEM_INDEX+ i].user_supervisor = 0; 
		kernel_page_table[VIDEO_MEM_INDEX+ i].read_write_permissions = 1;
		kernel_page_table[VIDEO_MEM_INDEX+ i].present = 1;
	}

	/* Determine a new virtual address for user to access vidmap data
		We arbitrarily choose the virtual address of 512 MB for vidmap.
		We check to make sure the page is not used, and then set up the PTE */

	/* Check to make sure that the page isn't already used */
	page_directory[USER_PAGE_TABLE_INDEX].page_table_address = ((uint32_t) user_page_table) >> PDE_PTE_ADDRESS_SHIFT;
	page_directory[USER_PAGE_TABLE_INDEX].open_bits = 0;
	page_directory[USER_PAGE_TABLE_INDEX].reserved_1 = 0;
	page_directory[USER_PAGE_TABLE_INDEX].page_size	= 0;
	page_directory[USER_PAGE_TABLE_INDEX].reserved_2 = 0;
	page_directory[USER_PAGE_TABLE_INDEX].accessed = 0;
	page_directory[USER_PAGE_TABLE_INDEX].cache_disable = 0;
	page_directory[USER_PAGE_TABLE_INDEX].write_through = 0;
	page_directory[USER_PAGE_TABLE_INDEX].user_supervisor = 1;
	page_directory[USER_PAGE_TABLE_INDEX].read_write_permissions = 1; 
	page_directory[USER_PAGE_TABLE_INDEX].present = 1;

	/* Initialize all the user page table entries to unused  */
	for(i=0; i< NUM_PAGES_IN_TABLE; i++) {
		user_page_table[i].present = 0;
	}

	/* initialize the pages in the user page table for video memory entries for multiple terminals */

	//Terminal 0 by default corresponds to VGA display memory
	user_page_table[USER_VIDEO_MEM_INDEX].physical_address = VIDEO_4KB_ALIGNED_ADDRESS >> PDE_PTE_ADDRESS_SHIFT;
	user_page_table[USER_VIDEO_MEM_INDEX].open_bits = 0;
	user_page_table[USER_VIDEO_MEM_INDEX].global = 1;
	user_page_table[USER_VIDEO_MEM_INDEX].reserved_1 = 0;
	user_page_table[USER_VIDEO_MEM_INDEX].dirty	= 0;
	user_page_table[USER_VIDEO_MEM_INDEX].accessed = 0;
	user_page_table[USER_VIDEO_MEM_INDEX].cache_disable = 0;
	user_page_table[USER_VIDEO_MEM_INDEX].write_through = 0;
	user_page_table[USER_VIDEO_MEM_INDEX].user_supervisor = 1; 
	user_page_table[USER_VIDEO_MEM_INDEX].read_write_permissions = 1;
	user_page_table[USER_VIDEO_MEM_INDEX].present = 1;

	for(i=1; i<NUM_TERMINALS; i++) {
		user_page_table[USER_VIDEO_MEM_INDEX + i].physical_address = (VIDEO_4KB_ALIGNED_ADDRESS +  (i + 1) * FOUR_KB) >> PDE_PTE_ADDRESS_SHIFT;
		user_page_table[USER_VIDEO_MEM_INDEX + i].open_bits = 0;
		user_page_table[USER_VIDEO_MEM_INDEX + i].global = 1;
		user_page_table[USER_VIDEO_MEM_INDEX + i].reserved_1 = 0;
		user_page_table[USER_VIDEO_MEM_INDEX + i].dirty	= 0;
		user_page_table[USER_VIDEO_MEM_INDEX + i].accessed = 0;
		user_page_table[USER_VIDEO_MEM_INDEX + i].cache_disable = 0;
		user_page_table[USER_VIDEO_MEM_INDEX + i].write_through = 0;
		user_page_table[USER_VIDEO_MEM_INDEX + i].user_supervisor = 1; 
		user_page_table[USER_VIDEO_MEM_INDEX + i].read_write_permissions = 1;
		user_page_table[USER_VIDEO_MEM_INDEX + i].present = 1;
	}

	// Enable Paging
	enablePaging();
}
