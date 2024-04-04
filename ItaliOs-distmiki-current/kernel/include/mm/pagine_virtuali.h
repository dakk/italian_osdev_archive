#ifndef _PAGINE_VIRTUALI_H_
#define _PAGINE_VIRTUALI_H_

#include <paging.h>
#include <mmu.h>

void init_spazio_virtuale(mem_desc*  desc);

void setta_spazio_virtuale(page_dir_entry* dir, void* linear_address, size_t pagine);

void setta_pagina_virtuale(page_dir_entry* dir, void* linear_address, void* fis_address);

void libera_spazio_virtuale(mem_desc*  desc, void* linear_address, size_t pagine);

void libera_pagina_virtuale(mem_desc*  desc, void* linear_address);

#endif
