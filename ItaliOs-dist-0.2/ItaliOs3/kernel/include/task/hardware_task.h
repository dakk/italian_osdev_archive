#ifndef _HARDWARE_TASK_H_
#define _HARDWARE_TASK_H_

#include <cstddef>

void switch_task(dword stack);
	
void *make_tss(void (*func)(),void (*end_func)() );

void del_tss(void* tss);

#endif
