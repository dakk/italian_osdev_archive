
/*
   ItalianOs

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

#include <io.h>
#include <mem.h>
#include <stdio.h>
#include <math.h>
#include <mytypes.h>

#define NUM_PAGES 70000
#define VUOTO 0
#define DIM_PAGE 4096

//commentare queste 2 righe per aggiungere i messaggi di debug
#undef debug
#define debug(string, ...)

unsigned long mem_end, bse_end;

static unsigned long page_vect[NUM_PAGES];
static unsigned int color = 0;
static unsigned long num_pages;

extern void enablepg(void);



void setbitmapbit(unsigned long page, unsigned long color);

BOOL isset(unsigned long page);


void count_memory(void)
{
	register unsigned long *mem;
	unsigned long mem_count, a;
	unsigned short memkb;
	unsigned char irq1, irq2;
	unsigned long cr0;

/*
 * save IRQ's 
 */
	irq1 = inportb(0x21);
	irq2 = inportb(0xA1);

/*
 * kill all irq's 
 */
	outportb(0x21, 0xFF);
	outportb(0xA1, 0xFF);

	mem_count = 0;
	memkb = 0;

// store a copy of CR0
	__asm__ __volatile("movl %%cr0, %0":"=g"(cr0):);

// invalidate the cache
// write-back and invalidate the cache
	__asm__ __volatile__("wbinvd");

// plug cr0 with just PE/CD/NW
// cache disable(486+), no-writeback(486+), 32bit mode(386+)
	__asm__ __volatile__("movl %0, %%cr0"::
			     "g"(cr0 | 0x00000001 | 0x40000000 | 0x20000000));

	do {
		memkb++;
		mem_count += 1024 * 1024;
		mem = (unsigned long *) mem_count;

		a = *mem;

		*mem = 0x55AA55AA;

// the empty asm calls tell gcc not to rely on whats in its registers
// as saved variables (this gets us around GCC optimisations)
	      asm("":::"memory");
		if (*mem != 0x55AA55AA)
			mem_count = 0;
		else {
			*mem = 0xAA55AA55;
		      asm("":::"memory");
			if (*mem != 0xAA55AA55)
				mem_count = 0;
		}

	      asm("":::"memory");
		*mem = a;
	}
	while (memkb < 4096 && mem_count != 0);

	__asm__ __volatile__("movl %0, %%cr0"::"g"(cr0));

	mem_end = memkb << 20;
	mem = (unsigned long *) 0x413;
	bse_end = ((*mem) & 0xFFFF) << 6;

	outportb(0x21, irq1);
	outportb(0xA1, irq2);
}


/*
   kmalloc: Allocates a block of memory from kernel's heapa
 */
void *kmalloc(unsigned long size)
{
	unsigned long count, count2, pages = 0, foundcount = 0;
	++color;

	debug("\nKmalloc: la grandezza della parte da allocare e' %d bytes",
	      size);
	pages = size / DIM_PAGE;
	if (size % DIM_PAGE)
		pages++;

	for (count = 1; count <= num_pages; count++) {
//printf("\nkmalloc: analizzo la pagina%d", count);
		if (!isset(count)) {
			if (pages == 1 || pages == 0) {
				debug("\nkmalloc: Ho trovato la pagine %d (1), con il colore %d", count - 1, color);
				setbitmapbit(count, color);
				return (void *) (DIM_PAGE * (count - 1));
			}

			for (count2 = count + 1; count2 <= num_pages;
			     count2++) {
				if (!isset(count2)) {
					foundcount++;
					if (foundcount >= pages) {
						for (count2 = count;
						     count2 < count + pages;
						     count2++)
							setbitmapbit(count2,
								     color);
						debug("\nkmalloc: Ho trovato la pagina %d con il colore %d", count - 1, color);
						return (void *) (4096 *
								 (count - 1));
					}
				}
				if (isset(count2)) {
					foundcount = 0;
					count = count2;
					break;
				}
			}
		}
	}
	return (void *) 0;
}

void free_color(unsigned long color)
{
	int i = 0;
	BOOL trovato = FALSE;
	while (i < NUM_PAGES) {
		if (page_vect[i] == color) {
			trovato = TRUE;
			debug("\nfree_color: Ho trovato il primo elemento del colore %d alla posizione %d", color, i);
			break;
		}
		i++;
	}
	if (trovato == FALSE) {
		debug("\nImpossidile deallocare il colore %d, nun c'è",
		      color);
		return;
	}
	while (page_vect[i] == color) {
		debug("\nfree_color: Sto liberando la pagina %d", i);
		page_vect[i++] = 0;
	}
	return;
}



void kfree(void *pointer)
{
	int page = (int) pointer;
	page /= DIM_PAGE;
	debug("\nkfree: La pagina in questione è %d, il valore di pointer è %d, mentre page vale %d", page, pointer, page2);
	free_color(page_vect[page]);
	return;
}

//Feature oscura realloc(NULL, size) == malloc(size)
//                                 realloc(pointer, 0) == free(pointer)
void *realloc(void *pointer, unsigned int size)
{
	unsigned int *punt;
	if (pointer == 0)
		return malloc(size);
	if (size == 0) {
		free(pointer);
		return pointer;
	}
	if (size < DIM_PAGE)
		return pointer;
	debug("\n\n\nSono dentro la realloc");
	if ((punt = kmalloc(size)))
		memcpy(punt, pointer, size);
	free(pointer);
	debug("\n\n\nrealloc finita");

	return punt;
}




void initmem()
{
	unsigned long count_bitmap, i;
	num_pages = mem_end / DIM_PAGE;
	printf("\ninitmem: il numero di pagine che ho a disposizione e': %d",
	       num_pages);
	printf("\ninitmem: la grandezza del mio vettore e' %d",
	       sizeof(page_vect));
//Preparo la bitmap

	color = VUOTO;
	for (i = 1; i <= num_pages; i++)
		setbitmapbit(i, VUOTO);


//imposto il primo MB come occupato, così sono sicuro che l'utete non possa sovrascrivere ne il kernel
//ne la memoria mappata da dispositivi fisici
	++color;
	for (count_bitmap = 1; count_bitmap <= pow(2, 20) / DIM_PAGE;
	     count_bitmap++)
		setbitmapbit(count_bitmap, color);

/*//creo i descrittori della prima directory
	for (count = 0; count < 1024; count++)
		page[count] =
			make_pgf_address(count * 4096, 0, SUPERVISOR,
					 READ_WRITE, PRESENT);
//imposta la prima dir entry con i descrittori appena creati
	dir[0] = make_pgf_address((unsigned long) page, 0,
				  SUPERVISOR, READ_WRITE, PRESENT);
//Ok, attivo la paginazione
//while(1);
	enablepg();*/

}

void setbitmapbit(unsigned long page, unsigned long color)
{
	page_vect[page - 1] = color;
	if (page > 255)
//debug("\nsetbitmapbit: Imposto la pagina %d con il valore %d", page - 1, color);
		return;
}

BOOL isset(unsigned long page)
{
	return ((page_vect[page - 1] != 0) ? TRUE : FALSE);
}
