#include<io.h>
#include<interrupt.h>


typedef struct descrittore vettore_di_interrupt;
vettore_di_interrupt IDT[DIM_IDT];

/********************************************************************************
********************************************************************************/
//handler di default
void handler_default(void)
{
  printk("\n\n Handler di default\n");
  asm("iret"); //Ritorna dall' interrupt
}
/********************************************************************************
********************************************************************************/
/*Funzione per impostare l'idtr. Ricordo che il registro idtr deve contenere un dato di 48 bit. 16 bit che 
  indicano la dimensione dell' IDT e 32 bit che indica l' indirizzo base dell' IDT. */
void carica_idt(vettore_di_interrupt* base_addr, int num_desc) {
		unsigned int idt_reg[2];
		idt_reg[0]=(num_desc*8) << 16;
		idt_reg[1]=(unsigned int)base_addr;
		__asm__ __volatile__ ("lidt (%0)": :"g" ((char *)idt_reg+2));
	}
/********************************************************************************
********************************************************************************/
//Tramite questa funzione assegniamo ad ogni interrupt il proprio handler(ovvero la propria funzione di gestione)
void Init_IDT_entry(void (*handler)(void), int indice){
        if(indice < DIM_IDT){   /* Vediamo se si tratta di un interrupt valido. Se è minore di della dimensione 
				  dell' IDT allora è valido */

                IDT[indice].offset0_15 = (unsigned int)handler & 0xFFFF;       //Consideriamo i bit da 0 a 15
		IDT[indice].offset16_31 = (unsigned int)handler >> 16; //Consideriamo i bit da 16 a 31
		IDT[indice].segmento = 0x8;      //Segmento codice
		IDT[indice].opzioni = 0x470;   //Opzioni del descrittore
	}

else
printk("\n\nInterrupt non valido\n");
}
/********************************************************************************
********************************************************************************/
//Inizializzazione dell' IDT
void initIDT(void){
int i;

	//impostiamo L' IDT con gli handler di default
        for(i = 17; i < DIM_IDT; i++)
                Init_IDT_entry(handler_default, i);

	/* Gestiamo le Eccezioni.*/

	Init_IDT_entry(trap0, 0);
	Init_IDT_entry(trap1, 1);
	Init_IDT_entry(trap2, 2);
	Init_IDT_entry(trap3, 3);
	Init_IDT_entry(trap4, 4);
	Init_IDT_entry(trap5, 5);
	Init_IDT_entry(trap6, 6);
	Init_IDT_entry(trap7, 7);
	Init_IDT_entry(trap8, 8);
	Init_IDT_entry(trap9, 9);
	Init_IDT_entry(trap10, 10);
	Init_IDT_entry(trap11, 11);
	Init_IDT_entry(trap12, 12);
	Init_IDT_entry(trap13, 13);
	Init_IDT_entry(trap14, 14);
	Init_IDT_entry(trap15, 15);
	Init_IDT_entry(trap16, 16);

  /* Fine della gestione delle eccezioni */

	/* A questo punto possiamo chiamare la funzione che si occupa di caricare l' indirizzo dell' IDT 
	   nel registro idtr */
	carica_idt(IDT, DIM_IDT);
	

	asm("sti"); //attiviamo gli interrupt
}



//Handler delle eccezioni

void trap0(void)
{
printk("\n\n #Trap0#: Division By Zero\n");
while(1);
}


void trap1(void)
{
printk("\n\n #Trap1#: Trap of Debug\n");
  while(1);
}


void trap2(void)
{
printk("\n\n #Trap2#: NMI !!!\n");
  while(1);
}


void trap3(void)
{
printk("\n\n #Trap3#: Breakpoint\n");
  while(1);
}


void trap4(void)
{
printk("\n\n #Trap4#: Overflow\n");
  while(1);
}


void trap5(void)
{
printk("\n\n #Trap5#: Bound Range Exceeded\n");
  while(1);
}


void trap6(void)
{
printk("\n\n #Trap6#: Invalid Opcode\n");
  while(1);
}


void trap7(void)
{
printk("\n\n #Trap7#: Coprocessor Not Available\n");
  while(1);
}


void trap8(void)
{
printk("\n\n #Trap8#: Double Fault\n");
  while(1);
}


void trap9(void)
{
printk("\n\n #Trap9#: Coprocessor Segment Overrun\n");
  while(1);
}


void trap10(void)
{
printk("\n\n #Trap10#: Invalid Task State Segment\n");
  while(1);
}


void trap11(void)
{
printk("\n\n #Trap11#: Segment Not Present\n");
  while(1);
}


void trap12(void)
{
printk("\n\n #Trap12#: Stack Fault\n");
  while(1);
}


void trap13(void)
{
printk("\n\n #Trap13#: General Protection\n");
  while(1);
}


void trap14(void)
{
printk("\n\n #Trap14#: Page Fault\n");
  while(1);
}


void trap15(void)
{
printk("\n\n #Trap15#: UNKNOWN ?????\n");
  while(1);
}


void trap16(void)
{
printk("\n\n #Trap16#: Error of Floating Point\n");
  while(1);
}


void trap17(void)
{
printk("\n\n #Trap17#: Memory Not Alligned\n");
  while(1);
}
//Fine handler delle eccezioni
/********************************************************************************
********************************************************************************/
/* Quando c'è bisogno di gestire un irq(ad esempio quando si preme un tasto) verrà chiamata questa funzione, la quale non può
   essere una funzione C visto che il compilatore mette RET come istruzione finale di una funzione, mentre un interrupt handler
   deve terminare con IRET */
void handler_asm()
 {

    asm(".globl handler_asm  \n"

    /*Disabilito gli interrupt con 'cli' e salvo i registri */
         "handler_asm: \n"
         "cli \n"
         "pusha \n"
	 "pushw   %ds \n"
	 "pushw   %es \n"
	 "pushw   %fs \n"
	 "pushw   %gs \n"
	 "movw    $0x10,%si \n"
	 "movw    %si,%ds \n"
 	 "movw    %si,%es \n"

    /* Quindi chiamo la funzione che si occupa di chiamare l' handler vero e proprio */
         "call gestisci_irq \n"

    /*Ripristino i valori dei registri e riabilito gli interrupt con 'sti' */
	"cli \n"     /* NO INTERRUPT! */
	"popw    %gs \n"
	"popw    %fs \n"
	"popw    %es \n"
	"popw    %ds \n"
	"popa \n"
	"sti \n"
	"iret \n" //Ritorna dall' interrupt
);



 }
