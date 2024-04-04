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


/************************************ OSSO ***********************************
 *  File: kernel/Memory.c                                                    *
 *  Memory management functions                                              *
 *****************************************************************************


 *****************************************************************************
 * $Id: memory.c,v 1.22 2001/08/13 23:55:28 iurlano Exp $
 *****************************************************************************
 *
 * $Log: memory.c,v $
 * Revision 1.22  2001/08/13 23:55:28  iurlano
 * FDC Work in progress, but needs DMA
 * DMA work started.
 * Some warnings fixed in kernel code
 *
 * Revision 1.21  2001/06/26 21:00:41  iurlano
 * Added mem information program.
 * Changed shell mem command to meminfo
 *
 * Revision 1.20  2001/06/21 09:46:08  iurlano
 * bugfix. Hoping that freepage bug has gone
 *
 * Revision 1.19  2001/04/26 19:25:54  iurlano
 * fat work
 * bugfixes
 *
 * Revision 1.18  2001/04/19 15:17:58  iurlano
 * Fixed some bug
 * changed ide driver
 * changed fat driver (cabled the pid of the ide driver)
 * changed shell( now accept 3 commands mem, info, ls) TRY IT
 * VERY STABLE SNAPSHOT
 *
 * Revision 1.17  2001/04/19 11:19:03  iurlano
 * fixed bugs
 * fixed bochs problem (very strange)
 *
 * Revision 1.16  2001/04/11 15:26:32  iurlano
 * Thread and processes termination work
 * Memory leak
 *
 * Revision 1.15  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.14  2001/04/05 21:06:13  iurlano
 * Completed xconfig modifies. Now requires the tix package (Sorry but tcl/tk doesn't provide a NoteBook widget :-(( )
 *
 * Revision 1.13  2001/04/05 14:10:54  iurlano
 * xconfig changes
 *
 * Revision 1.12  2001/04/03 07:56:14  iurlano
 * Dynamic Semaphores. Alpha code.
 *
 * Revision 1.11  2001/03/16 19:22:59  iurlano
 * Added something to the make xconfig prog
 * Now OSSO runs on plex86
 *
 * BIG PROBLEM. OSSO gives random errors about finding INIT and relocating
 *   and doesn't start
 *
 * Revision 1.10  2000/12/08 16:45:27  iurlano
 * Thread Work in progress.
 * First working version reached. Needs Debug.
 *
 * Revision 1.9  2000/12/07 15:00:10  marcopex
 * *** empty log message ***
 *
 * Revision 1.8  2000/12/07 12:50:50  marcopex
 * *** empty log message ***
 *
 * Revision 1.7  2000/12/07 11:07:42  marcopex
 * *** empty log message ***
 *
 * Revision 1.6  2000/12/07 10:28:52  marcopex
 * Added linear mapping functions (to be completed...)
 *
 * Revision 1.5  2000/12/05 20:59:47  iurlano
 * Thread work in progress. Yet to make first complete subsystem
 *
 * Revision 1.4  2000/11/26 23:34:59  iurlano
 * work in progress on DMA memory
 *
 * Revision 1.3  2000/08/20 16:46:06  iurlano
 * Fixed bug in init in modules loading
 * IDE driver test. Doesn't work with my Quantum Fireball 4.3 Gb but does
 *   work with my Conner 42MB
 * More translation
 * Fixed some bad comments
 * Changed paging.c for a test (see the comments there)
 *
 * Revision 1.2  2000/07/27 12:13:07  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:11  iurlano
 * OSSO : First Public Release
 *
 *****************************************************************************/ 

#include <memory.h> 
#include <types.h>
#include <panic.h> 
#include <paging.h>
#include <process.h>
#include <printk.h>
#include <semaphore.h>
#include <errors.h>
#include "../Config_Global.h"
#include "../Config_Kernel.h"

/********************/
/* DATA DECLARATION */
/********************/

#define FREEPAGE 0
#define DATAPAGE 1
#define PDIRPAGE 2
#define PTABPAGE 3

/* Memory size info */
dword num_pages;
dword mem_pages;
dword swap_pages=0;   /* not yet ... */
dword free_pages;
extern dword memory_size;


static Bool_Semaphore Mem_Mutex;
dword memory_size;

/* private */
page * pagine;                          /* memory pages vector */
char * pagelist=(char *)BASE_PAGE_ADDR; /* used pages vector */
dword  data_pages;              /* data pages e Page Directory index*/
dword  pt_pages;                /* Page Tables index */

/* .... CUT FROM HERE ... */

/* service function */
/* page * mem_getpage(int tipo); */

/* public (OOP style)*/
/*
void   meminit(void);
PDE  * getpagedirectory(void);
PTE  * getpagetable(void);
page * getpage(void);
void   freepage (page *);

void  * mallosso(mem_size dimensione);
void    freosso(void *);
meminfo getmeminfo(void);
*/

/* ... TO HERE ... */


/*********************/
/* PRIVATE FUNCTIONS */
/*********************/

