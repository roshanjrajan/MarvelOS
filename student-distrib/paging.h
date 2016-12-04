#ifndef PAGING_H
#define PAGING_H

#include "types.h"
#include "lib.h"

// General Constants
#define PAGE_ENABLE_MASK 0x80000000
#define NUM_PAGE_TABLES 1024
#define NUM_PAGES_IN_TABLE 1024
#define MIXED_SIZE_PAGE_MASK 0x00000010
#define SIZE_4KB_PAGE 4096
#define KERNEL_PAGE_TABLE_INDEX 0
#define VIDEO_MEM_INDEX 184
#define KERNEL_MEM_INDEX 1
#define KERNEL_4KB_ALIGNED_ADDRESS 0x00400000
#define USER_PAGE_TABLE_INDEX 128
#define USER_VIDEO_MEM_INDEX 0
#define PDE_PTE_ADDRESS_SHIFT 12
#define USER_PAGE_TABLE_INDEX 128
#define USER_VIDEO_MEM_INDEX 0
#define VIDEO_4KB_ALIGNED_ADDRESS 0xB8000

/* A page table entry struct */
typedef union pte_desc_t {
	uint32_t val;
	struct {
		uint8_t present: 1;
		uint8_t read_write_permissions: 1;
		uint8_t user_supervisor: 1;
		uint8_t write_through: 1;
		uint8_t cache_disable: 1;
		uint8_t accessed: 1;
		uint8_t dirty: 1;
		uint8_t reserved_1: 1;
		uint8_t global: 1;
		uint8_t open_bits: 3;
		uint32_t physical_address: SIZE_PHYSICAL_ADDRESS;
	} __attribute__((packed));
} pte_desc_t;


// Paging Directory and Tables
pde_desc_t page_directory[NUM_PAGE_TABLES] __attribute__((aligned(SIZE_4KB_PAGE)));
pte_desc_t kernel_page_table[NUM_PAGES_IN_TABLE] __attribute__((aligned(SIZE_4KB_PAGE)));
pte_desc_t user_page_table[NUM_PAGES_IN_TABLE] __attribute__((aligned(SIZE_4KB_PAGE)));

// Paging Functions
extern void loadPageDirectory(uint32_t * address);
extern void enablePaging(void);
extern void enableMixedSizes(void);
extern void clearTLB(void);
extern void init_paging(void);

#endif
