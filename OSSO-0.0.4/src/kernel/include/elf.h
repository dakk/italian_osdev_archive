/*
 * OSSO - A Micro Kernel OS
 * Copyright (c) 2000 Alessandro Iurlano.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */


/******************************* O S S O ***********************************
 * file : include/elf.h
 * Data structures and defines for the elf fileformat (to be tested)
 ***************************************************************************/

/***************************************************************************/
/*
 * $Id: elf.h,v 1.3 2001/05/01 01:04:19 iurlano Exp $
 */
/***************************************************************************/
/*
 * $Log: elf.h,v $
 * Revision 1.3  2001/05/01 01:04:19  iurlano
 * Work on Dynamic Linker problem
 * Seems like that the shared MPS_Pid is working now.
 * I thing that the condivision of a variable from the lib and the program works if the variable is declared in the program.
 * for the other case, I've to work on it and I don't think I'm going to do it very soon.
 *
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.5  2000/11/01 23:23:51  iurlano
 * Work on dynamic linking
 *
 * Revision 1.4  2000/10/30 23:02:28  iurlano
 * Working on ELF dynamic linking
 *
 * Revision 1.3  2000/10/25 19:59:39  iurlano
 * Added automatic module name printing in printf and printk
 * Added little ELF check in process.c
 *
 * Revision 1.2  2000/08/20 16:46:06  iurlano
 * Fixed bug in init in modules loading
 * IDE driver test. Doesn't work with my Quantum Fireball 4.3 Gb but does
 *   work with my Conner 42MB
 * More translation
 * Fixed some bad comments
 * Changed paging.c for a test (see the comments there)
 *
 * Revision 1.1.1.1  2000/05/30 08:11:05  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/


#ifndef __ELF_DEFS
#define __ELF_DEFS

#define EI_NIDENT 16

typedef 
struct __ELF_Header {
  byte e_ident[EI_NIDENT];
  word e_type;
  word e_machine;
  dword e_version;
  dword e_entry; /* entry point */
  dword e_phoff; /* Program Header Offset */
  dword e_shoff; /* Section Header Offset */
  dword e_flags;
  word e_ehsize; /* Header size */
  word e_phentsize; /* Program Header Entry Size */
  word e_phnum; /* Program Header Entry Number */
  word e_shentsize; /* Section Header Entry Size */
  word e_shnum;/* Section Header Entry Number */
  word e_shstindex; /* Section name string index */
} ELF_Header __attribute__ ((packed)); 

/* Consts for ELF e_type */

#define ET_NONE    0
#define ET_REL     1
#define ET_EXEC    2
#define ET_DYN     3
#define ET_CORE    4
#define ET_LOPROC  0xff00
#define ET_HIPROC  0xffff

/* Consts for MACHINE e_machine */

#define EM_NONE    0
#define EM_M32     1
#define EM_SPARC   2
#define EM_386     3
#define EM_68K     4
#define EM_88K     5
#define EM_860     7
#define EM_MIPS    8


/* Consts for VERSION e_version */

#define EV_NONE      0
#define EV_CURRENT   1

/* Consts for ELFCLASS */

#define ELFCLASSNONE      0
#define ELFCLASS32        1
#define ELFCLASS64        2


typedef 
struct __ELF_Section_Header {
  dword sh_name;       /* Name Index */
  dword sh_type;       /* Section Type */
  dword sh_flags;      /* Attributes */
  dword sh_addr;       /* Address */
  dword sh_offset;     /* Offset */
  dword sh_size;       /* Section Size */
  dword sh_link;       /* Link Index */
  dword sh_info;       /* Extra Info */
  dword sh_addraling;  /* Address Alignment */
  dword sh_entsize;    /* Size of Fixed-size entry */
} ELF_Section_Header __attribute__ ((packed)); 


/* Consts for SECTION TYPES sh_type */

#define SHT_NULL         0          /* INACTIVE */
#define SHT_PROGBITS     1          /* Program Defined */
#define SHT_SYMTAB       2          /* Symbol Table */
#define SHT_STRTAB       3          /* String Table */
#define SHT_RELA         4          /* Relocation Entries with addends */
#define SHT_HASH         5          /* Symbol Hash Table */
#define SHT_DYNAMIC      6          /* Info for Dynamic linking */
#define SHT_NOTE         7          /* Note Section */
#define SHT_NOBITS       8          /* No File Space Section */
#define SHT_REL          9          /* Relocation entries with no addends */
#define SHT_SHLIB       10          /* Reserved */
#define SHT_DYNSYM      11          /* Symbol Table */
#define SHT_LOPROC      0x70000000  /* Processor Specific */
#define SHT_HIPROC      0x7FFFFFFF
#define SHT_LOUSER      0x80000000  /* User Defined */
#define SHT_HIUSER      0xFFFFFFFF