/* This function returns a pointer to a new page. It is used
   only internally in this file */
page * mem_getpage(int tipo){
  dword i;/*
  if(pagelist[data_pages]==FREEPAGE){
    pagelist[data_pages]=tipo;
    data_pages++;
    free_pages--;
    return &(pagine[data_pages-1]);
    }*/
  Bool_P(&Mem_Mutex);

  i=0;
  while(pagelist[i]!=FREEPAGE && pagelist[i]!=PTABPAGE)i++;
  if(pagelist[i]==FREEPAGE){
    pagelist[i]=tipo;
    free_pages--;
    //printk("0 \n");//,pagelist[( &(pagine[i])-pagine)>>12]);
    //    printk("mem_getpage returning %x\n",&(pagine[i]));
    Bool_V(&Mem_Mutex);
    return &(pagine[i]);
  }
  /* finished ? ... */
  panic("Memory finished!! OLEEEEEE");

  Bool_V(&Mem_Mutex);
  return NULL;
}



/*****************************/
/* KERNEL RESERVED FUNCTIONS */
/*****************************/

void check_memory_size (void) {
#ifdef DIRECT_MEM_SIZE_PROBE
  char * address;
  int test,read;
  address=(char *)(1024*1024+1024);
  do {
    read=*((int *) address);
    *((int *)address)=345110;
    test=*((int *) address);
    *((int *)address)=read;
    address+=1024;
  } while (test==345110);
  address-=1024;
  memory_size=(dword) address;
#else
  memory_size=(dword) SYSTEM_MEMORY_SIZE*1024;
#endif
}


void meminit(void){
  dword i;
  dword pages;         /* total page number */
  dword dim_pagelist;  /* used pages vector size (in pages) */

  /* get memory data */
  pages=(dword) (memory_size-BASE_PAGE_ADDR)/DIM_PAGE;
  dim_pagelist=(pages/DIM_PAGE); 
  if(pages%DIM_PAGE) dim_pagelist++;
  pagine=(page *)(BASE_PAGE_ADDR+(DIM_PAGE*dim_pagelist));
  mem_pages=pages-dim_pagelist;
  num_pages=mem_pages+swap_pages;
  free_pages=num_pages;
  for(i=0;i<num_pages;i++) pagelist[i]=FREEPAGE;
  for(i=0;i<num_pages;i++) if (pagelist[i]!=FREEPAGE) panic("Error initializing memory structures");
  data_pages=0;
  pt_pages=mem_pages-1;
  /* Initialization of Mutual exclusion semaphore */
  Init_Bool_Semaphore(&Mem_Mutex,1);
}

void freepage (page * pagina){
  Bool_P(&Mem_Mutex);

  if (((dword)pagina)&0xFFF) panic("FREEPAGE: Not a page !");
  if(pagelist[(pagina-pagine)]==(char)FREEPAGE) {
    printk("FREEPAGE: PAGE already freed (free pages %d)\n",free_pages);
  }
  //printk("%d\n",pagelist[(pagina-pagine)]);
  pagelist[(pagina-pagine)]=(char)FREEPAGE;
  free_pages++;

  Bool_V(&Mem_Mutex);
}


/* returns a pointer to a new Page Directory */
PDE * getpagedirectory(void){
  dword * tpage;
  int i;

  tpage = (dword *)mem_getpage(PDIRPAGE);
  for (i=0;i<(DIM_PAGE/sizeof(dword));i++)
    tpage[i]=0;
  return (PDE *) tpage;
}
/* returns a pointer to a new Data Page */
page * getpage(void){
  dword * tpage;
  int i;

  tpage = (dword *) mem_getpage(DATAPAGE);
  for (i=0;i<(DIM_PAGE/sizeof(dword));i++)
    tpage[i]=0;
  return (page *) tpage;
}

/* returns a pointer to a new Page Table */
PTE * getpagetable(void){
  Bool_P(&Mem_Mutex);

  if(pagelist[pt_pages]==FREEPAGE){
    dword * tpage;
    int i;

    pagelist[pt_pages]=PTABPAGE;
    pt_pages--;
    free_pages--;
    //printk("0 GetPGTBLE\n");
    Bool_V(&Mem_Mutex);
    //    printk("Getpagetable Restituisce %x\n", &(pagine[pt_pages+1]));
    tpage = (dword *) &(pagine[pt_pages+1]);

    for (i=0;i<(DIM_PAGE/sizeof(dword));i++)
      tpage[i]=0;
    return (PTE *) tpage;
  }

  Bool_V(&Mem_Mutex);
  return NULL;
}

