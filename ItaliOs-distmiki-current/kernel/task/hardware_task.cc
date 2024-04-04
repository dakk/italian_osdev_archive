#include <cstddef>
#include <cstdlib>
#include <video.h>

namespace {
	struct TSS {
		dword edi;	//+0
		dword esi;	//+1
		dword ebp;	//+2
		dword null;	//+3
		dword ecx;	//+4
		dword edx;	//+5
		dword ebx;	//+6
		dword eax;	//+7
		dword eip;	//+8
		dword cs;	//+9
		dword eflags;	//+10
		dword fine;
	};
}

void switch_task(dword stack)
{
      asm("mov %0, %%esp /*reimposto lo stack*/	\n" "popa /*ripristino tutti i registri*/	\n" "sti  /*riabilito gli interrupt*/	\n" "iret /*esco dall'interrupt     */   \n": :"a"(stack));
}

void *make_tss(void (*func) (), void (*end_func) ())
{
	TSS *mytss = (TSS *) ((dword) malloc(0x5000) + 0x5000 - 100);
	mytss->eip = (dword) func;
	mytss->cs = 8;
	mytss->eflags = 0x202;
	mytss->fine = (dword) end_func;
	return mytss;
}

void del_tss(void *tss)
{
	free(tss);
}
