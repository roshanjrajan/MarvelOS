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
	page_directory[PAGE_TABLE_INDEX].page_table_address = ((uint32_t) page_table) >> PDE_PTE_ADDRESS_SHIFT;
	page_directory[PAGE_TABLE_INDEX].open_bits = 0;
	page_directory[PAGE_TABLE_INDEX].reserved_1 = 0;
	page_directory[PAGE_TABLE_INDEX].page_size	= 0;
	page_directory[PAGE_TABLE_INDEX].reserved_2 = 0;
	page_directory[PAGE_TABLE_INDEX].accessed = 0;
	page_directory[PAGE_TABLE_INDEX].cache_disable = 0;
	page_directory[PAGE_TABLE_INDEX].write_through = 0;
	page_directory[PAGE_TABLE_INDEX].user_supervisor = 0;
	page_directory[PAGE_TABLE_INDEX].read_write_permissions = 1; 
	page_directory[PAGE_TABLE_INDEX].present = 1;

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
		page_table[i].present = 0;
	}

	/* initialize the pages in the page table for video memory entries */
	page_table[VIDEO_MEM_INDEX].physical_address = VIDEO_4KB_ALIGNED_ADDRESS >> PDE_PTE_ADDRESS_SHIFT;
	page_table[VIDEO_MEM_INDEX].open_bits = 0;
	page_table[VIDEO_MEM_INDEX].global = 1;
	page_table[VIDEO_MEM_INDEX].reserved_1 = 0;
	page_table[VIDEO_MEM_INDEX].dirty	= 0;
	page_table[VIDEO_MEM_INDEX].accessed = 0;
	page_table[VIDEO_MEM_INDEX].cache_disable = 0;
	page_table[VIDEO_MEM_INDEX].write_through = 0;
	page_table[VIDEO_MEM_INDEX].user_supervisor = 0; 
	page_table[VIDEO_MEM_INDEX].read_write_permissions = 1;
	page_table[VIDEO_MEM_INDEX].present = 1;

	// Enable Paging
	enablePaging();
	
}
