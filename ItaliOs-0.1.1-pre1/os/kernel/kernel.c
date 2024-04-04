/*
ItaliOs
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//#include <include.h>
#include <string.h>
#include <video.h>
#include <gdt.h>
#include <idt.h>
#include <io.h>
#include <8259.h>
#include <keyboard.h>
#include <versions.h>
#include <stdio.h>
#include <speaker.h>
#include <ushell.h>
#include <mem.h>
#include <pit.h>
#include <delay.h>
#include <fs.h>
#include <rtc.h>
#include <dma.h>
#include <fdc.h>
#include <hdd.h>
#include <dir.h>
#include <dsp.h>
#include <const.h>
#include <mixer.h>
#include <vga.h>
#include <task.h>
#include <parallel.h>
#include <serial.h>
#include <math.h>

//#define DIM_STACK 0x2FFF4+(0x2FFF4/2)-(0x2FFF4/4)-(0x2FFF4/8)-(0x2FFF4/16)

//unsigned char KERNEL_STACK[DIM_STACK];        /* lo stack del kernel   */
unsigned int cpuspeed = 0;	/* velocità della cpu in mhz */
unsigned long memtotal = 0;	/* memoria ram totale */
int fdc_type;			/*tipo di floppy */
unsigned short int dsp_device;


void _start(void);		/* La funzione iniziale             */
int main();			/* Inizializzazione di tutte le periferiche e lancio della shell     */
extern void pit_ISR();
void init_system();
void _start(void)
{				/* il pnto di entrata del kernel */
//      asm volatile ("movl %0,%%esp       \n"::     "p" (KERNEL_STACK + DIM_STACK));
/*	printf("\nciao");
	while(1);*/
	init_gdt();

	main();			/* chiamo il main, non dobrebbe mai uscire       */
}


void dummy()
{
	while (1);
}


/*stampoan number punti*/
void perdi_tempo(int number)
{
	int i;
	for (i = 0; i < number; i++) {
		printf(".");
	}
	return;
}

static void versione()
{

	set_back_color(BLU);
	//cls();

	set_text_color(ROSSO + LAMPEGGIO);
	printf("\n\n\t\tItaliOs ");

	set_text_color(BIANCO + LAMPEGGIO);
	printf("Versione: ");

	set_text_color(VERDE + LAMPEGGIO);
	printf("%s \n\n", __ItaliOs_VERSION__);
	return;
}

void fpu_init(void)
{
	asm("finit");
	return;
}

void fpu_test(void)
{
	printf("\nIl seno di 0 e': ");
	print_float(sin(0));
	printf("\nIl coseno di 0 e': ");
	print_float(cos(0));
	printf("\nLa radice quadrata di 9 e': %d", (int) sqrt(9));
	return;
}

int main()
{
	unsigned long delaycount = 0;

	/*#ifdef _486_
	 * float divs = 196.97
	 * #else
	 * unsigned int divs = 500;
	 * #endif */

	// cls();
	versione();

	set_text_color(GIALLO);

//    printf("\nL'indirizzo del main è %x", &main);

	printf("Caricamento......");
	perdi_tempo(15);
	printf("OK\n");


	printf("Inizializzo 8259");
	Init8259();
	perdi_tempo(10);
	printf("OK\n");

	printf("Carico la IDT...");
	InitIDT();
	perdi_tempo(10);
	printf("OK\n");

	/*Calibro in loop del dely */
	/*calibrarlo prima di attivare lo scheduler */
	//printf("Calibro delay loop... [");
	delaycount = calibrateDelayLoop();
	//printf("%l]\n", delaycount);



	printf("Calcolo la quantita' di memoria...");
	count_memory();
	perdi_tempo(10);
	printf("OK\n");



	printf("La quantita' di memoria Ram che tu hai e': %u KB\n",
	       mem_end / 1024);


	printf("Calcolo la velocita' del processore:  ");
	cpuspeed = delaycount / 500;
	perdi_tempo(10);
	printf("OK\n");




	printf("Il tuo processore e' un: %d Mhz \n", cpuspeed);

/*
    printf("\nInizializzo il coprocessore matematico");
    fpu_init();
    perdi_tempo(10);
    printf("OK");
    printf("\n\nInizio test del coprocessore matematico: ");
    fpu_test();
    printf("\nFine test del coprocessore matematico");
*/
	printf("\n\nInizializzo il pit...");
	init_pit(19, 0);	//  (18 volte per secondo
	set_vector(pit_ISR, M_VEC, (D_INT + D_PRESENT + D_DPL3));
	enable_irq(0);
	perdi_tempo(10);
	printf("OK");

	printf("\nInizializzo la paginazione...");
	initmem();

	//while (1);

	perdi_tempo(10);
	printf("OK\n");




	add_task(dummy, "DUMMY");
	add_task(init_system, "INIT SYSTEM");
	//add_task(versione);
	init_task();

	while (1);


	return 0;
}

void init_system()
{
	printf("Inizializzo la tastiera...");
	InitKeyboard();
	perdi_tempo(10);
	printf("OK\n");

	printf("\nInizializzo il floppy...");
	fdc_type = init_floppy();
	perdi_tempo(10);
	printf("OK");

	printf("\nInizializzo il DMA...");
	perdi_tempo(10);
	printf("OK");

	//è un floppy di tipo NEC?
	if (fdc_type == 0x80)
		printf("\nRilevato un floppy di tipo NEC765");
	else
		printf("\nRilevato un floppy di tipo OEM");

	printf("\nRilevamento degli hd in corso..");
	perdi_tempo(15);
	printf("OK\n\n");

	check_hdds();

	printf("\nInizializzazione del file system...");
	perdi_tempo(10);
	fs_init();

	printf("\nRilevamento della parallela in corso...");
	perdi_tempo(12);
	detect_lpt_base_address();

	printf("\nrilevamento seriali");
	detect_UARTs();

	printf("\nCreo l'albero delle cartelle....");
	perdi_tempo(10);
	if (!init_fs_tree()) {
		printf("\nRoot creata");
		//      test_fs();
	}

	add_task(shell, "SHELL");

	suicide();

	while (1);
}