/* Consts for SECTION ATTRIBUTES sh_flags */

#define SHF_WRITE            0x1          /* Writable */
#define SHF_ALLOC            0x2          /* Must Be Allocated */
#define SHF_EXECINTSR        0x4          /* Executable */
#define SHF_MASKPROC  0xf0000000          /* Processor Specific */

typedef 
struct __ELF_Symbol_Table_entry {
  dword st_name;         /* Name Index */
  dword st_value;        /* Value */
  dword st_size;         /* Size */
  byte st_info;          /* Symbol Type */
  byte st_other;         /* Undefined */
  word st_shndex;        /* Relative Section */
} ELF_Symbol_Table_entry __attribute__ ((packed)); 

#define SHN_UNDEF 0

#define ELF32_ST_BIND(i) ((i)>>4)
#define ELF32_ST_TYPE(i) ((i)&0xF)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xF))


typedef 
struct __ELF_Relocation_entry {
  dword r_offset;      /* Offset */
  dword r_info;        /* Type and Section */
} ELF_Relocation_entry __attribute__ ((packed)); 

typedef 
struct __ELF_Relocation_Addendum_entry {
  dword r_offset;      /* Offset */
  dword r_info;        /* Type and Section */
  signed long int r_addend; /* Addendum */
} ELF_Relocation_Addendum_entry __attribute__ ((packed)); 

#define R_386_NONE     0
#define R_386_32       1
#define R_386_PC32     2
#define R_386_GOT32    3
#define R_386_PLT32    4
#define R_386_COPY     5
#define R_386_GLOB_DAT 6
#define R_386_JMP_SLOT 7
#define R_386_RELATIVE 8
#define R_386_GOTOFF   9
#define R_386_GOTPC    10

typedef 
struct __ELF_Program_Header {
  dword p_type;        /* Kind of the segment */
  dword p_offset;      /* Offset from start of file */
  dword p_vaddr;       /* Virtual Address of start */
  dword p_vpaddr;      /* Physical Address of start */
  dword p_filesz;      /* File Image size of segment */
  dword p_memsz;       /* Memory Image size of segment */
  dword p_flags;       /* Attributes */
  dword p_align;       /* Alignment */
} ELF_Program_Header __attribute__ ((packed)); 

/* Consts for Program Header p_type */

#define PT_NULL         0          /* UNUSED */
#define PT_LOAD         1          /* Loadable Segment */
#define PT_DYNAMIC      2          /* Dynamic linking */
#define PT_INTERP       3          /* Interpreter Name */
#define PT_NOTE         4          /* Note Section */
#define PT_SHLIB        5          /* Reserved */
#define PT_PHDR         6          /* Location And Size of Program Header */

/*
 addr=word=off = unsigned long int (4 byte)
 sword= signed long int (4 byte)
 half = unsigned short int (2 byte)
*/

#define DT_NULL         0
#define DT_NEEDED       1
#define DT_PLTRELSZ     2
#define DT_PLTGOT       3
#define DT_HASH         4
#define DT_STRTAB       5
#define DT_SYMTAB       6
#define DT_RELA         7
#define DT_RELASZ       8
#define DT_RELAENT      9
#define DT_STRSZ        10
#define DT_SYMENT       11
#define DT_INIT         12
#define DT_FINI         13
#define DT_SONAME       14
#define DT_RPATH        15
#define DT_SYMBOLIC     16
#define DT_REL          17
#define DT_RELSZ        18
#define DT_RELENT       19
#define DT_PLTREL       20
#define DT_DEBUG        21
#define DT_TEXTREL      22
#define DT_JMPREL       23
#define DT_LOPROC       0x70000000
#define DT_HIPROC       0x7FFFFFFF

typedef 
struct __ELF_Dynamic_Entry {
  signed long int d_tag;
  union {
    dword d_val;
    dword d_ptr;
  } d_un;
} ELF_Dynamic_Entry;

#endif




