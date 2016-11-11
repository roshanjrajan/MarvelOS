#ifndef SUPER_H
#define SUPER_H

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

extern file_descriptor_entry_t * fdt;

#endif

