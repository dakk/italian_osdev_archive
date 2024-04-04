#ifndef __TSS_H
#define __TSS_H

typedef struct registri tss_t;

void new_tss(tss_t*,void (*func)(), void (*end)());
#endif
