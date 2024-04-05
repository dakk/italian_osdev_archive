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

#include <gdt.h>
#include <stdio.h>
#include <mytypes.h>


#define MAX_ELEM 100

#define GDT_VUOTO 0
#define GDT_PIENO 1


union DT_entry GDT[MAX_ELEM];
unsigned char gdt[MAX_ELEM];	//qui ci metto se è occupato oppure no l'elemento della matrice


struct {
	unsigned short limit __attribute__ ((packed));
	union DT_entry *idt __attribute__ ((packed));
} loadgdt = {
(MAX_ELEM * sizeof(union DT_entry) - 1), GDT};

/*se viene tornato 0 significa che non ci sono + elementi disponibili*/
unsigned int get_first_gdt_empty(void)
{
	unsigned int i;
	for (i = 0; i < MAX_ELEM; i++)
		if (gdt[i] == GDT_VUOTO) {
			gdt[i] = GDT_PIENO;
			return i;
		}
	return 0;
}


unsigned int add_desc_to_gdt(unsigned int base, unsigned int limit,
			     unsigned int opt)
{
	unsigned int i = get_first_gdt_empty();
//    debug("\nmi trovo nella add_desc_to_gdt");
	GDT[i].desc.base_low = base & 0xFFFF;
	GDT[i].desc.base_med = (base >> 16) & 0xFF;
	GDT[i].desc.base_high = base >> 24;
	GDT[i].desc.limit_low = limit & 0xFFFF;
	GDT[i].desc.limit_high = (limit >> 16) & 0x0F;
	GDT[i].desc.access = (opt + D_PRESENT) >> 8;
	GDT[i].desc.granularity = ((opt & 0xff) >> 4);
	return i * 8;
}

void init_gdt(void)
{
	unsigned int i;
	for (i = 0; i < MAX_ELEM; i++)
		gdt[i] = GDT_VUOTO;
	gdt[0] = GDT_PIENO;	//il primo elemento della gdt non è usabile e va posto a zero
	GDT[0].dummy = 0;


	add_desc_to_gdt(0, 0xFFFFFFFF, (D_CODE + D_READ + D_BIG + D_BIG_LIM));
	add_desc_to_gdt(0, 0xFFFFFFFF,
			(D_DATA + D_WRITE + D_BIG + D_BIG_LIM));

	asm("pushl $2; popf");

	//    printf("\nPrima del caricamenteo della gdt");

	//"movl $0xFFFF,%%esp       \n"

	asm volatile
	  ("lgdtl (loadgdt)     \n" "movw $0x10,%%ax     \n" "movw %%ax,%%ds      \n" "movw %%ax,%%es      \n" "movw %%ax,%%fs      \n" "movw %%ax,%%gs      \n" "movw %%ax,%%ss      \n" "ljmp $0x08,$next    \n" "nop\n" "nop\n" "next:               \n"::"r" (GDT)	/*, "p"(KERNEL_STACK + (4096 * 16)) */
	   :"%eax");

	//printf("\ndopo il  caricamenteo della gdt");






	return;
}
