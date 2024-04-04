#ifndef _DESC_MEM_H_
#define _DESC_MEM_H_

#include <mmu.h>
#include <cstddef>

void init_desc_mem(mem_desc *desc);

void setta_desc_mem(mem_desc *desc, void* addr, size_t num_page);

size_t libera_desc_mem(mem_desc *desc, void* addr);

#endif
