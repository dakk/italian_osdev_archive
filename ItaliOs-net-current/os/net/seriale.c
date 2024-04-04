#define r_SERIALE
#include "rete.h"

#include <8259.h>
#include <idt.h>
#include <gdt.h>

 u_char *buffCOM=0;
 u_short buffCOMtX=0,buffCOMrX=0; 
/*
 u_short seriali[4]={0x3f8,0x2f8,0x3e8,0x2e8};
 u_char ProvaMDM=0;
*/
extern void com_isr(void);


unsigned int uart_open2(unsigned short number1, unsigned char mode,
		       unsigned int velocita, unsigned char parita,
		       unsigned char len_word, unsigned char stop_bit)
{
   	if (buffCOM==NULL) buffCOM=malloc(10000);
	//abilito l'uso dell'interupt, forzo che la richiesta per spedire e
	//forzo che i dati terminali siano pronti
	outportb(number1 + MCR, 0xB);

	//abilito la con la parità alta
	outportb(number1 + LCR, parita | len_word | stop_bit);

	//decido se abilitare l'interrupt oppure no
	outportb(number1 + IER, mode);
        // setto velocita'

     outb(inb(number1+3)|0x80,number1+3);
     outb((unsigned char)((115200/velocita)&0xff),number1);
     outb((unsigned char)(((115200/velocita)&0xff00)>>8),number1+1);
     outb(inb(number1+3)&0x7f,number1+3);
     return 1;
}


/*
void provamodem() {
  int t;
  u_char ch;
  for (t=0;t<4;t++) { 
   while (!(inb(seriali[t]+0x05)&0x20));
   outportb(seriali[t],66); 
   while (!(inb(seriali[t]+0x05)&0x20));
   outportb(seriali[t],65);
   delay(1000);
   if ((ch=inb(seriali[t]))!=0xff) {
    if (ch==0x65) printf("SERIALE%u: modem presente\n",t+1); else
     printf("SERIALE%u: presente\n",t+1);
   }
  }
}
*/
/* da qui in poi aggiunta in attesa dell'irq variabile */

void isr_com(void)
{
u_char Cgh;
/*        if (ProvaMDM) {
            
        } else */
	if (inb(porta1 + 0x05)&1) {
           buffCOM[buffCOMtX]=Cgh=inb(porta1);
/*           printf("%x,%x(%u) ",buffCOM[buffCOMtX],Cgh,buffCOMtX); */
           if ((++buffCOMtX)>=10000) buffCOMtX=0;
        }
	/*
	 * se il bit 0 del LSR vale 1 allora un dato e pronto nel
	 * RDR (read data register). Questo bit viene automaticamente
	 * resettato quando si fa una lettura nell'RDR
	 */
	outb(EOI, M_PIC);
	return;
}

void scrivibyteseriale(u_char Chg) {
  while (!(inb(porta1+0x05)&0x20));
  outportb(porta1, Chg);
}

u_int leggibyteseriale() {
u_char lgB1;
   if (buffCOMtX!=buffCOMrX) {
    lgB1=buffCOM[buffCOMrX];
/*    printf("[%u]%x,%x ",buffCOMrX,buffCOM[buffCOMrX],lgB1);  */
    if ((++buffCOMrX)>=10000) buffCOMrX=0;
    return ((u_int)lgB1)&0xff; 
    } else
    return NO_DATA;

}

void setta_irq(u_char Sirq)
{
	disable_irq(Sirq);
	set_vector(com_isr, M_VEC + Sirq, (D_INT + D_PRESENT + D_DPL1));
	enable_irq(Sirq);
}


asm(".globl com_isr        \n" 
    "com_isr:              \n" 
    "   pusha              \n"	/* Save all registers               */
    "   pushw %ds          \n"	/* Set up the data segment          */
    "   pushw %es          \n" 
    "   pushw %ss          \n"	/* Note that ss is always valid     */
    "   pushw %ss          \n" 
    "   popw %ds           \n" 
    "   popw %es           \n" 
    "                      \n" 
    "   call isr_com       \n" 
    "                      \n" 
    "   popw %es           \n" 
    "   popw %ds           \n"	/* Restore registers                */
    "   popa               \n" 
    "   iret               \n"	/* Exit interrupt                   */
);
