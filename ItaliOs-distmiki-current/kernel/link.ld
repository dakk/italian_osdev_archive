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

/*OUTPUT_FORMAT("elf32-i386")*/
OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
ENTRY(_start)
SECTIONS
{
  . = 0x10000;
   
  .text           :  { *(.text .stub .text.* .gnu.linkonce.t.*)  }
  PROVIDE (etext = .);   
  .rodata         : { *(.rodata .rodata.* .gnu.linkonce.r.*) }
  . = ALIGN(32 / 8);
  .data           :
  {


  /*costruttori del c++*/
           __CTOR_LIST__ = .; LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2) *(.ctors)   LONG(0) __CTOR_END__ = .;

        __DTOR_LIST__ = .; LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2) *(.dtors) LONG(0) __DTOR_END__ = .;

    *(.data .data.* .gnu.linkonce.d.*)
    SORT(CONSTRUCTORS)
  }
  .ctors          :
  {
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  }
  .dtors          :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
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
