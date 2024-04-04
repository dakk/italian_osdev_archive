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


/******************************* O S S O ***********************************
 * file : kernel/hd.c                                                      *
 * First Hard Disk (IDE) basic functions                                   *
 ***************************************************************************

 ***************************************************************************
 * $Id: hd.c,v 1.2 2000/08/04 17:47:58 iurlano Exp $
 ***************************************************************************
 *
 * $Log: hd.c,v $
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:02  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1  2000/03/21 10:43:17  iurlano
 * Spostati files dalla dir del kernel a quella dei drivers
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:41  pecciant
 *
 *
 * Revision 1.2  1999/11/25 11:22:29  iurlano
 * Pulizia e riordino
 *
 * 
 ***************************************************************************/

/* Code for interacting with first hard disk (see ide.c for the driver) */

#include <sys.h>
#include <io.h>
#include <kernel.h>

#include <printk.h>

/* PRIMARY IDE CONTROLLER PORTS */

#define HD0_STAT 0x3F6
#define HD0_DATA 0x1F0
#define HD0_ERROR 0x1F1
#define HD0_WPC 0x1F1
#define HD0_SECTOR_COUNT 0x1F2
#define HD0_SECTOR_NUMBER 0x1F3
#define HD0_CYLINDER_LOW 0x1F4
#define HD0_CYLINDER_HIGH 0x1F5
#define HD0_DRIVE 0x1F6
#define HD0_STATUS 0x1F7
#define HD0_COMMAND 0x1F7

/* CONTROLLER STATUS BITS (HDx_STATUS)*/

#define HD_BUSY 0x80
#define HD_DREADY 0x40
#define HD_WRITE_FAULT 0x20
#define HD_SEEK 0x10
#define HD_DRQ 0x8
#define HD_CORRECTED 0x4
#define HD_INDEX 0x2
#define HD_ERROR 0x1

/* CONTROLLER COMMANDS */

#define HDC_READ_SECTORS 0x20
#define HDC_READ_SECTORS_NR 0x21           /* NR == NO RETRY */
#define HDC_WRITE_SECTORS 0x30
#define HDC_WRITE_SECTORS_NR 0x31           /* NR == NO RETRY */
#define HDC_DIAGNOSTICS 0x90
#define HDC_IDENTIFY_DRIVE 0xEC
#define HDC_FORMAT_TRACK 0x50

static int hd0_cylinders;
static int hd0_heads;
static int hd0_sectors;

int total_sectors=0;

static int irqe=0;
//static int irqf=0;

void delay(void) {
  int t;
  for (t=0;t<1000;t++) nop();
}

void IRQE(void) {
  pusha();
  irqe++;
  /*
    DEBUG
    printk("IRQ E\n");
  */
  eoi();
  popa();
  __asm__("movl %ebp,%esp\n"
          "popl %ebp\n");  
  iret();
}
void IRQF(void) {
  pusha();
  printk("IRQ F\n");
  eoi();
  popa();
  __asm__("movl %ebp,%esp\n"
          "popl %ebp\n");  
  iret();
}

void get_BIOS_geometry(void) {
  word * tmp2;
  tmp2=(word *) 0x1000;
  hd0_cylinders=(((tmp2[2]>>8)&0xFF)|((tmp2[2]<<2)&0x300)) +1;
  hd0_sectors=(tmp2[2]&63);
  hd0_heads=((tmp2[3]>>8)&0xFF )+ 1;
  
  total_sectors=(hd0_cylinders)*(hd0_heads)*(hd0_sectors);

  printk("BIOS Reported Geometry : \n");
  printk("Status : %x \n",tmp2[0]);
  printk("Cyilinders : 0 - %d \n",hd0_cylinders-1);
  printk("Sectors    : 1 - %d \n",hd0_sectors);
  printk("Heads      : 0 - %d \n",hd0_heads-1);
  printk("Total sectors : %u (%u MB)\n",total_sectors,total_sectors/2048);
}

