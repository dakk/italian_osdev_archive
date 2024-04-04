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


/*************************** LIB O S S O ***********************************
 * file : lib/ld.c                                                         *
 * Dynamic Linker                                                          *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: ld.c,v 1.16 2001/05/14 15:45:57 iurlano Exp $
 ***************************************************************************
 *
 * $Log: ld.c,v $
 * Revision 1.16  2001/05/14 15:45:57  iurlano
 * Work on SMP
 *
 * Revision 1.15  2001/05/02 21:53:53  iurlano
 * Added Backspace feature to the shell
 *
 * Revision 1.14  2001/05/01 01:04:19  iurlano
 * Work on Dynamic Linker problem
 * Seems like that the shared MPS_Pid is working now.
 * I thing that the condivision of a variable from the lib and the program works if the variable is declared in the program.
 * for the other case, I've to work on it and I don't think I'm going to do it very soon.
 *
 * Revision 1.13  2001/04/22 13:17:38  iurlano
 * Changed mpstest
 * mps now working
 * integrated some code for pid asking in the libosso
 *
 * Revision 1.12  2001/04/03 21:34:44  iurlano
 * Dynamic semaphores work in progress
 * Little changes in make xconfig
 * Added and changed in the code the value of THREAD_INVALID from 0 to -1
 *
 * Revision 1.11  2001/03/31 15:07:50  iurlano
 * bugfixes
 *
 * Revision 1.10  2001/03/23 21:05:49  iurlano
 *
 * I've finally found the bug! it is in process.c in init_process_structures
 * the problem is the for that initializes the mutexes of the processes
 * I can't figure why it is wrong, yet.
 *
 * Revision 1.9  2001/03/16 19:22:59  iurlano
 * Added something to the make xconfig prog
 * Now OSSO runs on plex86
 *
 * BIG PROBLEM. OSSO gives random errors about finding INIT and relocating
 *   and doesn't start
 *
 * Revision 1.8  2000/12/08 16:45:27  iurlano
 * Thread Work in progress.
 * First working version reached. Needs Debug.
 *
 * Revision 1.7  2000/11/02 23:35:38  iurlano
 * Dynamic linking seems to work!!
 * Some problems at boot stage with files with extension
 *
 * Revision 1.6  2000/11/02 15:11:13  iurlano
 * Work on dynamic linker
 *
 * Revision 1.5  2000/11/02 11:59:37  iurlano
 * ELF dynamic linking work in progress
 *
 * Revision 1.4  2000/11/01 23:23:52  iurlano
 * Work on dynamic linking
 *
 * Revision 1.3  2000/10/30 23:02:29  iurlano
 * Working on ELF dynamic linking
 *
 * Revision 1.2  2000/10/30 14:25:00  iurlano
 * First dynamically linked program working
 *
 * Revision 1.1  2000/10/29 23:47:14  iurlano
 * Added Dynamic Linker
 *
 *
 ***************************************************************************/

#include <printf.h>
#include <syscalls.h>
#include <types.h>
#include <elf.h>
#include <string.h>


//#define LD_DEBUG // Uncomment this to gain debug printfs


typedef
struct __sym_table {
  ELF_Symbol_Table_entry *sym;
  char *strtable;
  dword strsize;
  dword base_address;
} sym_table;

sym_table Tables[10]; /* To decide how to do it */
int sym_num=0;

void Relocate_Program(dword base_address);
void _start(void);
dword Symbol_Lookup(char * Symbol_Name);
/* LD is a shared object, not a program so it will have to relocate
   himself without help */
 

/* LD entry point */
void _dt_Linker(void) {
  dword base_address,program_base_address;
  dword got_address;
  int k,jmprelidx,lth;
  ELF_Dynamic_Entry *dyn;
  ELF_Relocation_entry *Rel;
  ELF_Relocation_entry *Got;
  ELF_Symbol_Table_entry *sym;
  //  ELF_Header *LD_Header;
  //  ELF_Program_Header * LD_PHeader;
  dword Relsz,Relent;
  dword strsize;
  char * strtable;

  /* Get base address and got address from the kernel */
  __asm__ ("":"=a" (base_address),"=c" (got_address),"=d" (program_base_address):);
  /* Debug */
#ifdef LD_DEBUG
  __asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('X'));
