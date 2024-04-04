/* multiboot.h -- the Multiboot Header */
/******************************************************************************
* Exabyte Operating System                                                    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; either version 2              *
* of the License, or (at your option) any later version.                      *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License           *
* along with this program; if not, write to the Free Software                 *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
*******************************************************************************/

#ifndef IA32_MULTIBOOT_H
#define IA32_MULTIBOOT_H

/* numero magico per l'header multiboot */
#define MULTIBOOT_HEADER_MAGIC          0x1BADB002

/* flag per l'header multiboot (pre-boot) */
#define MULTIBOOT_FLAGS_MODS_ALIGNED	1	// bit 0 -- moduli page-aligned
#define MULTIBOOT_FLAGS_MEMORY_MAP	2	// bit 1 -- memory map
#define MULTIBOOT_FLAGS_VIDEO_MODES	4	// bit 2 -- video modes
#define MULTIBOOT_FLAGS_ADDRESS		0x10000	// bit 15 -- indirizzi dell'immagine

/* flag per l'header multiboot (post-boot) */
#define MULTIBOOT_FLAGS_MEMINFO		1	// bit 0 -- informazioni sulla memoria
#define MULTIBOOT_FLAGS_BOOTDRIVE	2	// bit 1 -- boot drive
#define MULTIBOOT_FLAGS_CMDLINE		4	// bit 2 -- command line
#define MULTIBOOT_FLAGS_MODULES		8	// bit 3 -- moduli
#define MULTIBOOT_FLAGS_SYMS1		16	// bit 4 -- simboli/a.out
#define MULTIBOOT_FLAGS_SYMS2		32	// bit 5 -- simboli/elf
#define MULTIBOOT_FLAGS_MMAP		64	// bit 6 -- memory map
#define MULTIBOOT_FLAGS_DRIVES		128	// bit 7 -- informazioni sui drive
#define MULTIBOOT_FLAGS_CONFIGTABLE	256	// bit 8 -- configuration table (??)
#define MULTIBOOT_FLAGS_BOOTLDRNAME	512	// bit 9 -- nome del boot loader
#define MULTIBOOT_FLAGS_APMTABLE	1024	// bit 10 -- tabella apm
#define MULTIBOOT_FLAGS_VBE		2048	// bit 11 -- informazioni sulla grafica

#define MULTIBOOT_LAST_FLAG		MULTIBOOT_FLAGS_VBE	// ultimo flag per il conteggio

#ifdef __ELF__
# define MULTIBOOT_HEADER_FLAGS         (MULTIBOOT_FLAGS_MODS_ALIGNED | MULTIBOOT_FLAGS_MEMORY_MAP)
#else
# define MULTIBOOT_HEADER_FLAGS         (MULTIBOOT_FLAGS_MODS_ALIGNED|MULTIBOOT_FLAGS_MEMORY_MAP|MULTIBOOT_FLAGS_ADDRESS)
#endif

/* numero magico passato da un bootloader multiboot */
#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002

#ifndef ASM
/* da qui in poi non includere in boot.S */

/* The Multiboot header. */
typedef struct
{
	unsigned long magic;
	unsigned long flags;
	unsigned long checksum;
	unsigned long header_addr;
	unsigned long load_addr;
	unsigned long load_end_addr;
	unsigned long bss_end_addr;
	unsigned long entry_addr;
	unsigned long mode_type;
	unsigned long width;
	unsigned long height;
	unsigned long depth;
} multiboot_header;

/* The symbol table for a.out. */
typedef struct
{
	unsigned long tabsize;
	unsigned long strsize;
	unsigned long addr;
	unsigned long reserved;
} aout_symbol_table;

/* The section header table for ELF. */
typedef struct elf_section_header_table
{
  unsigned long num;
  unsigned long size;
  unsigned long addr;
  unsigned long shndx;
} elf_section_header_table;

/* boot_device structure */
typedef struct
{
	unsigned char part3;
	unsigned char part2;
	unsigned char part1;
	unsigned char drive;
} drive_device;

/* The module structure. */
typedef struct
{
	unsigned long mod_start;
	unsigned long mod_end;
	unsigned long string;
	unsigned long reserved;
} multiboot_module;

/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size. */
typedef struct
{
	unsigned long size;		// fottuto -4!!!
	unsigned long base_addr_low;
	unsigned long base_addr_high;
	unsigned long length_low;
	unsigned long length_high;
	unsigned long type;
} memory_map;

/* drives structure */
typedef struct
{
	unsigned long size;
	unsigned char drive_number;
	unsigned char drive_mode;
	unsigned short drive_cylinders;
	unsigned char drive_heads;
	unsigned char drive_sectors;
	unsigned short * drive_ports;
} drives_t;

/* apm_addr structure */
typedef struct
{
	unsigned short version;
	unsigned short cseg;
	unsigned long offset;
	unsigned short cseg_16;
	unsigned short dseg;
	unsigned short flags;
	unsigned short cseg_len;
	unsigned short cseg_16_len;
	unsigned short dseg_len;
} apm_table;

/* The Multiboot information */
typedef struct
{
	unsigned long flags;
	unsigned long mem_lower;
	unsigned long mem_upper;
	drive_device boot_device;
	unsigned char * cmdline;
	unsigned long mods_count;
	unsigned long mods_addr;
	union
	{
		aout_symbol_table aout_sym;
		elf_section_header_table elf_sec;
	} u;
	unsigned long mmap_length;	// lunghezza di tutte le struct
	memory_map * mmap_addr;
/* TODO
	unsigned long drives_length;	// lunghezza di tutte le struct
	drives_t * drives_addr;
	unsigned long config_table;
	unsigned char * bootloader_name;
	apm_table * apm_addr;
	unsigned long vbe_control_info;
	unsigned long vbe_mode_info;
	unsigned short vbe_mode;
	unsigned short vbe_interface_seg;
	unsigned short vbe_interface_off;
	unsigned short vbe_interface_len;
   TODO */
} multiboot_info;

int multiboot_check(unsigned long magic,multiboot_info * info);
int prepare_modules(multiboot_module * pre_boot,unsigned long count);
int copy_modules(multiboot_module * post_boot,unsigned long count);
int multiboot_module_count(void);
int multiboot_get_module(unsigned int index,multiboot_module * mod_desc);

extern multiboot_info boot_info; // copia delle info di boot

#endif /* ! ASM */
#endif /* ! __MULTIBOOT_H */