int is_ready(void) {
  if ((inb(HD0_STATUS)&(HD_BUSY|HD_DRQ))==0) return 1;
  return 0;
}

int reset_controller(void) {
  int t;
  outb(4,HD0_STAT);
  for (t=0;t<10000;t++) nop();
  outb(0,HD0_STAT);
  for (t=0;t<10000;t++) if (is_ready()) break;
  if (!is_ready()) return 1;
  return 0; /* OK */
}

int select_device(unsigned int device) {
  int t;
  if (device>1) return 0;
  for (t=0;t<10000;t++) if (is_ready()) break;
  if (!is_ready()) return 0;
  //  printk("WRITING %x\n",(0x80|0x20|(device<<4)));
  outb((0x80|0x20|(device<<4)),HD0_DRIVE);
  for (t=0;t<10000;t++) if (is_ready()) break;
  if (!is_ready()) return 0;
  return 1;
}

int read_sector_hw(unsigned int device,unsigned int cyl,unsigned int head,unsigned int sec,char * dat) {
  int t;
  int status;
  word * data;
  data=(word *) dat;
  for (t=0;t<10000;t++) if (is_ready()) break;
  if (!is_ready()) return 0;
  if (!select_device(device)) return 0;
  outb(1,HD0_SECTOR_COUNT);
  outb(sec,HD0_SECTOR_NUMBER);
  outb((cyl & 0xFF),HD0_CYLINDER_LOW);
  outb((cyl >> 8),HD0_CYLINDER_HIGH);
  outb((0x80|0x20|(device<<4)|head),HD0_DRIVE);
  outb(HDC_READ_SECTORS,HD0_COMMAND);
  delay();
  while (irqe==0) ; irqe--;
  status=inb(HD0_STATUS);
  if (status&HD_ERROR) printk("Read Sector Error : %x\n",inb(HD0_ERROR));
  else {
    for (t=0;t<256;t++) data[t]=inw(HD0_DATA);
    status=inb(HD0_STAT);
    status=inb(HD0_STATUS);
    /*
    printk("Read Sector. Controller 0: %x (error %x)\n",
	   inb(HD0_STATUS),inb(HD0_ERROR));
    */
    if (status&HD_ERROR) printk("Read Sector Error : %x\n",inb(HD0_ERROR));
    if (status&HD_ERROR) return 0; else return 1;
  }
  return 0;
}

int write_sector_hw(unsigned int device, unsigned int cyl, unsigned int head,
		 unsigned int sec,char * dat) {
  int t;
  int status;
  word * data;
  data=(word *) dat;

  for (t=0;t<10000;t++) if (is_ready()) break;
  if (!is_ready()) return 0;
  if (!select_device(device)) return 0;
  outb(1,HD0_SECTOR_COUNT);
  outb(sec,HD0_SECTOR_NUMBER);
  outb((cyl & 0xFF),HD0_CYLINDER_LOW);
  outb((cyl >> 8),HD0_CYLINDER_HIGH);
  outb((0x80|0x20|(device<<4)|head),HD0_DRIVE);
  outb(HDC_WRITE_SECTORS,HD0_COMMAND);
  delay();
  while (((inb(HD0_STATUS)&(HD_BUSY|HD_DRQ))!=HD_DRQ)||irqe!=0) ;
  if (irqe!=0) {
    printk("Write Sectors Error : %x\n",inb(HD0_ERROR));
    return 0;
  }
  //  port_write(HD0_DATA,data,256);
  for (t=0;t<256;t++) outw(data[t],HD0_DATA);
  while (irqe==0) ; irqe--;
  status=inb(HD0_STATUS);
  printk("Write Sector Done. Controller 0: %x (error %x)\n",
	 inb(HD0_STATUS),inb(HD0_ERROR));

  if (status&HD_ERROR) return 0;
  return 1;
}

