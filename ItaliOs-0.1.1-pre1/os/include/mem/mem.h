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

#ifndef MEM_INCLUDE
#define MEM_INCLUDE

#include <mytypes.h>

#define malloc kmalloc
#define free kfree

unsigned long mem_end, bse_end;

void count_memory();
BOOL isset(unsigned long page);

/*inizializza le sttture per la memoria*/
void initmem(void);

/*iniziallizza un blocco di memoria di size bytes*/
void *kmalloc(unsigned long size);

/*libera  la zona di memoria puntata da ponter*/
void kfree(void *pointer);

/*alloca ed assegna al puntatore un  zone di memoria di size bytes*/
void *realloc(void *pointer, unsigned int size);


#endif


