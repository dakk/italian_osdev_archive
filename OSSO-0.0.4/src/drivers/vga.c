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


/****************************** O S S O  ***********************************
 * file : drivers/vga.c
 * Description: some code for vga control
 ***************************************************************************

 ***************************************************************************
 * $Id: vga.c,v 1.3 2001/04/15 14:15:43 iurlano Exp $
 ***************************************************************************
 *
 * $Log: vga.c,v $
 * Revision 1.3  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.2  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.1  2001/02/07 21:28:40  iurlano
 * added vga driver prototype
 *
 *
 ***************************************************************************/


#include <io.h>
#include <printf.h>
#include <processes.h>
#include <semaphores.h>
#include <drivers.h>
#include <messages.h>
#include <protocols/disk.h>
#include <protocols/init.h>
//#include <devices.h>
#include <pci.h>
#include <types.h>

#define SEQ_ADDR                 0x03C4
#define GRACON_ADDR              0x03CE
#define CRTC_ADDR                0x03D4

// VGA register port addresses
#define ATTRCON_ADDR             0x03C0
#define MISC_ADDR                0x03C2
#define VGAENABLE_ADDR           0x03C3
#define SEQ_ADDR                 0x03C4
#define GRACON_ADDR              0x03CE
#define CRTC_ADDR                0x03D4
#define STATUS_ADDR              0x03DA



unsigned char modeC4[62] = {
  // MISC reg,  STATUS reg,    SEQ regs
  0x63,      0x00,          0x03,0x01,0x0F,0x00,0x06,
  // CRTC regs
  0x5F,0x4F,0x50,0x82,0x54,0x80,0x0BF,0x1F,0x00,0x41,0x00,0x00,0x00,0x00,0x00,0x00
  ,
  0x9C,0x0E,0x8F,0x28,0x00,0x96,0xB9,0xE3,0xFF,
  // GRAPHICS regs
  0x00,0x00,0x00,0x00,0x00,0x40,0x05,0x0F,0xFF,
  // ATTRIBUTE CONTROLLER regs
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
  0x41,0x00,0x0F,0x00,0x00
};

unsigned char mode12h[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0xE3,     0x00,          0x03,0x01,0x0F,0x00,0x06,
// CRTC regs
0x5F,0x4F,0x50,0x82,0x54,0x80,0x0B,0x3E,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x59,
0xEA,0x8C,0x0DF,0x28,0x00,0x0E7,0x04,0xE3,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x0F,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
0x01,0x00,0x0F,0x00,0x00
};

unsigned char mode13h[62] = {
  // MISC reg,  STATUS reg,    SEQ regs
  0x63,      0x00,          0x03,0x01,0x0F,0x00,0x0E,
  // CRTC regs
  0x5F,0x4F,0x50,0x82,0x54,0x80,0x0BF,0x1F,0x00,0x41,0x00,0x00,0x00,0x00,0x00,
  0x00,0x9C,0x0E,0x8F,0x28,0x40,0x96,0xB9,0x0E3,0xFF,
  // GRAPHICS regs
  0x00,0x00,0x00,0x00,0x00,0x40,0x05,0x0F,0xFF,
  // ATTRIBUTE CONTROLLER regs
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
  0x41,0x00,0x0F,0x00,0x00
};

#define DELAY() for(k=0;k<50;k++) __asm__("nop")

void SetMode(byte * registers) {

  int i,j,k;
  
  i=0;
  
  //****  regs in eax
  
  outb(registers[i++],MISC_ADDR);
  DELAY();
  outb(registers[i++],STATUS_ADDR);
  DELAY();
  for (j=0;j<5;j++) {
    outb(j,SEQ_ADDR);
    DELAY();
    outb(registers[i++],SEQ_ADDR+1);
    DELAY();
  }
  
  outb(0x11,CRTC_ADDR);
  DELAY();
  outb(0xE,CRTC_ADDR+1);
  DELAY();
  for (j=0;j<25;j++) {
    outb(j,CRTC_ADDR);
    DELAY();
    outb(registers[i++],CRTC_ADDR+1);
    DELAY();
  }
  
  for (j=0;j<9;j++) {
    outb(j,GRACON_ADDR);
    DELAY();
    outb(registers[i++],GRACON_ADDR+1);
    DELAY();
  }
  
  j=inb(STATUS_ADDR);
  for (j=0;j<21;j++) {
    inw(ATTRCON_ADDR);
    DELAY();
    outb(j,ATTRCON_ADDR);
    DELAY();
    outb(registers[i++],ATTRCON_ADDR);
    DELAY();
  }
  outb(0x20,ATTRCON_ADDR);
    
    
}

