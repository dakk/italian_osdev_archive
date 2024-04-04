int main(void);
void initOS(void);

void _start()
{
  initOS();
  while(1);
}

#include <kernel.h>
#include <string.h>
#include <video.h>
#include <fismem.h>
#include <cpuid.h>
#include <idt.h>
#include <8253.h>
#include <8259.h>
#include <keyboard.h>
#include <stdlib.h>
#include <stdio.h>
#include <task.h>
#include <scheduler.h>
#include <signal.h>
#include <shell.h>
#include <semaphore.h>
#include <dma.h>
#include <floppy.h>
#include <stdarg.h>

void initOS()
{
    _kclear();
    _kputs("\033\003--\n-" OSNAME "\n- " OSVERSION "\n- " OSSITEURL "\n--\n\n\033\007");

    _kcpuid();
/*------------------*/
    printf("\n- Initialize RAM... \n");
    	mem_size();
	init_pages();
    printf("    \t %d  b RAM\n", total_mem/1024);
    printf("    \t %d kb RAM\n", total_mem/1024);
    printf("    \t %d mb RAM\n", total_mem/1024/1024);
/*------------------*/
    _kputs("\n- Detect Floppy:");
   	detect_floppy();
   	disable_interrupts();
/*------------------*/    
    _kputs("\n- Initialize PIC8259...   [");
        initPIC8259();
    _kputs("\033\002 OK \033\007]\n");
/*------------------*/
    _kputs("- Initialize KEYBOARD...  [");
        initKEYBOARD();
    _kputs("\033\002 OK \033\007]\n");
    	enable_interrupts();
/*------------------*/
   	disable_interrupts();
    _kputs("- Initialize IDT...       [");
        initIDT();
    _kputs("\033\002 OK \033\007]\n");
     	enable_interrupts();
/*------------------*/
    _kputs("- Initialize PIT...       [");
        InitPIT();
    _kputs("\033\002 OK \033\007]\n");
/*------------------*/
    _kputs("- Initialize DMA...       [");
        InitDma();
    _kputs("\033\002 OK \033\007]\n");
/*------------------*/
    _kputs("- Initialize SCHEDULER... [");	
        init_scheduler();
    _kputs("\033\002 OK \033\007]\n");
/*------------------*/
        disable_interrupts();
    _kputs("- Initialize FLOPPY...    \n");
        init_floppy();   
        enable_interrupts();
 /*------------------*/
    
   _ksetcursor(0,0);

   new_task(idle_task, "idle");
   new_task(shell, "shell");
}
