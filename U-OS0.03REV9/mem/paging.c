#include <paging.h>
#include <mydef.h>
#include <stdio.h>


page_dir pagedir_entry[1024];
page_table pagetable[1024];
void init_pagedir() {

    unsigned int registro;
    int i=0;
    while(i<1024) {
        pagedir_entry[i]= 0x00000000;
        i++;
    }
    asm("movl %%cr0,%0    \n": "=r"(registro));
    if(registro>>31) printf ("Paginazione gia' abilitata %x\n", registro);
    else printf("Abilito la paginazione");
    pagedir_entry[0] = ((int)  pagetable) & 0xFFFFF000;
    pagedir_entry[0] = pagedir_entry[0] | 0x01;
    pagetable[0] = 0x00000001;
}

void create_pagetable(unsigned int parameters) {

}

void readcr0() {
    unsigned int registro, registro1;
    registro = 0;
    registro1=0;
    asm ("movl %%cr0,%0    \n"
             : "=r"(registro));
    printf("\nValore cr0: 0x%x\n", registro);
    printf("\nNuovo valore cr0: 0x%x\n", registro|0x80000000);
}