/* This function tries to map a linear space in the requested process,
   aligned to a page and return the logical address of mapping.
*/
dword Linear_alloc(pid_t processo, size_t dimensione, int livello){
  dword pages_needed;
  PDE * page_dir;
  PTE * page_table, * p;
  dword indirizzo_di_ritorno;
  page * temp;
  dword k;
  int i,volta;

  volta=0;
  /* Check if we have enough memory */
  pages_needed=dimensione/DIM_PAGE;
  if (dimensione%DIM_PAGE){
    pages_needed+=1;
  } 
  if(pages_needed > free_pages ){
    /* sorry, no memory left ... */    
    panic("Linear Alloc : Memory finished\n");
    return 0;
  }

  /* Look for enough unmapped space in the process space */
  /* NON ANCORA FATTO !!!!*/
  page_dir=(PDE *)processo->PageDir;

  Bool_P(&(processo->Mem_Sem));
  indirizzo_di_ritorno=(processo->last_used_page)+4096/* oppure + DIM_PAGE? */;
  for (i=0;i<pages_needed;i++) {
    temp=getpage();
    if (!volta){
      //printk("Allocating from %x to ",temp);
      volta++;
    }
    if (temp==NULL) panic("Memory finished !");
    processo->last_used_page+=4096/* oppure + DIM_PAGE? */;
    k=(processo->last_used_page)>>22;
    //printk("Page Dir : %d\n",k);
    if (page_dir[k].Present==0) {
      p=getpagetable();
      if (p==NULL) panic("Memory finished !");
      page_dir[k].Page_Table_Base_Addr=(dword)(p)>>12;
      page_dir[k].Present=1;
      page_dir[k].Read_Write=1;
      page_dir[k].User=livello;
      
    }
    page_table=(PTE *)((dword)page_dir[k].Page_Table_Base_Addr<<12);
    k=((processo->last_used_page)>>12)&0x3FF;
    //printk("Page Table : %d\n",k);
    page_table[k].Page_Base_Addr=((dword)temp)>>12;
    page_table[k].Present=1;
    page_table[k].Read_Write=1;
    if (livello==User_Level) page_table[k].User=1;
    else page_table[k].User=0;
  }
  //printk(" %x\n",temp);
  Bool_V(&(processo->Mem_Sem));  
  return indirizzo_di_ritorno;
} 

/* This function free a linear aligned block in the logical mapping 
   of the process */
int Linear_free (pid_t processo, dword indirizzo, size_t dimensione){
  /*dword * memoria=(dword *)memory;*/
  dword pages_needed;
  PDE * page_dir;
  PTE * page_table;
  dword k,i,j;
  int volta=0;

  Bool_P(&(processo->Mem_Sem));
  page_dir=(PDE *)processo->PageDir;

  pages_needed=(dimensione+4)/DIM_PAGE;
  if ((dimensione+4)%DIM_PAGE){
    pages_needed+=1;
  }

  for(j=0;pages_needed>j;j++){

    k=(dword)((dword)indirizzo)>>22;
    if(page_dir[k].Present==0){
      printk("Ciclo %d/%d: Page dir: %d\n",j,pages_needed,k);
      panic("Freeing unallocated memory !!");
    }
    page_table=(PTE *)((dword)page_dir[k].Page_Table_Base_Addr<<12);
    i=(((dword)indirizzo)>>12)&0x3FF;
    if(!volta){
      //printk("Freeing from %x to ",page_table[i].Page_Base_Addr<<12);
      volta++;
    }
    if(page_table[i].Present==0) {
      printk("Freeing: %x->",page_table[i].Page_Base_Addr<<12);
      printk("Loop %d/%d: Page table: %d\n",j,pages_needed,i);
      panic("Freeing unallocated memory !!");
    }
    //printk("Dealloco: %x->",page_table[i].Page_Base_Addr<<12);

    /* Ho commentato questa parte. Da rivedere, Ale  */
    freepage((page *)(page_table[i].Page_Base_Addr<<12));
    page_table[i].Present=0;
    indirizzo+=4096;
  }
  Bool_V(&(processo->Mem_Sem));

  //printk("%x->",page_table[i].Page_Base_Addr<<12);
  return E_SUCCESS;
} 



/***************************************************/
/* SYSCALLS (PUBLIC FUNCTIONS RESERVED TO PROCESS) */
/***************************************************/

/* memory allocation function (for processes) */
void * mallosso(mem_size dimensione){
  dword * indirizzo;

  indirizzo= (dword *) Linear_alloc(Processo_In_Esecuzione, dimensione+4, Processo_In_Esecuzione->level);
  if(indirizzo!=NULL){ 
    *((dword *)indirizzo)=dimensione+4;
    return (void *)(((dword *)indirizzo)+1);
  } else return NULL;
}


/* memory free function (for processes) */
void freosso (void * indirizzo){
  size_t * ind=(size_t *)indirizzo;

  Linear_free(Processo_In_Esecuzione, (dword)(ind-1), *(ind-1) );
}


/* returns system memory informations */
meminfo * getmeminfo(void){
  meminfo * dati;
  
  dati=mallosso(sizeof(meminfo));
  dati->total=memory_size;
  dati->num_pages=num_pages;
  dati->mem_pages=mem_pages;
  dati->swap_pages=swap_pages;
  dati->free_pages=free_pages;
  return dati;
}


