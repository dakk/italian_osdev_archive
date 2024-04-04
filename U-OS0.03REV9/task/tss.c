#include <tss.h>
#include <task.h>

void new_tss(tss_t *tss, void (*func)(), void (*end)())
{
    tss->eax=0;
    tss->ebx=0;
    tss->ecx=0;
    tss->edx=0;
    tss->edi =0;
    tss->esi =0;
    tss->cs = 8;
    tss->eip = (unsigned)func;
    tss->eflags = 0x202;
    tss->fine = (unsigned)end;
}
