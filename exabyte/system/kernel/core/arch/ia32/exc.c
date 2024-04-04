/* exc.c -- ia32 exceptions */
/******************************************************************************
* Exabyte Operating System                                                    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; either version 2              *
* of the License, or (at your option) any later version.                      *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License           *
* along with this program; if not, write to the Free Software                 *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
*******************************************************************************/

#include <arch-ia32/exception.h>
#include <arch-ia32/segment.h>
#include <arch-ia32/config.h>
#include <kernel.h>	// per l'underscore e compagnia bella ;)
#include <panic.h>

void _exc0();
void _exc1();
void _exc2();
void _exc3();
void _exc4();
void _exc5();
void _exc6();
void _exc7();
void _exc8();
void _exc9();
void _exc10();
void _exc11();
void _exc12();
void _exc13();
void _exc14();
void _exc16();
void _exc17();
void _exc18();
void _exc19();

/* setta degli handler predefiniti per le eccezioni, successivamente verranno impostati dall'esterno */
int ia32_exc_init()
{
	ia32_exc_set_handler(0,KERNEL_CS_SEL,&_exc0,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(1,KERNEL_CS_SEL,&_exc1,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(2,KERNEL_CS_SEL,&_exc2,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(3,KERNEL_CS_SEL,&_exc3,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(4,KERNEL_CS_SEL,&_exc4,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(5,KERNEL_CS_SEL,&_exc5,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(6,KERNEL_CS_SEL,&_exc6,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(7,KERNEL_CS_SEL,&_exc7,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(8,KERNEL_CS_SEL,&_exc8,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(9,KERNEL_CS_SEL,&_exc9,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(10,KERNEL_CS_SEL,&_exc10,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(11,KERNEL_CS_SEL,&_exc11,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(12,KERNEL_CS_SEL,&_exc12,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(13,KERNEL_CS_SEL,&_exc13,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(14,KERNEL_CS_SEL,&_exc14,GATE_TYPE_32BIT_INTGATE,0);
	// exc#15: intel reserved
	ia32_exc_set_handler(16,KERNEL_CS_SEL,&_exc16,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(17,KERNEL_CS_SEL,&_exc17,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(18,KERNEL_CS_SEL,&_exc18,GATE_TYPE_32BIT_INTGATE,0);
	ia32_exc_set_handler(19,KERNEL_CS_SEL,&_exc19,GATE_TYPE_32BIT_INTGATE,0);
	return 0;
}

#ifndef CONFIG_ARCH_FASTASM
/* imposta un handler per l'eccezione <exc>, con <flags> e requested privilege level <rpl> */
int ia32_exc_set_handler(unsigned int exc,unsigned short selector,void * handler,unsigned int flags,unsigned char rpl)
{
/* possibili errori:
   1. rpl deve essere zero
   2. selettore non valido
   3. le eccezioni sono fino a 19
*/
	if(rpl > 0 || !selector || exc > 19) return -1;
	ia32_idt_set_desc(exc,selector,(unsigned long)handler,flags,rpl);
	return 0;
}
#endif

/* handler predefiniti exceptions */
void _exc0 () { panic("codice 0: divisione per zero\n"); }
void _exc1 () { panic("codice 1: eccezione di debug\n"); }
void _exc2 () { panic("codice 2: interrupt non mascherabile\n"); }
void _exc3 () { panic("codice 3: break point\n"); }
void _exc4 () { panic("codice 4: overflow\n"); }
void _exc5 () { panic("codice 5: indice non compreso nell'intervallo\n"); }
void _exc6 () { panic("codice 6: codice operativo non valido\n"); }
void _exc7 () { panic("codice 7: coprocessore non disponibile\n"); }
void _exc8 () { panic("codice 8: double fault\n"); }
void _exc9 () { panic("codice 9: overrun del coprocessore\n"); }
void _exc10() { panic("codice 10: stato del processo non valido\n"); }
void _exc11() { panic("codice 11: segmento inesistente\n"); }
void _exc12() { panic("codice 12: eccezione dello stack\n"); }
void _exc13() { panic("codice 13: protezione generale\n"); }
void _exc14() { panic("codice 14: page fault\n"); }
void _exc16() { panic("codice 16: errore del coprocessore\n"); }
void _exc17() { panic("codice 17: memoria non allineata\n"); }
void _exc18() { panic("codice 18: machine-check exception\n"); }
void _exc19() { panic("codice 19: SIMD Floating-Point exception\n"); }
