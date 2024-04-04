#ifndef __FISMEM_H
#define __FISMEM_H

#include <mydef.h>

#define BASE_MEM 0x100000

#define OFFSET_MEM 0x400
#define VALUE_MEM 0xAA55AA55
#define ANTI_VALUE_MEM ~0xAA55AA55

#define MAX_NUM_PAGES 0x100000
#define SIZE_VETT 0x8000

typedef struct {
    size_t mem_map[SIZE_VETT];
}mem_info;

typedef struct {
    void* inizio;
    size_t num_pages;
}pages_struct;

struct mem_area{
    pages_struct pages_info[510];
    size_t next_free;
    struct mem_area* next;
};

typedef struct mem_area *mem_area_pointer;

extern mem_area_pointer mem_list;
extern size_t total_mem;
extern mem_info free_pages;
extern unsigned int pos_mem;
extern int max_pages;
extern int max_vet;

void init_pages();
void mem_size();
void* richiedi_pagine(int);
void rilascia_pagine(void *);
void add_mem_list(size_t, int);
void print_mem_list();

#endif
