#include <mem.h>
#include <mmu.h>
#include <cstring>
#include <sem.h>

extern mem_desc kernel_desc;

namespace {
	sem sem_malloc;
	sem sem_calloc;
	sem sem_free;
	sem sem_realloc;
}

extern "C" void *kcalloc(size_t nmemb, size_t size)
{
	sem_calloc.prendi();
	if (size) {
		void *mem = kmalloc(size * nmemb);
		memset(mem, 0, size * nmemb);
		sem_calloc.lascia();
		return mem;
	}
	sem_calloc.lascia();
	return NULL;
}

extern "C" void *kmalloc(size_t size)
{
	sem_malloc.prendi();
	if (size) {
		void *addr = alloca(size, &kernel_desc);
		sem_malloc.lascia();
		return addr;
	}
	sem_malloc.lascia();
	return NULL;
}

void kfree(void *ptr)
{
	sem_free.prendi();
	if (ptr)
		dealloca(ptr, &kernel_desc);
	sem_free.lascia();
}

void *krealloc(void *pointer, size_t size)
{
	void *punt;
	sem_realloc.prendi();
	if (pointer == 0) {
		punt = kmalloc(size);
		sem_realloc.lascia();
		return punt;
	}
	if (size == 0) {
		kfree(pointer);
		sem_realloc.lascia();
		return pointer;
	}
	if ((punt = kmalloc(size)))
		memcpy(punt, pointer, size);
	kfree(pointer);
	sem_realloc.lascia();
	return punt;
}