#endif
  /* Get address of dynamic section */
  dyn=(ELF_Dynamic_Entry*)got_address;
#ifdef LD_DEBUG
  __asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('2'));
#endif
  lth=0;
  sym=NULL;
  Rel=NULL;

  // Dynamic Section printing
  /*
    k=0;
    while (dyn[k].d_tag!=DT_NULL) {
    int t;
    t=dyn[k].d_tag;
    while (t>0) {
    __asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('0'+t%10));
    t/=10;
    }
    __asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('\n'));
    k++;
    }
  */

  /* Harvest data in the dynamic section */
  k=0;
  while (dyn[k].d_tag!=DT_NULL) {
    //__asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('C'));
    //    __asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" (dyn[k].d_tag+'0'));
    switch (dyn[k].d_tag) {
    case DT_PLTRELSZ:
      lth=dyn[k].d_un.d_val;
      break;
    case DT_SYMTAB:
      sym=(ELF_Symbol_Table_entry*)(dyn[k].d_un.d_ptr+base_address);
      break;
    case DT_REL:
      //__asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('P'));
      Rel=(ELF_Relocation_entry*)(dyn[k].d_un.d_ptr+base_address);
      break;
    case DT_STRTAB:
      strtable=(char *)(dyn[k].d_un.d_ptr+base_address);
      break;
    case DT_STRSZ:
      strsize=(dyn[k].d_un.d_val);
      break;
    case DT_RELSZ:
      Relsz=dyn[k].d_un.d_val;
      break;
    case DT_RELENT:
      Relent=dyn[k].d_un.d_val;
      break;
    case DT_PLTGOT:
      Got=( ELF_Relocation_entry *)dyn[k].d_un.d_val;
      break;
    case DT_JMPREL:
      jmprelidx=k;
      break;
    }
    k++;
  }
  /* Debug */
#ifdef LD_DEBUG
  __asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('Y'));
#endif

  /* JMPREL found. Functions to relocate */
  if (lth!=0) {

    ELF_Relocation_entry *rels;

#ifdef LD_DEBUG
    __asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('J'));
#endif

    rels=(ELF_Relocation_entry*)(dyn[jmprelidx].d_un.d_ptr+base_address);
    /* Process Relocations */
    for (k=0;k<(lth/sizeof(rels[0]));k++) {
#ifdef LD_DEBUG
      __asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('r'));
#endif
      switch (rels[k].r_info&0xff) {
      case R_386_NONE:
      case R_386_COPY:
	/* Nothing to do (to check ???) */
	break;
      case R_386_32:
	/* S + A */
	*((dword*)(rels[k].r_offset+base_address))+=
	  base_address+sym[rels[k].r_info>>8].st_value;	
	break;
      case R_386_RELATIVE:
	/* B + A */
	*((dword*)(rels[k].r_offset+base_address))+=base_address;	
	break;
      case R_386_PC32:
	/* S + A - P */
	*((dword*)(rels[k].r_offset+base_address))+=
	  base_address+sym[rels[k].r_info>>8].st_value+rels[k].r_offset;
	break;
      case R_386_JMP_SLOT:
      case R_386_GLOB_DAT:
	/* S */
	/* If symbol is defined */
	if (sym[rels[k].r_info>>8].st_shndex!=0)
	  *((dword*)(rels[k].r_offset+base_address))=
	    base_address+sym[rels[k].r_info>>8].st_value;
	break;
      default:
	/* Unknown relocation type */
	__asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('#'));
	break;
      }
    }
  }
  
  /* Rel found. Relocate variables */
  if (Rel!=NULL) {
    /* Perform relocation, this is type R_386_GLOB_DAT */
    for (k=0;k<Relsz/Relent;k++) {
      /* Debug */
#ifdef LD_DEBUG
      __asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('R'));
#endif
      switch (Rel[k].r_info&0xff) {
      case R_386_NONE:
      case R_386_COPY:
	/* Nothing to do (to check ???) */
	break;
      case R_386_32:
	/* S + A */
	*((dword*)(Rel[k].r_offset+base_address))+=
	  base_address+sym[Rel[k].r_info>>8].st_value;	
	break;
      case R_386_RELATIVE:
	/* B + A */
	*((dword*)(Rel[k].r_offset+base_address))+=base_address;	
	break;
      case R_386_PC32:
	/* S + A - P */
	*((dword*)(Rel[k].r_offset+base_address))+=
	  base_address+sym[Rel[k].r_info>>8].st_value+Rel[k].r_offset;
	break;
      case R_386_JMP_SLOT:
      case R_386_GLOB_DAT:
	/* S */
	/* If symbol is defined */
	if (sym[Rel[k].r_info>>8].st_shndex!=0)
	  *((dword*)(Rel[k].r_offset+base_address))=
	    base_address+sym[Rel[k].r_info>>8].st_value;
	break;
      default:
	/* Unknown relocation type */
	__asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('W'));
	break;
      }
    }
  }
  /* Debug */
