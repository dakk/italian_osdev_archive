/*!	\file include/kernel/elf32.h
 *	\brief ELF32 file format header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-10-01
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef ELF32_H
#define ELF32_H

/** \ingroup Kernel
 *  \defgroup KElf32 ELF32 loader
 *  The loader of the ELF32 format files.
 *  @{
 */

// --- 32-bit ELF address types ----------------------------------------//

typedef uint32_t	Elf32_Addr;
typedef uint16_t	Elf32_Half;
typedef uint32_t	Elf32_Off;
typedef int32_t		Elf32_Sword;
typedef uint32_t	Elf32_Word;

// -- ELF32 header -----------------------------------------------------//

#define EI_NIDENT	16	//!< Size of e_ident[].

//! 32-bit ELF header.
typedef struct elf32_hdr
{
	unsigned char	e_ident[EI_NIDENT];
	Elf32_Half	e_type;
	Elf32_Half	e_machine;
	Elf32_Word	e_version;
	Elf32_Addr	e_entry;
	Elf32_Off	e_phoff;
	Elf32_Off	e_shoff;
	Elf32_Word	e_flags;
	Elf32_Half	e_ehsize;
	Elf32_Half	e_phentsize;
	Elf32_Half	phnum;
	Elf32_Half	e_shentsize;
	Elf32_Half	e_shnum;
	Elf32_Half	e_shstrndx;
} elf32_hdr_t;

#define EI_MAG0		0	//!< File magic number (0).
#define EI_MAG1		1	//!< File magic number (1).
#define EI_MAG2		2	//!< File magic number (2).
#define EI_MAG3		3	//!< File magic number (3)
#define EI_CLASS	4	//!< File class.
#define EI_DATA		5	//!< Data encoding.
#define EI_VERSION	6	//!< File version.
#define EI_PAD		7	//!< Start of padding bytes.

#define ELF_MAG0	0x7F	//!<< ELF magic number.
#define ELF_MAG1	'E'	//!<< ELF magic number[0].
#define ELF_MAG2	'L'	//!< ELF magic number[1].
#define ELF_MAG3	'F'	//!< ELF magic number[2].

// ELF class or capacity						//
#define ELF_CLASSNONE	0	//!< Invalid class.
#define ELF_CLASS32	1	//!< 32-bit objects.
#define ELF_CLASS64	2	//!< 64-bit objects.

// ELF data encoding							//
#define ELF_DATANONE	0	//!< Invalid data encoding.
#define ELF_DATA2LSB	1	//!< Least significant byte encoding.
#define ELF_DATA2MSB	2	//!< Most significant byte encoding.

// --- ELF32 Sections --------------------------------------------------//

//! ELF32 Section header.
typedef struct elf32_shdr
{
	Elf32_Word	sh_name;
	Elf32_Word	sh_type;
	Elf32_Word	sh_flags;
	Elf32_Addr	sh_addr;
	Elf32_Off	sh_offset;
	Elf32_Word	sh_size;
	Elf32_Word	sh_link;
	Elf32_Word	sh_info;
	Elf32_Word	sh_addralign;
	Elf32_Word	sh_entsize;
} elf32_shdr_t;

// Section types							//

//! The section is inactive.
#define SHT_NULL	0
//! The section holds informations defined by the program, whose format
//! and meaning are determined solely by the program.
#define SHT_PROGBITS	1
//! The section holds a symbol table.
#define SHT_SYMTAB	2
//! The section holds a string table.
#define SHT_STRTAB	3
//! The section holds relocation entries.
#define SHT_RELA	4
//! The section holds a symbol hash table.
#define SHT_HASH	5
//! The section holds informations for dynamic linking.
#define SHT_DYNAMIC	6
//! The section holds informations that marks the file in some way.
#define SHT_NOTE	7
//! A section of this type occupies no space in the file but otherwise
//! resembles SHT_PROGBITS. Although this section contains no bytes, the
//! \c sh_offset member contains the conceptual file offset.
//! For example the ".bss" section can be considered a section of this
//! type.
#define SHT_NOBITS	8
//! The section holds a relocation entries without explicit addends.
#define SHT_REL		9
//! This section type is reserved, but has unspecified semantics.
#define SHT_SHLIB	10
//! This section contains a set of dynamic linking symbols.
#define SHT_DYNSYM	11
//! Values in this exclusive range are reserved for processor-specific
//! semantics.
#define SHT_LOPROC	0x700000000
//! Values in this exclusive range are reserved for processor-specific
//! semantics.
#define SHT_HIPROC	0x700000000
//! This value specifies the lower bound of the range of indexes
//! reserved for application programs.
#define SHT_LOUSER	0x700000000
//! This value specifies the upper bound of the range of indexes
//! reserved for application programs.
#define SHT_HIUSER	0x700000000

// Section attribute flags						//

//! Data should be writable during the process execution.
#define SHF_WRITE	0x1
//! The section occupies memory during the process execution.
#define SHF_ALLOC	0x2
//! The section contains executable machine instructions.
#define SHF_EXECINSTR	0x4
//! All bits included in this mask are reserved for processor-specific
//! semantics.
#define SHF_MASKPROC	0xF00000000

// --- Prototypes ------------------------------------------------------//

uint32_t elf32_copy_sections( elf32_hdr_t *f );
size_t elf32_get_entry_point( uint8_t *file_buffer );
size_t elf32_load_file( uint8_t *file_buffer, int size, int dir, char *file_name );

/** @} */ // end of KElf32

#endif
