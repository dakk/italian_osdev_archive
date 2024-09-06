/*
 * Dreamos
 * paging.h
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

 /*
  * Autore Ivan Gualandri
  * Prima versione: 13/10/2004
  * Thanks to: Mius - La mia guida spirituale
  */

#ifndef __PAGING_H
#define __PAGING_H

#include <mydef.h>
/*Page_dir struttura dati che conterra' le informazioni per i vari elementi della page_dir*/
#define PAGE_DIR_SIZE 1024
#define PAGE_FLAGS 3 /*Vuol dire: pagina read_only, supervisor, present*/

#define PAGE_NOTPRESENT 0x00
#define PAGE_READ 0x00
#define PAGE_SUPERVISOR 0x00
#define PAGE_READWRITE 0x02
#define PAGE_PRESENT 0x01
#define PAGE_USER 0x04

typedef unsigned int pagedir_t;  //Il tipo che sara' usato per le entry della page_dir
typedef unsigned int pagetable_t; //Tipo utilizzato per le entry della page_table
typedef unsigned int pdbr; //Tipo che sta ad indicare l'indirizzo base della page dir, che si inserisce nel registro cr3

void printcr0();
void enable_paging(); /*Questa funzione abilita la paginazione forse tornera' un int*/
void disable_paging(); /*Questa funzione disabilita la paginazione*/


extern pagedir_t *kernel_pd; /*Questa variabile conterra' la page dir del kernel*/

void init_paging();
void mappage(unsigned int, unsigned int, unsigned int);
void noop();

void edit_pte(unsigned int, unsigned int, unsigned int, unsigned int);
void edit_pde(unsigned int, unsigned int, unsigned int);/*Per accedere ai singoli elementi di pdir e ptable*/

pagedir_t* get_pd_virtual_address(unsigned int); 
pagetable_t* get_pt_virtual_address(unsigned int, unsigned int); /*Qyeste due funzioni tornano l'ind virtuale della pd e della pt*/
int is_present(pagedir_t*);
void is_presenttry();
pagedir_t* create_pagedir(unsigned int);
#endif