#ifdef LD_DEBUG
  __asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('Z'));
#endif
  /* End LD Relocation */
  /*
    printf("Got Printing ... \n");
    {
    int yy;
    yy=0;
    printf("Got addresses 1 %x, 2 %x, 3 %x\n",Got, ((byte*)Got)+base_address,got_address);
    Got=((byte*)Got)+base_address;
    do {
    printf("%d : Tag %x, val %x, offs %x\n",yy,Got[yy].r_info&0xFF,Got[yy].r_info>>0,Got[yy].r_offset);
    yy++;
    } while ((Got[yy-1].r_info&0xFF)!=0);
    }
  */
#ifdef LD_DEBUG
  printf("\nLinker Relocated and started\n");
#endif
  sym_num=2;
  Tables[0].sym=sym;
  Tables[0].base_address=base_address;
  Tables[0].strtable=strtable;
  Tables[0].strsize=strsize;

  Relocate_Program(program_base_address);

  /* 
     DEBUG prints
     if (lth!=0) {

     ELF_Relocation_entry *rels;

     printf("DEBUG\n");

     rels=(ELF_Relocation_entry*)(dyn[jmprelidx].d_un.d_ptr+base_address);
     for (k=0;k<(lth/sizeof(rels[0]));k++) {
     printf("Relocation entry : %d value %x ",k,sym[rels[k].r_info>>8].st_value);
     printf("  shndx : %x ;Name : %s (val %x)\n",sym[rels[k].r_info>>8].st_shndex,strtable+sym[rels[k].r_info>>8].st_name,Symbol_Lookup(strtable+sym[rels[k].r_info>>8].st_name));
     }
     }
  */


  /* Need another pass */
  if (lth!=0) {
    ELF_Relocation_entry *rels;
#ifdef LD_DEBUG
    printf("Relocating JMPREL (Linker)...\n");
#endif
    rels=(ELF_Relocation_entry*)(dyn[jmprelidx].d_un.d_ptr+base_address);

    /* Process Relocations, this is type 7 R_386_JUMP_SLOT */
    for (k=0;k<(lth/sizeof(rels[0]));k++) {
      switch (rels[k].r_info&0xff) {
      case R_386_NONE:
      case R_386_COPY:
	/* Nothing to do (to check ???) */
	break;
      case R_386_32:
	/* S + A */
	*((dword*)(rels[k].r_offset+base_address))+=
	  base_address+sym[rels[k].r_info>>8].st_value;	
	break;
      case R_386_RELATIVE:
	/* B + A */
	*((dword*)(rels[k].r_offset+base_address))+=base_address;	
	break;
      case R_386_PC32:
	/* S + A - P */
	*((dword*)(rels[k].r_offset+base_address))+=
	  base_address+sym[rels[k].r_info>>8].st_value+rels[k].r_offset;
	break;
      case R_386_JMP_SLOT:
      case R_386_GLOB_DAT:
	/* S */
	/* If symbol is defined */
	if (sym[rels[k].r_info>>8].st_shndex==0)
	  *((dword*)(rels[k].r_offset+base_address))=Symbol_Lookup(strtable+sym[rels[k].r_info>>8].st_name);
	break;
      default:
	/* Unknown relocation type */
	__asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('@'));
	break;
      }

    }
  }

#ifdef LD_DEBUG
  printf("Relocating JMPREL (Linker) End\n");
