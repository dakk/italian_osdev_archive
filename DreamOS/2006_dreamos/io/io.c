/*
 * Dreamos
 * 8259.h
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 
/*
 * Autore Ivan Gualandri
 * Prima versione: 27/10/2003
 */
  
 #include <io.h>
 
 byte inportb  (word numport)
 {
 //Prende l'input da una porta fisica(numport)
     byte portvalue;
     __asm__  ("inb %%dx,%%al;outb %%al,$0x80" :"=a" (portvalue) : "d"(numport));
     return portvalue;
 }
 
 void outportb (word portdata, word portnum)
 {
 //Manda il dato (portdata) direttamente su una porta fisica (portnum)
 asm volatile ("outb %%al,%%dx;outb %%al,$0x80": :"d" (portnum), "a"(portdata));
  //asm  ("outb %%al, %%dx": : "a"(portdata), "d"(portnum));
 }

