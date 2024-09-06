/*
 * Dreamos
 * paging.c
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
  * Prima vers//La variabile seguente indica la pagedir, pagetableione: 13/10/2004
  * Thanks to: Mius - La mia guida spirituale
  */

#include <paging.h>
#include <fismem.h>
#include <mydef.h>
#include <stdio.h>
#include <scheduler.h>
#include <task.h>

#define DEBUG

pagedir_t *kernel_pd = (pagedir_t *) 0x1000;
pagetable_t *kernel_pt = (pagetable_t*)0x2000;

void init_paging() {
/* Questa funzione crea una page dir e una page table per il kernel
 * Mappa i primi 4mb (lo spazio di memoria riservato al kernel nella page dir
 * Abilita la paginazione
 */
        int i=0; /*,regcr0*/;
        //pdbr kpd;
        /*Inizializzo i vari valori della page dir*/
        while(i<1024) {
                kernel_pd[i] = 0; /*Setto gli attributi di tutti i campi della page dir a supervisor level, read/write, non present*/
                i++;
        }
        /*Assegno al primo elemento della page dir l'indirizzo della page_table*/
        kernel_pd[0] = (unsigned int)  kernel_pt;
        kernel_pd[0] = kernel_pd[0] | PAGE_FLAGS;
        kernel_pd[1023] = (unsigned int) kernel_pd;
        kernel_pd[1023] = kernel_pd[1023] | PAGE_FLAGS;
        /*Setto la page_table*/
        i=0;
        while(i<1024) {
        /*Questa funzione inserisce nei primi 1024 elementi della page table gli indirizzi dei primi 4mb di memoria*/
                kernel_pt[i] = (i * 4096) | PAGE_FLAGS;
                i++;
        }
	enable_paging((unsigned int) kernel_pd & 0xFFFFF000);
        /*kpd = (unsigned int) kernel_pd & 0xFFFFF000; //Il valore da inserire nel registro cr3
        printf("pdbr: 0x%x - kernel_pd[1023]: 0x%x", kpd, kernel_pd[1023]);
        asm ("movl %%cr0,%0": "=r"(regcr0)); //Leggo Il valore di CR0
        if(regcr0 >> 31) {
                printf("Paginazione gia abilitata\n");
        }
        else {
                printf("Abilito la paginazione\n");
                printf("Copio indirizzo della page dir del kernel in pdbr\n");
                asm volatile("movl %0, %%cr3\n" : : "r"(kpd));
                printf("Setto il bit della paginazione\n");
                regcr0 |= 0x80000000;
                printf("Aggiorno il registro cr0\n");
                asm volatile("movl %0, %%cr0\n": :"r"(regcr0));
                printf("Paginazione abilitata\n");
        }*/
}

void enable_paging(pagedir_t baseAddress){
	int regcr0;
	pdbr kbasepd;
	/*Controllo se la paginazione e' abilitata*/
	kbasepd = (unsigned int) baseAddress&0xFFFFF000;
	printf("kbasepd = 0x%X\n", kbasepd);
	asm("cli");
	assign_pdir(kbasepd);
	printf("TaskPDIR: 0x%X", get_pdir());
	asm ("movl %%cr0,%0": "=r"(regcr0)); 
	printf("RegCR0: 0x%X\n", regcr0);
	if(regcr0 >> 31) {
                printf("Paginazione gia abilitata\n");
        }
	else {
                printf("Abilito la paginazione\n");
                printf("Copio indirizzo della page dir del kernel in pdbr\n");
                asm volatile("movl %0, %%cr3\n" : : "r"(kbasepd));
                printf("Setto il bit della paginazione\n");
                regcr0 |= 0x80000000;
                printf("Aggiorno il registro cr0\n");
                asm volatile("movl %0, %%cr0\n": :"r"(regcr0));
                printf("Paginazione abilitata\n");
        }
	//asm volatile("movl %0, %%cr3\n" : : "r"(kpd);
	asm("sti");
}

void disable_paging(){
	/*Thanks to BlackOS*/
	unsigned int regcr0;
	asm volatile ("mov %%cr0, %0":"=r" (regcr0));
	regcr0 ^= 0x80000000;
	asm volatile ("mov %0, %%cr0"::"r" (regcr0));
	 asm volatile
	("movw $0x10,%%ax     \n"
	"movw %%ax,%%ds      \n"
	"movw %%ax,%%es      \n"
	"movw %%ax,%%fs      \n"
	"movw %%ax,%%gs      \n"
	"movw %%ax,%%ss      \n"
	"ljmp $0x08,$next2    \n"
	"nop\n" "nop\n"
	"next2:               \n":::"%eax");
	return;
}







/*FUNZIONI STILL WORKING*/

