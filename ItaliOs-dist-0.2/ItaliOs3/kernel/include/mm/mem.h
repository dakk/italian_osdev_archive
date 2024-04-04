#ifndef _MEM_H_
#define _MEM_H_

#ifdef __cplusplus
	#include <cstddef>
	extern "C"{
#else
	#include <stddef.h>
#endif

void *kcalloc(size_t nmemb, size_t size);
void *kmalloc(size_t size);
void kfree(void *ptr);
void *krealloc(void *ptr, size_t size);
		
		
#ifdef __cplusplus
	};
#endif
	
#endif
