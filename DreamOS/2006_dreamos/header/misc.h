/*
 * Dreamos
 * misc.h
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
 * Author: Lethalman
 */

// Eflags
unsigned int get_eflags(); //Return the eflags value
void set_eflags(int eflags); //Set the eflags value
unsigned int chk_eflags(int flag); //Check if the flag is set or cleared
enum {
  EFLAG_CF = 1<<0, // Carry Flag (Status)
  EFLAG_PF = 1<<2, // Parity Flag (Status)
  EFLAG_AF = 1<<4, // Auxiliary Carry Flag (Status)
  EFLAG_ZF = 1<<6, // Zero Flag (Status)
  EFLAG_SF = 1<<7, // Sign Flag (Status)
  EFLAG_TF = 1<<8, // Trap Flag (System)
  EFLAG_IF = 1<<9, // Interrupt Enable Flag (System)
  EFLAG_DF = 1<<10, // Direction Flag (Control)
  EFLAG_OF = 1<<11, // Overflow Flag (Status)
  EFLAG_IOPL1 = 1<<12, // I/O Privilege Level 1 (System)
  EFLAG_IOPL2 = 1<<13, // I/O Privilege Level 2 (System)
  EFLAG_NT = 1<<14, // Nested Task (System)
  EFLAG_RF = 1<<16, // Resume Flag (System)
  EFLAG_VM = 1<<17, // Virtual-8086 Mode (System)
  EFLAG_AC = 1<<18, // Alignment Check (System)
  EFLAG_VIF = 1<<19, // Virtual Interrupt Flag (System)
  EFLAG_VIP = 1<<20, // Virtual Interrupt Pending (System)
  EFLAG_ID = 1<<21, // ID Flag (System)
};

// Messages
void _kerror(char *error); //Write a simple error message
void _kwarning(char *warning); //Write a simple warning message