#endif
  
  /* Rel found. Relocate variables */
  if (Rel!=NULL) {
    /* Perform relocation, this is type R_386_GLOB_DAT */
    for (k=0;k<(Relsz/Relent);k++) {
      switch (Rel[k].r_info&0xff) {
      case R_386_NONE:
      case R_386_COPY:
	/* Nothing to do (to check ???) */
	break;
      case R_386_32:
	/* S + A */
	*((dword*)(Rel[k].r_offset+base_address))+=
	  base_address+sym[Rel[k].r_info>>8].st_value;	
	break;
      case R_386_RELATIVE:
	/* B + A */
	*((dword*)(Rel[k].r_offset+base_address))+=base_address;	
	break;
      case R_386_PC32:
	/* S + A - P */
	*((dword*)(Rel[k].r_offset+base_address))+=
	  base_address+sym[Rel[k].r_info>>8].st_value+Rel[k].r_offset;
	break;
      case R_386_JMP_SLOT:
      case R_386_GLOB_DAT:
	/* S */
	/* If symbol is defined */
	if (sym[Rel[k].r_info>>8].st_shndex==SHN_UNDEF)
	  *((dword*)(Rel[k].r_offset+base_address))=Symbol_Lookup(strtable+sym[Rel[k].r_info>>8].st_name);
	break;
      default:
	/* Unknown relocation type */
	__asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('!'));
	break;
      }

    }
  }  
  /* Finished Relocationg, launching program */
#ifdef LD_DEBUG
  printf("Dynamic Linker  work finished. Starting PRogram...\n");
#endif
  _start();
  while (1) ;
}

dword Symbol_Lookup(char * Symbol_Name) {
  int j,k,x;
  dword value;
  value=-1;
#ifdef LD_DEBUG
  printf("Symbol name = %s ",Symbol_Name);
#endif
  for (j=0;j<sym_num;j++) {
    for (k=1;k<Tables[j].strsize;k++) {
      /* Making sure that k is the start of a string, that is it
	 is preeceded by a \0
      */
      if ((*((Tables[j].strtable)+k-1)=='\0')&&(strncmp(Symbol_Name,(Tables[j].strtable)+k,Tables[j].strsize-k)==0)) {
	x=0;
#ifdef LD_DEBUG
	printf("Symbol found at %d:%d : %s\n",j,k,(Tables[j].strtable)+k);
#endif
	while (Tables[j].sym[x].st_name!=k) x++;
	if (Tables[j].sym[x].st_shndex!=0) {
	  value=Tables[j].sym[x].st_value+Tables[j].base_address;
#ifdef LD_DEBUG
	  printf("Value found %x\n",value);
#endif
	  return value;
	}

      }
    }
  }
  return value;
}

