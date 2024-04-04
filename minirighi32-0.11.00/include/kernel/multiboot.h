/*!	\file include/kernel/multiboot.h
 *	\brief Multiboot information (required from GRUB).
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-09
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef MULTIBOOT_H
#define MULTIBOOT_H

//! The symbol table for a.out format.
typedef struct {
	unsigned long tab_size;
	unsigned long str_size;
	unsigned long address;
	unsigned long reserved;
}
aout_symbol_table_t;

//! The section header table for ELF format.
typedef struct {
	unsigned long num;
	unsigned long size;
	unsigned long address;
	unsigned long shndx;
}
elf_section_header_table_t;

//! The multiboot informations.
typedef struct multiboot_info
{
	unsigned long flags;
	unsigned long mem_lower;
	unsigned long mem_upper;
	unsigned long boot_device;
	unsigned long cmdline;
	unsigned long mods_addr;
	union
	{
		aout_symbol_table_t aout_sym_t;
		elf_section_header_table_t elf_sec_t;
	} u;
	unsigned long mmap_length;
	unsigned long mmap_addr;
} multiboot_info_t;

#endif
