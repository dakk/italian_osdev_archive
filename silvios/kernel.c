/*
SilviOs

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

#include <string.h>
#include <video.h>
#include <gdt.h>
#include <idt.h>
#include <io.h>
#include <8259.h>
#include <keyboard.h>
#include <versions.h>
#include <time.h>
#include <stdio.h>
#include <speaker.h>
#include <ushell.h>
#include <mem.h>
#include <pit.h>
#include <delay.h>

#define uchar  unsigned char		/* Defentitions to make life	*/
#define uint   unsigned int		/* Easy for us:  uchar, uint	*/
#define ulong  unsigned long int	/* and ulong			*/
#define iuchar inline unsigned char	/* Inline functions		*/
#define ivoid  inline void

uchar KERNEL_STACK[8192];		/* The stack to use: 8k stack	*/
unsigned int cpuspeed=0;		/* CPU Speed in Megahertz */
unsigned long memtotal = 0;		/* Memory Total in bytes */

void _start(void);			/* The entry point		*/
int main();				/* The main kernel program 	*/
void _exit(void);			/* What to do when finished	*/
void ulong_strcnv(uchar *str, ulong i); /* Convert a dword to a string  */

desc_table(GDT, 3)
{
	{dummy:0},
	stnd_desc(0, 0xFFFFF, (D_CODE + D_READ + D_BIG + D_BIG_LIM)),
	stnd_desc(0, 0xFFFFF, (D_DATA + D_WRITE + D_BIG + D_BIG_LIM)),
};

struct
{
	unsigned short limit __attribute__ ((packed));
	union DT_entry *idt __attribute__ ((packed));
} loadgdt = { (3 * sizeof(union DT_entry) - 1), GDT };

void _start(void)			/* il pnto di entrata del kernel*/
{
   asm ("pushl $2; popf");

	 asm volatile                /*Carico la gdt nello stack  */
	 (
      		"lgdtl (loadgdt)     \n"
	        "movw $0x10,%%ax     \n"
      		"movw %%ax,%%ds      \n"
      		"movw %%ax,%%es      \n"
      		"movw %%ax,%%fs      \n"
      		"movw %%ax,%%gs      \n"
      		"movw %%ax,%%ss      \n"
      		"movl $0xFFFF,%%esp       \n"
      		"ljmp $0x08,$next    \n"
		"nop\n"
		"nop\n"
	        "next:               \n" /* Continue here                         */
      		:
      		: "r" (GDT), "p" (KERNEL_STACK+4096)
      		: "%eax"
   	);

   main();				/* chiamo il main	*/
   _exit();				/* finisce e spegne			*/
}

/*stampoan number punti*/
void perdi_tempo(int number){
	int i;
	for (i =0; i < number; i++){
		kprint(".");
	}
	return;
}

int main()
{
	unsigned char tempstring[255];
	unsigned long delaycount=0;

	#ifdef _PENTIUM1_
		unsigned int divs = 500;
	#else
		#ifdef  _486_
			float divs = 196.97;
		#else
			float divs = 500.00;
		#endif
	#endif


	cls();

	set_back_color(BLU);
	cls();

	set_text_color(ROSSO+LAMPEGGIO);
	kprint("\n\n                       SilviOs Versione: ");
	kprint(__SILVIOS_VERSION__);
	kprint("\n\n");

	set_text_color(GIALLO);

	kprint("Caricamento......");
	perdi_tempo(15);
	kprint("OK\n");

	kprint("Inizializzo 8259");
	Init8259();
	perdi_tempo(10);
	kprint("OK\n");

	kprint("Carico la IDT...");
	InitIDT();
	perdi_tempo(10);
	kprint("OK\n");

	 /*Calibro in loop del dely*/
        /*calibrarlo prima di attivare lo scheduler*/
	kprint("Calibro delay loop... [");
        delaycount = calibrateDelayLoop();
        printlong(delaycount);
	kprint("]\n");

	kprint("\n");

	kprint("Calcolo la quantita' di memoria...");
	count_memory();
	perdi_tempo(10);
	kprint("OK\n");

	kprint("La quantita' di memoria Ram che tu hai e': ");
	printlong(mem_end/1024);
	kprint(" KB\n");

	kprint("\n");

	kprint("Calcolo la velocita' del processore: ");
	cpuspeed=delaycount/divs;
	perdi_tempo(10);
	kprint("OK\n");

	kprint("Il tuo processore e' un:  ");
	printlong(cpuspeed);
	kprint(" Mhz\n");
	
	kprint("\n");

	kprint("Inizializzo la tastiera...");
	InitKeyboard();
	perdi_tempo(10);
	kprint("OK\n");

	/*inno_italia();*/

	kprint("\nPer la lista dei comandi fare help\n");

 	while(1)
	{
	  	kprint("\nSilviOS] /: ");
		getstring(tempstring);
		kprint("\n");

		if(!kstrcmp(tempstring, "ver")){
			 kprint("\nSilviOs Versione ");
			 kprint(__SILVIOS_VERSION__);
			 kprint(" By Abruzzo Silvio goku_vegeta@libero.it\n");
			 continue;
		}

	        if (!kstrcmp(tempstring, "date")){
		  sys_time(0);
		  kprint("\n");
		  continue;
		}
		if(!kstrcmp(tempstring, "time")){
		  sys_time(1);
		  kprint("\n");
		  continue;
		}
		if(!kstrcmp(tempstring, "reboot")){
		  break;
		}

		if(!kstrcmp(tempstring, "cls")){
		  cls();
		  continue;
		}

		if(!kstrcmp(tempstring, "sound")){
		  int freq, tempo;
		  kprint("\n");
		  kprint("Inserisci la frequenza: ");
		  freq = get_number();
		  kprint("\nInserisci il tempo: ");
		  tempo = get_number();
		  suona(freq, tempo);
		  kprint("\n");
		  continue;
		}

		if(!kstrcmp(tempstring, "text_color")){
		  unsigned char colore;
		  kprint("\n Inserisci il numero del  colore desideri\n Numerori validi da 0 a 15: ");
		  colore = get_number();
		  if  (colore<=15)
		    set_text_color(colore);
		  else
		  	kprint("\nERRORE: Colore non valido\n");
		  kprint("\n");
		  continue;
		}

		if(!kstrcmp(tempstring, "back_color")){
		  unsigned char colore;
		  kprint("\n Insrisci il numero del colore che desideri\n Numeri validi da 0 a  7: ");
		  colore = get_number();
		  if (colore<=7){
		  	  set_back_color(colore);
		          cls();
		}else{
			kprint("\nERRORE: colore non valido\n");
		}
		  continue;
		}

		if(!kstrcmp(tempstring, "list_color")){
			list_color();
			continue;
		}

		if(!kstrcmp(tempstring, "help")){
		  help();
		  continue;
		}

		//sei ancora qui?
		kprint("Hai inserito un comando sbagliato. Per una lista dei comandi fare Help. Ritenta e sarai piu' fortunato\n");

  	}
	return 0;
}

void _exit(void)			/*Punto di uscita del kernel	*/
{
   while(inportb(0x64) & 0x02);		/* Riavvia il computer		*/
   outportb(0x64, 0xfe);

   asm("cli;hlt");
}
