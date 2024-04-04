#ifndef _PAGINE_VIRTUALI_H_
#define _PAGINE_VIRTUALI_H_

#include <paging.h>

void init_spazio_virtuale(page_dir_entry* dir);

void setta_spazio_virtuale(page_dir_entry* dir, void* linear_address, size_t pagine);

#endif
