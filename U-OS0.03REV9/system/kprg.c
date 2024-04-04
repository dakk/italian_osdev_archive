#include <kprg.h>
#include <video.h>
#include <scheduler.h>
#include <keyboard.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <signal.h>
#include <clock.h>
#include <dma.h>
#include <paging.h>
#include <floppy.h>
#include <kernel.h>
#include <idt.h>

#define DELAY_NUM		1000000
#define DELAY_NUM1		500000
sem_t *sem;

void littledelay() {
	int i;
	for (i=0; i<DELAY_NUM; i++);
}

void littledelay1() {
	int i;
	for (i=0; i<DELAY_NUM1; i++);
}

void proc1() {
        asm("cli");
	printf("[PROC1] Take the sem\n");
        asm("sti");
	sem_down(sem);
        asm("cli");
	printf("[PROC1] Take semaforo\n");
        asm("sti");
	littledelay();
        asm("cli");
	printf("[PROC1] Release the sem\n");
        asm("sti");
	sem_up(sem);
        asm("cli");
	printf("[PROC1] Rilasciato Semaforo\n");
        asm("sti");
}

void proc2() {
	littledelay1();
        asm("cli");
	printf("[PROC2] Take the sem\n");
        asm("sti");
	sem_down(sem);
        asm("cli");
	printf("[PROC2] Take semaforo\n");
        asm("sti");
	littledelay();
        asm("cli");
	printf("[PROC2] Release the sem\n");
        asm("sti");
	sem_up(sem);
        asm("cli");
	printf("[PROC2] Rilasciato Semaforo\n");
        asm("sti");
	_kputs("uos# ");
}

void prova_sigsleep() {
    printf("CIAO1\n");
    sig_sleep(get_current_task());
    printf("CIAO2\n");
    while (1);
}

void provagets() {
        char *var1;
        var1 = gets();
        printf("\n%s\n", var1);
}

void sumprg() {
        int var1, var2, ris;
        printf("\nNumber 1: ");
        var1 = getchar() -48;
        printf("%d\n", var1);
        printf("Number 2: ");
        var2 = getchar() -48;
        printf("%d\n", var2);
        ris=var1+var2;
	_kputs("\n\033\003Result:\033\007");
        printf("    %d\n", ris);
}

void halt() {
	sig_kill("1");
	disable_interrupts();
	sig_kill("0");
	_kputs("\t\n\n\033\004 Turn off your PC! \033\007\n\n");
	asm("cli");
	asm("hlt");
}

void prova() {                 
	sem= sem_create();
        asm("cli");
        new_task(proc1, "Proc1");
	new_task(proc2, "Proc2");
        asm("sti");
}