void Relocate_Program(dword base_address) {
  ELF_Header *Header;
  ELF_Program_Header *PHeader;
  ELF_Dynamic_Entry *pdyn;
  ELF_Relocation_entry *Rel;
  ELF_Symbol_Table_entry *sym;
  dword Relsz,Relent;
  dword strsize;
  dword k,jmprelidx,lth;
  char * strtable;

  Rel=NULL;
#ifdef LD_DEBUG
  printf("Relocating program at : %x\n",base_address);
#endif
  /* Finding ELF data */
  Header=(ELF_Header*)base_address;
  PHeader=(ELF_Program_Header*)(base_address+Header->e_phoff);
  /* Finding dynamic section */
  pdyn=NULL;
  for (k=0;k<(Header->e_phnum);k++) {
    if (PHeader[k].p_type==PT_DYNAMIC) {
      pdyn=(ELF_Dynamic_Entry*)PHeader[k].p_vaddr;
      break;
    }
  }
  if (pdyn==NULL) {
    printf("Error searching Dynamic section ! !\n");
    while (1) ;
  }
  /* Harvest data in the dynamic section */
  k=0;
  while (pdyn[k].d_tag!=DT_NULL) {
    switch (pdyn[k].d_tag) {
    case DT_JMPREL:
      jmprelidx=k;
      break;
    case DT_PLTRELSZ:
      lth=pdyn[k].d_un.d_val;
      break;
    case DT_SYMTAB:
      sym=(ELF_Symbol_Table_entry*)(pdyn[k].d_un.d_ptr);
      break;
    case DT_STRTAB:
      strtable=(char *)(pdyn[k].d_un.d_ptr);
      break;
    case DT_STRSZ:
      strsize=(pdyn[k].d_un.d_val);
      break;
    case DT_REL:
      Rel=(ELF_Relocation_entry*)(pdyn[k].d_un.d_ptr);
      break;
    case DT_RELSZ:
      Relsz=pdyn[k].d_un.d_val;
      break;
    case DT_RELENT:
      Relent=pdyn[k].d_un.d_val;
      break;
    }
    k++;
  }

  Tables[1].sym=sym;
  Tables[1].base_address=0;
  Tables[1].strtable=strtable;
  Tables[1].strsize=strsize;

  k=0;
  while (pdyn[k].d_tag!=DT_NULL) {
    if (pdyn[k].d_tag==DT_NEEDED) {
      //      printf("Library needed : %s\n",strtable+pdyn[k].d_un.d_val);
    }
    k++;
  }

  //  printf("Checking for JmpRel lth==%d...\n",lth);
  //  printf("Address : %x\n",pdyn[jmprelidx].d_un.d_ptr);
  if (lth!=0) {
    ELF_Relocation_entry *rels;
    //    printf("Relocating JMPREL(Program)...\n");

    rels=(ELF_Relocation_entry*)(pdyn[jmprelidx].d_un.d_ptr);

    /* Process Relocations, this is type 7 R_386_JUMP_SLOT */
    for (k=0;k<(lth/sizeof(rels[0]));k++) {
      switch (rels[k].r_info&0xff) {
      case R_386_NONE:
      case R_386_COPY:
	/* Nothing to do (to check ???) */
	break;
      case R_386_32:
	/* S + A */
	*((dword*)(rels[k].r_offset))+=
	  base_address+sym[rels[k].r_info>>8].st_value;	
	break;
      case R_386_RELATIVE:
	/* B + A */
	*((dword*)(rels[k].r_offset))+=base_address;	
	break;
      case R_386_PC32:
	/* S + A - P */
	*((dword*)(rels[k].r_offset))+=
	  base_address+sym[rels[k].r_info>>8].st_value+rels[k].r_offset;
	break;
      case R_386_JMP_SLOT:
      case R_386_GLOB_DAT:
	/* S */
	/* If symbol is defined */
	if (sym[rels[k].r_info>>8].st_shndex!=0)
	  *((dword*)(rels[k].r_offset))=sym[rels[k].r_info>>8].st_value;
	else
	  *((dword*)(rels[k].r_offset))=Symbol_Lookup(strtable+sym[rels[k].r_info>>8].st_name);
	break;
      default:
	/* Unknown relocation type */
	__asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('$'));
	break;
      }
      /*
      printf("Relocation entry : %d value %x ",k,sym[rels[k].r_info>>8].st_value);
      printf("  shndx : %x ;Name : %s (val %x)\n",
	     sym[rels[k].r_info>>8].st_shndex,
	     strtable+sym[rels[k].r_info>>8].st_name,
	     Symbol_Lookup(strtable+sym[rels[k].r_info>>8].st_name));
      */   
    }
  }
  
  /* Rel found. Relocate variables */
  if (Rel!=NULL) {
    /* Perform relocation, this is type R_386_GLOB_DAT */
    for (k=0;k<(Relsz/Relent);k++) {
      switch (Rel[k].r_info&0xff) {
      case R_386_NONE:
      case R_386_COPY:
	/* Nothing to do (to check ???) */
	break;
      case R_386_32:
	/* S + A */
	*((dword*)(Rel[k].r_offset))+=
	  base_address+sym[Rel[k].r_info>>8].st_value;	
	break;
      case R_386_RELATIVE:
	/* B + A */
	*((dword*)(Rel[k].r_offset+base_address))+=base_address;	
	break;
      case R_386_PC32:
	/* S + A - P */
	*((dword*)(Rel[k].r_offset+base_address))+=
	  base_address+sym[Rel[k].r_info>>8].st_value+Rel[k].r_offset;
	break;
      case R_386_JMP_SLOT:
      case R_386_GLOB_DAT:
	/* S */
	/* If symbol is defined */
	if (sym[Rel[k].r_info>>8].st_shndex!=0)
	  *((dword*)(Rel[k].r_offset))=sym[Rel[k].r_info>>8].st_value;
	else *((dword*)(Rel[k].r_offset))=Symbol_Lookup(strtable+sym[Rel[k].r_info>>8].st_name);
	break;
      default:
	/* Unknown relocation type */
	__asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" ('W'));
	break;
      }

    }
  }
  
}