void mappage(unsigned int va, unsigned int ra, unsigned int attr) {
/*Questa funzione Mappa un indirizzo reale ra in un indirizzo virtuale va con degli attributi*/
        unsigned int pd_entry, pt_entry, prova;
	pagedir_t *via;
	/*Ottengo pd_entry e pt_entry*/
	pd_entry=(va>>22)&0x3FF;
	pt_entry=(va>>12)&0x3FF;
	printf("pd_entry: 0x%X pt_entry: 0x%X\n", pd_entry, pt_entry); 
	/*Le pagine sono allineate?*/
        if((ra%4096)==0) printf("page aligned\n");
	else printf("Page Not Aligned\n");
	/*La page dir e' presente?*/
	if(is_present(get_pd_virtual_address(pd_entry))) {
	/*Si e' presente edito la pte*/
		printf("Ok proceed to mapping\n");
		/*call to edit_pte*/
	}
	else {
	/*No non e' presente quindi prima creo la page dir e poi edito la pte.*/
		printf("Not present, you need to build a pagedir\n");
		via = get_pd_virtual_address(pd_entry);
		printf("VIA: %x\n", via);
		*via = create_pagedir(pd_entry);
		*via=(*via)|(PAGE_READWRITE | PAGE_SUPERVISOR | PAGE_PRESENT);
		/*Controllo che la creazione sia andata a buon fine*/
		if(is_present((*via))) printf("Ok %x\n", (via));
		else printf("something goes wrong\n");
		/*Chiamo la edit_pte*/
		edit_pte(pd_entry, pt_entry, PAGE_PRESENT | PAGE_READWRITE | PAGE_USER,ra);
		via = (unsigned int*)  va;
		printf("Address of Via: 0x%x\n", via);
		(*via) = 0x01;
		printf("Via: 0x%x, Add: 0x%X\n", *(via), via);
		*via = 0x55;
		printf("Via: 0x%x\n", *via);
		printf("PTVA: 0x%X\n", get_pt_virtual_address(pd_entry, pt_entry));
	}
	/*Stampe di debug*/
	#ifdef DEBUG
	/*printf("Virtual Address: 0x%X Real Address: 0x%x\n", va, ra);
        printf("0x%x, 1 pd el num: 0x%x, 2 pt el num: 0x%x\n", va,pd_entry,pt_entry);
	printf("Pdir Addreass: 0x%X\n", kernel_pd + (pd_entry * 4));
        printf("0x%x, 1: 0x%x, 2: 0x%x\n", va,pd_entry,pt_entry);
        printf("Pd[va1]: 0x%X, Pt[va2]: 0x%x\n", kernel_pd[pd_entry], kernel_pt[pt_entry]);	*/
	#endif
	/*Fine stampe di debug*/
}

void edit_pde(pagedir_t pt_entry, pagetable_t pd_entry, unsigned int flags) {
/* Assegna una page table a un entry della page dir
 * pd_entry = e' l'entry della page dir da modificare
 * pt_entry = e' il valore da assegnarle
 * flags = i flags
 */
	pagedir_t *curpd;
	curpd = get_pd_virtual_address(pd_entry);
	*curpd = (pt_entry | flags);
	
}

void edit_pte(unsigned int pd_entry, unsigned int pt_entry, unsigned int flags, unsigned int real_address){
/*Questa funzione riceve come argomenti: l'entry della page_dir, l'entry della page table, i flags
 *l'indirizzo reale, inserisce all'interno della page table il valore real_addres
 *per fare cio' deve sapere dove si trova la page table chiedendo l'indirizzo virtuale.
 **/
	pagetable_t *curpt;
	if(is_present(get_pd_virtual_address(pd_entry))) printf("OK 2\n");
	printf("PD VA: 0x%X\n", get_pd_virtual_address(pd_entry));
	curpt =  get_pt_virtual_address(pd_entry, pt_entry);
	printf("PT: 0x%X\n", curpt);
	*curpt = (real_address | flags);
	printf("Curpt: 0x%X\n", curpt);
}

pagedir_t* get_pd_virtual_address(unsigned int pd_entry){
	return (pagedir_t *)((1023<<22) + (1023<<12) + (pd_entry*4));
}

pagetable_t* get_pt_virtual_address(unsigned int pd_entry, unsigned int pt_entry){
/*Questa funzione prend
 */
	printf("PD_ENTRY: 0x%X\n", pd_entry<<12);
	return (pagetable_t *)(0xFFC00000 + (pd_entry<<12) + (pt_entry*4));
}

int is_present(pagedir_t* address){
	if(*address&PAGE_PRESENT) return 1;
	else return 0;
}


pagedir_t* create_pagedir(unsigned int pd_entry) {
	return (pagedir_t*) fis_alloc(1);
}




/*Funzioni di Debug*/

void printcr0() {
//Questa funzione stampa a video il valore del registro cr0.
//Forse la converto a int come valore di ritorno e le cambio nome
        pagedir_t regcr0; //Conterra' il valore di cr0
        asm ("movl %%cr0,%0": "=r"(regcr0));
        printf("Cr0 contiene il valore: 0x%x\n", regcr0);
        if(regcr0 & 0x80000000) {
                printf("Paginazione Abilitata\n");
        }
        else {
                printf("Paginazione Non Abilitata\n");
        }
}

void noop() {
        /*Funzione di test*/
        int i=0;
        /*primo elemento page dir conterra' una page table, che sara' riservata al kernel*/
        while(i<1024) {
                kernel_pd[i] = 0x03;
                i++;
        }
        printf("Prova: %d\n", kernel_pd[100]);
}

void is_presenttry() {
/*Da eliminare*/
	pagedir_t* addr1;
	pagedir_t* addr2;
	pagedir_t* addr3;
	pagedir_t* addr4;
	addr1=get_pd_virtual_address(1023);
	addr2=get_pd_virtual_address(0);
	addr3=get_pd_virtual_address(1);
	addr4=get_pd_virtual_address(100);
	if(is_present(addr1)) printf("1 present\n");
	else printf("1 not present\n");
	if(is_present(addr2)) printf("2 present\n");
	else printf("2 not present\n");
	if(is_present(addr3)) printf("3 present\n");
	else printf("3 not present\n");
	if(is_present(addr4)) printf("4 present\n");
	else printf("4 not present\n");

}
