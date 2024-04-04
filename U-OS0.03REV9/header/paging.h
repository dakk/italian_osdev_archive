#ifndef __PAGING_H
#define __PAGING_H

#include <mydef.h>

#define PAGE_DIR_SIZE 1024

typedef unsigned int page_dir;
typedef unsigned int page_table;

extern page_dir pagedir_entry[1024];
extern page_table pagetable[1024];

void init_pagedir();
void create_pagetable(unsigned int);

void readcr0();

#endif