void setpal(int color, char r, char g, char b)
{
  __asm__ ("out %%al,%%dx\n"::"a" (color),"d" (0x3C8));
   // Send color
  __asm__ ("out %%al,%%dx\n"::"a" (r),"d" (0x3C9));
  __asm__ ("out %%al,%%dx\n"::"a" (g),"d" (0x3C9));
  __asm__ ("out %%al,%%dx\n"::"a" (b),"d" (0x3C9));


}
void setpalette4()
{
   setpal( 0,  0,  0,  0);
   setpal( 1,  0, 42, 42);
   setpal( 2, 42,  0, 42);
   setpal( 3, 63, 63, 63);
}

void setpalette256()
{
   int j = 0;
   for (j= 0; j < 64; j++)
     setpal(j, j, 0,0);

   for (j= 0; j < 64; j++)
     setpal(j+64, 0,j,0);

   for (j= 0; j < 32; j++)
     setpal(j+128, 0,0,j*2);
   for (j= 32; j < 64; j++)
     setpal(j+128, 0,0,64-2*(j-32));

   for (j= 0; j < 64; j++)
     setpal(j+192, j, j,j);
}



typedef char row[320];

char * videomem=(char*)0xA0000;

void putpixel(int x,int y, int color) {
  videomem[x+320*y]=color;
}

int main(void) {

  row * VideoMem;
  int i,j;

  printf("VGA Starting!\n");

  // MODE X  :  320 x 240 x 256

  /*
  outB(MISC_ADDR,0xE3);
  // turn off write protect
  out(CRTC_ADDR,0x2C11);
  // vertical total
  out(CRTC_ADDR,0x0D06);
  // overflow register
  out(CRTC_ADDR,0x3E07);
  // vertical retrace start
  out(CRTC_ADDR,0xEA10);
  // vertical retrace end AND wr.prot
  out(CRTC_ADDR,0xAC11);
  // vertical display enable end
  out(CRTC_ADDR,0xDF12);
  // start vertical blanking
  out(CRTC_ADDR,0xE715);
  // end vertical blanking
  out(CRTC_ADDR,0x0616);
  */

  outb(0xF,0x3D4);
  outb(0,0x3D5);
  outb(0xE,0x3D4);
  outb(0,0x3D5);



  __asm__("cli");

  SetMode(mode13h);
  setpalette256();

  VideoMem=(row *)0xA0000;

  outb(0xD,0x3D4);
  outb(0,0x3D5);
  outb(0xC,0x3D4);
  outb(0,0x3D5);
  
  
  for (i=0;i<0xFFFF;i++) videomem[i]=0;
  j=0;
  for(j=20;j<30;j++) {
    for (i=20;i<85;i++) videomem[i+320*j]=128+(j-20)*6;
    for (i=95;i<300;i++) videomem[i+320*j]=128+(j-20)*6;
  }

  for(j=20;j<30;j++) {
    for (i=20;i<j;i++) {
      videomem[i+320*j]=128+(i-20)*6;
    }      
  }
  for(j=30;j<170;j++) {
    for (i=20;i<30;i++) {
      videomem[i+320*j]=128+(i-20)*6;
    }      
  }
  for(j=170;j<180;j++) {
    for (i=20;i<30-(j-170);i++) {
      videomem[i+320*j]=128+(i-20)*6;
    }      
  }
  /*
  for(j=0;j<10;j++) {
    for (i=20;i<225;i++) videomem[i+320*(170+j)]=128+j*6;
    for (i=235;i<300;i++) videomem[i+320*(170+j)]=128+j*6;
  }
  */


  while (1);

}