int read_sector(unsigned int device,unsigned int logical_sector,char * data) {
  unsigned int cyl,head,sec,tmp;
  
  tmp=logical_sector;
  sec=logical_sector%hd0_sectors + 1;
  logical_sector/=hd0_sectors;
  head=logical_sector%hd0_heads;
  logical_sector/=hd0_heads;
  cyl=logical_sector;
  //  printk("Logical sector %u translated into %u:%u:%u\n",tmp,cyl,head,sec);
  return read_sector_hw(device,cyl,head,sec,data);
}

int write_sector(unsigned int device,unsigned int logical_sector,char * data) {
  unsigned int cyl,head,sec,tmp;

  tmp=logical_sector;
  sec=logical_sector%hd0_sectors + 1;
  logical_sector/=hd0_sectors;
  head=logical_sector%hd0_heads;
  logical_sector/=hd0_heads;
  cyl=logical_sector;
  //printk("Logical sector %u translated into %u:%u:%u\n",tmp,cyl,head,sec);
  return write_sector_hw(device,cyl,head,sec,data);
}

void check_hd(void) {
  word data[512];
  
  int t;
  if (reset_controller()) printk("Controller BUSY\n");
  if (!select_device(0)) printk("ERROR SELECT DEVICE\n");
  printk("controller 0: %x (error %x), controller 1: %x\n",
	 inb(HD0_STATUS),inb(HD0_ERROR),inb(0x177));
  outb(HDC_DIAGNOSTICS,HD0_COMMAND);
  printk("Wating for IRQ\n");
  while (irqe==0) ; irqe--;
  if (!select_device(0)) printk("ERROR SELECT DEVICE\n");
  outb(0x10,HD0_COMMAND);
  while (irqe==0) ; irqe--;
  t=inb(HD0_STATUS);
  t=inb(HD0_STAT);
  printk("controller 0: %x (error %x), controller 1: %x\n",
	 inb(HD0_STATUS),inb(HD0_ERROR),inb(0x177));

  if (!select_device(0)) printk("ERROR SELECT DEVICE\n");
  printk("Waiting for controller\n");
  printk("controller 0: %x (error %x), controller 1: %x\n",
	 inb(HD0_STATUS),inb(HD0_ERROR),inb(0x177));
  while (!is_ready())  ;
  
  
  outb(HDC_IDENTIFY_DRIVE,HD0_COMMAND);
  printk("Waiting for data\n");

  while (irqe==0) ; irqe--;
  printk("controller 0: %x (error %x), controller 1: %x\n",
	 inb(0x1F7),inb(0x1F1),inb(0x177));
  //  port_read(HD0_DATA,data,256);
  for (t=0;t<256;t++) data[t]=inw(HD0_DATA);
  if ((data[0] &0x8000)==1) printk("ATAPI device\n"); 
  else printk("ATA device\n");
  
  if (data[10]==0) printk("No Serial number\n");
  else {
    printk("Serial Number : ");
    for (t=10;t<20;t++) printk("%c%c",data[t]>>8,data[t]&0xFF);
    printk("\n");
  }
  
  if (data[0x2E/2]==0) printk("No Firmware Revision\n");
  else {
    printk("Firmware Revision : ");
    for (t=0x2E/2;t<4+0x2E/2;t++) printk("%c%c",data[t]>>8,data[t]&0xFF);
    printk("\n");
  }

  if (data[0x36/2]==0) printk("No Model number\n");
  else {
    printk("Model Number : ");
    for (t=0x36/2;t<20+0x36/2;t++) printk("%c%c",data[t]>>8,data[t]&0xFF);
    printk("\n");
  }
  
  printk("Physical Cylinders : %u, Heads : %u, Sectors : %u\n",
	 data[1],data[3],data[6]);
  /*
  printk("Validity : first : %d second : %d\n",data[0x6A/2]&1,data[0x6A/2]&2);
  printk("Current translation mode: Cylinders : %u, Heads : %u, Sectors : %u\n"
  ,data[54],data[55],data[56]);
  printk("LBA mode supported : %d ATA Version : %x:%x\n",
	 data[0x62/2]&(1<<9),data[80],data[81]);
  */
}


