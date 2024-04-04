
/*
 * OSSO - A Micro Kernel OS
 * Copyright (c) 2000 Alessandro Iurlano.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <printf.h>
#include <processes.h>
#include <signals.h>
#include <memory.h>
#include <semaphores.h>
#include <drivers.h>
#include <types.h>


#include "keyscancodes.h"

extern int abs_y; /* printk.c */
extern Last_Pid_Used;
/* Vettore con gli stati dei caratteri */
unsigned char key_states[256]="";

/* Stato dei tasti modificatori */
static int shift=0;
static int control=0;
static int alt=0;

/* Stato dei led */
static int NumLock=1;
static int ScrollLock=0;
static int CapsLock=0;


byte inb(word port) { 
  unsigned char _v; 
  __asm__ volatile ("inb %%dx,%%al":"=a" (_v):"d" (port)); 
  return _v;
}

word inw(word port) { 
  unsigned short int _v; 
  __asm__ volatile ("inw %%dx,%%ax":"=a" (_v):"d" (port)); 
  return _v;
}

void outb(byte value, word port) {
  __asm__ ("outb %%al,%%dx"::"a" (value),"d" (port));
  return ;
}

void outw(word value, word port) {
  __asm__ ("outw %%ax,%%dx"::"a" (value),"d" (port));
  return ;
}

void UpdateKbdLocks(void) {
  unsigned char data;
  data=(ScrollLock)|(NumLock<<1)|(CapsLock<<2);

  outb(0xed,0x60);
  while (inb(0x64) & 02) ;
  outb(data,0x60);
  while (inb(0x64) & 02) ;
}


static int KeyPause=0;
static int KeyStampa=0;
static int extended_key=0;

int main(void) {
  SemaphoreAddress sem;
  unsigned char a,kstat;
  unsigned char keypressed;

  printf("Driver Test!\n");
  
  sem=get_HW_Sem(1);
  while (1) {
//    printf("Attesa di P\n");
    P(sem);
    keypressed=0;
    kstat=inb(0x64); /* Disponibilita' di un dato da leggere */
    if (kstat &1) {
      a=inb(0x60); /* Lettura scancode */
      if (a==0xE0) { /* Scancode esteso */
	extended_key=1;
      } 
      else if (a!=0xFA) { /* 0xFA == Acknowledge dei comandi */
	if (a & 128) key_states[(a & 127)+128*extended_key]=0;
	else key_states[(a & 127)+128*extended_key]=1;

	shift=key_states[kscn_Left_Shift] | key_states[kscn_Right_Shift];
	control=key_states[kscn_Left_Control] | key_states[kscn_Right_Control];
	alt=key_states[kscn_Left_Alt] | key_states[kscn_Right_Alt];

	/* Tasto premuto: */
	switch (a) {
	case 69:
	  NumLock=1-NumLock;
	  UpdateKbdLocks();
	  a=0;
	  break;
	case 70:
	  ScrollLock=1-ScrollLock;
	  UpdateKbdLocks();
	  a=0;
	  break;
	case 58:
	  CapsLock=1-CapsLock;
	  UpdateKbdLocks();
	  a=0;
	  break;
	default:
	  if (extended_key==0) {
	    if (a==69 && KeyPause==2) {
	      KeyPause=0;
	      keypressed=0;
	      printf("PAUSA\n");
	    } else if (a==29 && KeyPause==1) {
	      keypressed=0;
	      KeyPause=2;
	    } else if (a==0xE1) {
	      KeyPause=1;
	      keypressed=0;
	    } else KeyPause=0;
	    if ((a & 128)==0) {
	      if (shift==1)
		keypressed=char_table[a & 127][1];
	      else if (control==1)
		keypressed=char_table[a & 127][2];
	      else if (alt==1)
		keypressed=char_table[a & 127][3];
	      else 
		keypressed=char_table[a & 127][0];

	    }
	  } else {
	    keypressed=0;
	    if ((a & 127)==0x37) {
	      if (KeyStampa==1) {
		printf("STAMPA!\n");
		keypressed=0;
	      }
	    }

	    if ((a & 127)==0x2A) {
	      KeyStampa=1;
	      keypressed=0;
	    } else KeyStampa=0;
	    switch (a) {
	    case 0x1C:
	      keypressed=13;
	      break;
	    case 0x2A:
	      KeyStampa=1;
	      keypressed=0;
	      break;
	    case 0x36:
	      keypressed='/';
	      break;
	    case 0x40:
	      printf("Right\n");
	      break;
	    case 0x47: {
	      int i;
	      //	      printf("Processo in esecuzione : %d \nCodaPronti == :\n",
	      //	     Processo_In_Esecuzione->pid);
	      //printf("Last Pid %d\n",Last_Pid_Used);
	      //for (i=testa_CodaPronti;i!=coda_CodaPronti;i=(i+1)%MAX_REFERENCES){
	      //	printf("Pid %d, active == %d (level==%d)\n",CodaPronti[i]->pid,
	      //	       CodaPronti[i]->active,CodaPronti[i]->level);
	    }
	    break;
	    
	    case 0x48:
	      //      printf("Pagine libere : %d; GDT Entries libere : %d\n",
	      //	     free_pages,get_free_gdt_entries());
	      break;
	    case 0x49:
	      if (shift) {
		//		word t;
		//t=get_videomem_start_offset();
		//		if (t>0) {
		//  if (t>=800) set_videomem_start_offset(t-800);
		//  else set_videomem_start_offset(0);
		//		}
	      } else
		printf("Page Up\n");
	      break;
	    case 0x4B:
	      printf("Left\n");
	      break;
	    case 0x4F:
	      printf("End\n");
	      break;
	    case 0x50:
	      printf("Down\n");
	      break;
	    case 0x51:	    
	      if (shift) {
		/* 		word t; */
		/* 		t=get_videomem_start_offset(); */
		/* 		if (t<(abs_y-24)*80) { */
		/* 		  if (t<=(abs_y-34)*80) set_videomem_start_offset(t+800); */
		/* 		  else set_videomem_start_offset((abs_y-24)*80); */
		/* 		} */
	      } else	    
		printf("Page Down\n");
	      break;
	    case 0x52:
	      printf("Insert\n");
	      break;
	    case 0x53:
	      if (shift==0 && control==1 && alt==1) {
		printf("***********REBOOT**********\n\n");
		outb(0xFE,0x64);
		outb(0x1,0x60);
		__asm__("hlt");
	      }
	      else printf("Canc\n");
	      break;

	    }
	  }
	  break;
	}
	if (keypressed!=0) printf("%c",keypressed);
	extended_key=0;
      }
    }
    else printf("No DATA from keyboard\n");


//    printf("Premuto un tasto!\n");
  }
  return 0;
}
