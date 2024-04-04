/*!	\file kernel/elf32.c
 *	\brief ELF32 file format loader.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-08-19 Andrea Righi:
 *			Zero the .bss section (uninitialized data).\n
 *		2003-01-24 Andrea Righi:
 *			Skip NULL sections when copying them to
 *			memory.\n
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <const.h>
#include <errno.h>
#include <string.h>

#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/fat.h>
#include <kernel/kernel_map.h>
#include <kernel/keyboard.h>

#include <kernel/elf32.h>

//! \brief
//!	Check if a file has a valid elf32 header.
//! \param f The file header.
//! \return
//!	The entry point of the file or NULL if the file is not a
//!	valid ELF32.
size_t elf32_check( elf32_hdr_t *f )
{
#ifdef ELF32_DEBUG
	kprintf("\n\rFile header:");
	kprintf("\n\rmagic[]=%c%c%c%c class=%u data=%u",
		f->e_ident[EI_MAG0],
		f->e_ident[EI_MAG1],
		f->e_ident[EI_MAG2],
		f->e_ident[EI_MAG3],
		f->e_ident[EI_CLASS],
		f->e_ident[EI_DATA]
	);
#endif
	// Magic number check						//
	if (
		(f->e_ident[EI_MAG0] != ELF_MAG0) ||
		(f->e_ident[EI_MAG1] != ELF_MAG1) ||
		(f->e_ident[EI_MAG2] != ELF_MAG2) ||
		(f->e_ident[EI_MAG3] != ELF_MAG3)
	)
		return( NULL );

	// Class check							//
	if ( f->e_ident[EI_CLASS] != ELF_CLASS32 )
		return( NULL );

	// Data encoding check						//
	if ( f->e_ident[EI_DATA] != ELF_DATA2LSB )
		return( NULL );

	// Check OK!							//
	return( f->e_entry );
}

//! Get the name of a section.
char *get_section_name_by_index(elf32_hdr_t *elf32_header, unsigned long index) {
	elf32_shdr_t *elf32_sheader = (elf32_shdr_t *)
		((unsigned long)elf32_header + (unsigned long)elf32_header->e_shoff);
	char *name;

	elf32_sheader += (elf32_header->e_shstrndx);
	name = (char *)((unsigned long)elf32_header +
		(unsigned long)elf32_sheader->sh_offset + index);
	return name;
}

//! Copy a section into memory (in the current address space).
void copy_section(elf32_hdr_t *f, elf32_shdr_t *sec) {
#ifdef ELF32_DEBUG
	kprintf("section=%s\n", get_section_name_by_index(f, sec->sh_name));
#endif
	// Skip NULL sections.
	if ((sec->sh_addr==NULL) || (sec->sh_offset==NULL))
		return;

	// Note: copy this section into the current address space.
	if (strcmp(get_section_name_by_index(f, sec->sh_name), ".bss") == 0) {
		// Zero the .bss section (uninitialized data).
		memset((void *)(sec->sh_addr), 0, sec->sh_size);
	} else {
		// Copy the content of the section in memory.
		memcpy((void *)(sec->sh_addr),
			(void *)((uint32_t)f + sec->sh_offset),
			sec->sh_size );
	}
}

//! \brief
//!	Copy the ELF32 sections at the right locations in memory.
//! \param f The buffer where the ELF32 file is located.
//! \return
//!	\li the entry point address on success;
//!	\li NULL if an error occurs.
uint32_t elf32_copy_sections(elf32_hdr_t *f)
{
	elf32_shdr_t *sec;
	int	sec_num;
	int	sec_entsize;
	int	i;

	// Check if it is a valid ELF32 file				//
	if ( elf32_check(f) == NULL )
		return( NULL );

	// Get the section informations					//
	sec_num = 	f->e_shnum;
	sec_entsize =	f->e_shentsize;
#ifdef ELF32_DEBUG
	kprintf("\n\rEntry point @ %#010x", f->e_entry);
	kprintf("\n\rsec_num=%#010x, sec_entsize=%#010x\n\r", sec_num, sec_entsize);
#endif

	// First of all check if the sections are not in the user space
	// boundaries.
	// ----- User Sections		(4KB - 1GB)
	// ----- User Heap		(1GB - 2GB)
	// ----- User Stack		(2GB - 3GB)
	sec = (elf32_shdr_t *)((uint32_t)f->e_shoff + (uint32_t)f);
	for (i=0; i<sec_num; i++)
	{
		if ( (sec->sh_addr >= K_VIR_START) || (sec->sh_addr + sec->sh_size) >= K_VIR_START )
			return( NULL );
		(uint32_t)sec += sec_entsize;
	}

	// Copy the sections in memory					//
	sec = (elf32_shdr_t *)((uint32_t)f->e_shoff + (uint32_t)f);
	for (i=0; i<sec_num; i++)
	{
		copy_section( f, sec );
		(uint32_t)sec += sec_entsize;
	}
	return( f->e_entry );
}

//! \brief
//!	Get the entry point of a valid elf32 file loaded in memory.
size_t elf32_get_entry_point( uint8_t *file_buffer )
{
	size_t ret;

	// Check if it is a valid ELF file.
	ret = elf32_check( (elf32_hdr_t *)file_buffer );
	if ( ret == NULL )
		return( ret );

	// Return the entry point.
	return( ret );
}

//! \brief
//!	Load an ELF32 file into a buffer.
//! \param file_name The name of the file to be loaded.
//! \param file_buffer The buffer where the file will be loaded.
//! \return
//!	The entry point of the file or NULL if the file is not
//!	a valid elf32 file.
size_t elf32_load_file( uint8_t *file_buffer, int size, int dir, char *file_name )
{
	size_t ret;

	// Load the file into the buffer.
	if ( fat12_load_file(file_buffer, size, dir, file_name) < 0 )
		return( NULL );

	// Check if it is a valid ELF file.
	ret = elf32_get_entry_point( file_buffer );

	// The ELF32 file is successfully loaded.
	return( ret );
}
