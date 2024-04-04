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

#include <io.h>
#include <mem.h>
#include <stdio.h>

unsigned long mem_end, bse_end;

void count_memory(void)
 {
         register unsigned long *mem;
         unsigned long mem_count, a;
         unsigned short  memkb;
         unsigned char   irq1, irq2;
         unsigned long cr0;

         /* salvo gli IRQ's */
         irq1=inportb(0x21);
         irq2=inportb(0xA1);

         /* uccido gli irq's */
         outportb(0x21, 0xFF);
         outportb(0xA1, 0xFF);

         mem_count=0;
         memkb=0;

         // memorizzo una copia di CR0
         __asm__ __volatile("movl %%cr0, %0":"=g"(cr0):);

         __asm__ __volatile__ ("wbinvd");

         __asm__ __volatile__("movl %0, %%cr0" :: "g" (cr0 | 0x00000001 | 0x40000000 | 0x20000000));

         do
         {
                 memkb++;
                 mem_count+=1024*1024;
                 mem=(unsigned long *)mem_count;

                 a=*mem;

                 *mem=0x55AA55AA;

                 asm("":::"memory");
                 if(*mem!=0x55AA55AA)
                         mem_count=0;
                 else
                 {
                         *mem=0xAA55AA55;
                         asm("":::"memory");
                         if(*mem!=0xAA55AA55)
                                 mem_count=0;
                 }

                 asm("":::"memory");
                 *mem=a;
         }while(memkb<4096 && mem_count!=0);

         __asm__ __volatile__("movl %0, %%cr0" :: "g" (cr0));

         mem_end=memkb<<20;
         mem=(unsigned long *)0x413;
         bse_end=((*mem)&0xFFFF)<<6;

         outportb(0x21, irq1);
         outportb(0xA1, irq2);
 }
