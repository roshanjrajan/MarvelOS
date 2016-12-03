/* lib.h - Defines for useful library functions
 * vim:ts=4 noexpandtab
 */

#ifndef _LIB_H
#define _LIB_H

#include "types.h"

#define ERROR_VAL -1
#define VIDEO 0xB8000
#define NUM_TERMINALS 3
#define MAX_PROCESSES 6
#define FOUR_KB 0x00001000
#define SIZE_PHYSICAL_ADDRESS 20
#define MAX_NUM_FDT_ENTRIES 8
#define ARG_SIZE 128

// Structs used

typedef union pde_desc_t {
	uint32_t val;
	struct {
		uint8_t present: 1;
		uint8_t read_write_permissions: 1;
		uint8_t user_supervisor: 1;
		uint8_t write_through: 1;
		uint8_t cache_disable: 1;
		uint8_t accessed: 1;
		uint8_t reserved_2: 1;
		uint8_t page_size: 1;
		uint8_t reserved_1: 1;
		uint8_t open_bits: 3;
		uint32_t page_table_address: SIZE_PHYSICAL_ADDRESS;
	} __attribute__((packed));
} pde_desc_t;

typedef	struct __attribute__((packed)) fops_table
{
	int32_t (*open)(const uint8_t*);
	int32_t (*close)(int32_t);
	int32_t (*read)(int32_t, void*, int32_t);
	int32_t (*write)(int32_t, const void*, int32_t);
} fops_table_t;

typedef struct __attribute__((packed)) file_descriptor_entry
{
	fops_table_t * fops_pointer;
	uint32_t inodeNum;
	int32_t file_position;
	uint32_t flags;
} file_descriptor_entry_t;

typedef struct __attribute__((packed)) PCB {
	int32_t pid;
	int32_t parent_pid;
	pde_desc_t pde;
	uint32_t esp;
	uint32_t ebp;
	uint8_t exception_flag : 1;
	uint8_t parent_terminal;
	uint8_t has_child_flag : 1;
	uint8_t pause_process_flag : 1;
	uint32_t current_eip;
	uint8_t arg_ptr[ARG_SIZE];
	file_descriptor_entry_t process_fdt[MAX_NUM_FDT_ENTRIES];
} PCB_t; 

// Thread handling
int curThread;

//Process Handling
PCB_t* PCB_ptrs[MAX_PROCESSES];
int cur_pid;
uint8_t currentTerminal;
int screen_x[NUM_TERMINALS];
int screen_y[NUM_TERMINALS];

int32_t printf(int8_t *format, ...);
void KBputc(uint8_t c);
void putc(uint8_t c);
int32_t puts(int8_t *s);
int8_t *itoa(uint32_t value, int8_t* buf, int32_t radix);
int8_t *strrev(int8_t* s);
uint32_t strlen(const int8_t* s);
void clear(void);

void set_cursor(int x, int y);
void text_shift_up(int terminalNum);
void erase_char();
void back_char();

void* memset(void* s, int32_t c, uint32_t n);
void* memset_word(void* s, int32_t c, uint32_t n);
void* memset_dword(void* s, int32_t c, uint32_t n);
void* memcpy(void* dest, const void* src, uint32_t n);
void* memmove(void* dest, const void* src, uint32_t n);
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n);
int8_t* strcpy(int8_t* dest, const int8_t*src);
int8_t* strncpy(int8_t* dest, const int8_t*src, uint32_t n);

void test_interrupts();

/* Userspace address-check functions */
int32_t bad_userspace_addr(const void* addr, int32_t len);
int32_t safe_strncpy(int8_t* dest, const int8_t* src, int32_t n);

/* Port read functions */
/* Inb reads a byte and returns its value as a zero-extended 32-bit
 * unsigned int */
static inline uint32_t inb(port)
{
	uint32_t val;
	asm volatile("xorl %0, %0\n \
			inb   (%w1), %b0" 
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
} 

/* Reads two bytes from two consecutive ports, starting at "port",
 * concatenates them little-endian style, and returns them zero-extended
 * */
static inline uint32_t inw(port)
{
	uint32_t val;
	asm volatile("xorl %0, %0\n   \
			inw   (%w1), %w0"
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
}

/* Reads four bytes from four consecutive ports, starting at "port",
 * concatenates them little-endian style, and returns them */
static inline uint32_t inl(port)
{
	uint32_t val;
	asm volatile("inl   (%w1), %0"
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
}

/* Writes a byte to a port */
#define outb(data, port)                \
do {                                    \
	asm volatile("outb  %b1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Writes two bytes to two consecutive ports */
#define outw(data, port)                \
do {                                    \
	asm volatile("outw  %w1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Writes four bytes to four consecutive ports */
#define outl(data, port)                \
do {                                    \
	asm volatile("outl  %l1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Clear interrupt flag - disables interrupts on this processor */
#define cli()                           \
do {                                    \
	asm volatile("cli"                  \
			:                       \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Save flags and then clear interrupt flag
 * Saves the EFLAGS register into the variable "flags", and then
 * disables interrupts on this processor */
#define cli_and_save(flags)             \
do {                                    \
	asm volatile("pushfl        \n      \
			popl %0         \n      \
			cli"                    \
			: "=r"(flags)           \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Set interrupt flag - enable interrupts on this processor */
#define sti()                           \
do {                                    \
	asm volatile("sti"                  \
			:                       \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Restore flags
 * Puts the value in "flags" into the EFLAGS register.  Most often used
 * after a cli_and_save_flags(flags) */
#define restore_flags(flags)            \
do {                                    \
	asm volatile("pushl %0      \n      \
			popfl"                  \
			:                       \
			: "r"(flags)            \
			: "memory", "cc"        \
			);                      \
} while(0)

#endif /* _LIB_H */
