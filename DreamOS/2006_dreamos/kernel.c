/**
 *   kernel.c
 *   DreamOS kernel TEST
 *   http://www.dreamos.org/
 */

#define VERSION "v0.0-TEST 14 (snapshot finarfin) - 1st 2006"
#define SITEURL "www.dreamos.org"

int main(void);
void initOS(void);

void _start()
{  
  initOS();
  while(1);
}

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
#include <paging.h>
#include <apm.h>
#include <video.h>

void mouse_irq() {_kputs("Prova Mouse\n"); }

void initOS() {
  _kclear();
  _kputs("\033\003--\n- DreamOS\n- " VERSION "\n- " SITEURL "\n--\n\n\033\007");

  //Identificazione CPU
  _kcpuid();
  
  //Calcolo quantita di ram
  printf("\n- Calcolo la quantita di ram... ");
  mem_size();
  printf("%d kb rilevati\n", total_mem/1024);
  //Inizializzo il gestore della memoria fisica
  init_pages();
  printf("- Numero di pagine a disposizione: %d\n", max_pages);
  printf("- Numero elementi della bitmap: %d\n", max_vet);
  _kputs("\nRilevo lettore floppy:"); detect_floppy();
  //Inizializzazione PIC
  disable_interrupts();
  
  _kputs("\n- Inizializzo PIC8259...");
  initPIC8259();
  _kprintDone();
  
  //Enable keyboard
  _kputs("- Abilito la Tastiera...");
  initKEYBOARD();
  _kprintDone();
  enable_interrupts();
  
  //Inizializzazione IDT
  disable_interrupts();
  _kputs("- Inizializzo la IDT... ");
  initIDT();
  _kprintDone();
  enable_interrupts();
  
  //Inizializzazione PIT
  _kputs("- Inizializzo PIT...    ");
  InitPIT();
  _kprintDone();
  
  //Inizializzazione Dma
  _kputs("- Inizializzo DMA...    ");
  InitDma();
  _kprintDone();
  
  disable_interrupts();
  _kputs("- Inizializzo FLOPPY...    ");
  init_floppy();
  enable_interrupts();

  //Inizializzazione struttura apm_setup
  _kputs("- Inizializzo l'APM...  ");
	init_apm();
  _kprintDone();
  
  init_scheduler();
  
  //_ksetcursor(0,0);
  
  new_task(idle_task, "idle");
  _kputs("Entering dreamshell... ");
  new_task(dreamshell, "shell");
}
