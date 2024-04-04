/*
ItaliOs

Copyright (C) 2003 Abruzzo Silvio
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

/*File generato da LD per produrre binari standard e modificato per ottenere caratteristiche particolari, come l'etichettamento delle zone di memoria dei costruttori*/

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(main)
SECTIONS
{
  . = 0x10000;
  .text           :  { *(.text .stub .text.* .gnu.linkonce.t.*)  }
  PROVIDE (etext = .);   
  .rodata         : { *(.rodata .rodata.* .gnu.linkonce.r.*) }
  . = ALIGN(32 / 8);
  .data           :
  {
    *(.data .data.* .gnu.linkonce.d.*)
      }
  .bss            :  {
    *(.bss .bss.* .gnu.linkonce.b.*)
   *(COMMON)
   . = ALIGN(32 / 8);
  }
  . = ALIGN(32 / 8);
  _end = .;
  PROVIDE (end = .);
}
