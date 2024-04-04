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
 * file : drivers/ide.c                                                    *
 * First Hard Disk (IDE) driver                                            *
 ***************************************************************************

 ***************************************************************************
 * $Id: ide.c,v 1.22 2001/08/23 19:21:11 iurlano Exp $
 ***************************************************************************
 *
 * $Log: ide.c,v $
 * Revision 1.22  2001/08/23 19:21:11  iurlano
 * Changed IDE to publish correctly each partition, whole disk included.
 * Added mount point support to vfs. Extended its protocol.
 * Changed fat driver to accept two mandatory parameters: device (the partitions with the fs) and mount (the mount point)
 * Now it is possible to mount a partition and actually see it int the FS!!
 * Works!
 * Changed fs.c in libOSSO to use the vfs for dir and file access.
 * Changed mount utility to inspect partitions and mount points state.
 * Added Documentation for FAT driver
 *
 * Revision 1.21  2001/08/22 14:19:26  iurlano
 * Added partition support to the FAT driver. Tested with hda3 and hda4 and seems working
 *
 * Revision 1.20  2001/08/09 10:37:53  iurlano
 * Change Creat_thread to accept an integer parameter to be passed to the thread.
 * corrected exit_thread behaviour.
 * Added format routines to the ide (WARNING VERY DANGEROUS and NOT working)
 *
 * Revision 1.19  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.18  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.17  2001/07/08 11:12:50  steros
 * Converted MPS to dynamic allocation and other little fix.
 *
 * Revision 1.16  2001/07/01 15:17:32  iurlano
 * Added first prototype of partition management system
 * Added mount program
 *
 * Revision 1.15  2001/06/16 11:35:21  steros
 * Added dir scanning functions. Fixed various bugs.
 *
 * Revision 1.14  2001/06/10 22:46:37  iurlano
 * Added hiworld program to xconfig panel
 *
 * Revision 1.13  2001/06/07 15:41:05  iurlano
 * Added geometry parameter to ide driver
 * Added Documentation file
 *
 * Revision 1.12  2001/06/03 17:54:51  steros
 * Fat restructuring.
 *
 * Revision 1.11  2001/06/02 12:58:27  iurlano
 * Changed ide driver to support getting geometry from the hard disk
 *
 * Revision 1.10  2001/05/30 20:07:48  iurlano
 * Changed IDE driver to work with event handling. Work prefectly
 *
 * Revision 1.9  2001/05/02 18:50:27  steros
 * Driver dependancies automation implemented.
 *
 * Revision 1.8  2001/04/29 17:40:48  steros
 * Added dynamic mps pid request on drv2mps also.
 * Improved mps and his protocols.
 * There's still problems in taking MPS pid from process MPS_Pid variable.
 *
 * Revision 1.7  2001/04/22 13:17:38  iurlano
 * Changed mpstest
 * mps now working
 * integrated some code for pid asking in the libosso
 *
 * Revision 1.6  2001/04/20 15:30:59  iurlano
 * Changes in to the fat driver
 * creating the filesystem protocol
 *
 * Revision 1.5  2001/04/19 15:17:58  iurlano
 * Fixed some bug
 * changed ide driver
 * changed fat driver (cabled the pid of the ide driver)
 * changed shell( now accept 3 commands mem, info, ls) TRY IT
 * VERY STABLE SNAPSHOT
 *
 * Revision 1.4  2000/08/25 13:39:37  iurlano
 * Added CHANGELOG
 * more translations
 *
 * Revision 1.3  2000/08/20 16:46:06  iurlano
 * Fixed bug in init in modules loading
 * IDE driver test. Doesn't work with my Quantum Fireball 4.3 Gb but does
 *   work with my Conner 42MB
 * More translation
 * Fixed some bad comments
 * Changed paging.c for a test (see the comments there)
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:00  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/


/* First hard disk driver prototype */

#include <io.h>
#include <printf.h>
#include <processes.h>
#include <semaphores.h>
#include <drivers.h>
#include <string.h>
#include <messages.h>
#include <protocols/disk.h>
#include <protocols/vfs.h>
#include <protocols/init.h>
#include <events.h>

#include <protocols/mpsint.h>
#include <protocols/drv2mps.h>

#include <drivers/protocols/mpsint.h>
#include <drivers/protocols/prg2mps.h>


int USE_BIOS_GEOMETRY=0;

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

static int hd1_cylinders;
static int hd1_heads;
static int hd1_sectors;


int total_sectors=0;
static int ret;

static PID vfs_pid;
//static int irqe=0;

SemaphoreAddress sem;

//static int irqf=0;

typedef struct __TPartition {
  int active;
  byte os;
  dword Preceeding_Sectors;
  dword Length;
} TPartition;

#define MAX_IDE_PARTITIONS 20

TPartition partitions[MAX_IDE_PARTITIONS];


void Exit(void) {
  DriverExit();
  exit();
}


void delay(void) {
  int t;
  for (t=0;t<1000;t++) __asm__("nop");
}

void get_BIOS_geometry(void) {
  word * tmp2;
  int hd0_c,hd0_s,hd0_h,total_sec;

  tmp2=(word *) 0x600;
  hd0_c=(((tmp2[2]>>8)&0xFF)|((tmp2[2]<<2)&0x300)) +1;
  hd0_s=(tmp2[2]&63);
  hd0_h=((tmp2[3]>>8)&0xFF )+ 1;
  total_sec=(hd0_cylinders+1)*(hd0_heads)*(hd0_sectors);

  hd0_cylinders=hd0_c;
  hd0_sectors=hd0_s;
  hd0_heads=hd0_h;
  total_sectors=total_sec;

  printf("BIOS Reported Geometry : \n");
  printf("Status : %x \n",tmp2[0]);
  printf("Cyilinders : 0 - %d \n",hd0_c);
  printf("Sectors    : 1 - %d \n",hd0_s);
  printf("Heads      : 0 - %d \n",hd0_h-1);
  printf("Total sectors : %u (%u MB)\n",total_sec,total_sec/2048);
}

int is_ready(void) {
  if ((inb(HD0_STATUS)&(HD_BUSY/*|HD_DRQ*/))==0) return 1;
  return 0;
}

int reset_controller(void) {
  int t;
  //  printf("[IDE] reset controller\n");
  outb(4,HD0_STAT);
  for (t=0;t<10000;t++) __asm__ ("nop");
  outb(0,HD0_STAT);
  for (t=0;t<10000;t++) if (is_ready()) break;
  //  printf("[IDE] reset controller END\n");
  if (!is_ready()) return 1;
  return 0; /* OK */
}

int select_device(unsigned int device) {
  int t;
  //  printf("[IDE] select device\n");
  if (device>1) return 0;
  for (t=0;t<30000;t++) if (is_ready()) break;
  if (!is_ready()) return 0;
//  printf("WRITING %x\n",(0x80|0x20|(device<<4)));
  outb((0x80|0x20|(device<<4)),HD0_DRIVE);
  for (t=0;t<30000;t++) if (is_ready()) break;

  if (!is_ready()) return 0;
  return 1;
}

int read_sector_hw(unsigned int device,unsigned int cyl,unsigned int head,unsigned int sec,char * dat) {
  PID t;
  int status;
  word *data=(word *) dat;

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
// while (irqe==0) ; irqe--;
  //  P(sem);
  ret=wait((EVENT)"INTERRUPT14",&t);
  if (ret!=0) {
    printf("Error waiting INTERRUPT14 event (%d)\n",ret);
    Exit();
  }

  status=inb(HD0_STATUS);
  if (status&HD_ERROR) printf("Read Sector Error : %x\n",inb(HD0_ERROR));
  else {
    for (t=0;t<256;t++) data[t]=inw(HD0_DATA);
    status=inb(HD0_STAT);
    status=inb(HD0_STATUS);
    /*
      printf("Read Sector. Controller 0: %x (error %x)\n",
      inb(HD0_STATUS),inb(HD0_ERROR));
    */
    if (status&HD_ERROR) printf("Read Sector Error : %x\n",inb(HD0_ERROR));
    if (status&HD_ERROR) return 0; else return 1;
  }
  return 0;
}

int write_sector_hw(unsigned int device, unsigned int cyl, unsigned int head,
		    unsigned int sec,char * dat) {
  PID t;
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
//  while (((inb(HD0_STATUS)&(HD_BUSY|HD_DRQ))!=HD_DRQ)||irqe!=0) ;
//  if (irqe!=0) {
//    printf("Write Sectors Error : %x\n",inb(HD0_ERROR));
//    return 0;
//  }
  //  port_write(HD0_DATA,data,256);
  for (t=0;t<256;t++) outw(data[t],HD0_DATA);
  //  while (irqe==0) ; irqe--;
  //P(sem);
  ret=wait((EVENT)"INTERRUPT14",&t);
  if (ret!=0) {
    printf("Error waiting INTERRUPT14 event (%d)\n",ret);
    Exit();
  }

  status=inb(HD0_STATUS);
  printf("Write Sector Done. Controller 0: %x (error %x)\n",
	 inb(HD0_STATUS),inb(HD0_ERROR));

  if (status&HD_ERROR) return 0;
  return 1;
}

int format_sector_hw(unsigned int device, unsigned int cyl, unsigned int head,
		     unsigned int sec) {
  int j;
  PID t;
  int status;
  word data[256];

  for (t=0;t<256;t++) data[t]=0;

  printf("Format sect called ...\n");
  for (t=0;t<10000;t++) if (is_ready()) break;
  if (!is_ready()) return 0;
  if (!select_device(device)) return 0;
  outb(63,HD0_SECTOR_COUNT);
  outb(sec,HD0_SECTOR_NUMBER);
  outb((cyl & 0xFF),HD0_CYLINDER_LOW);
  outb((cyl >> 8),HD0_CYLINDER_HIGH);
  outb((0x80|0x20|(device<<4)|head),HD0_DRIVE);
  outb(HDC_FORMAT_TRACK,HD0_COMMAND);
  delay();

  for (j=0;j<63;j++)
    for (t=0;t<256;t++) outw(data[t],HD0_DATA);
  //  while (((inb(HD0_STATUS)&(HD_BUSY|HD_DRQ))!=HD_DRQ)||irqe!=0) ;
  //  if (irqe!=0) {
  //    printf("Write Sectors Error : %x\n",inb(HD0_ERROR));
  //    return 0;
  //  }
  //  port_write(HD0_DATA,data,256);

  
  ret=wait((EVENT)"INTERRUPT14",&t);
  if (ret!=0) {
    printf("Error waiting INTERRUPT14 event (%d)\n",ret);
    exit();
  }

  status=inb(HD0_STATUS);
  printf("Write Sector Done. Controller 0: 0x%x (error 0x%x)\n",
	 inb(HD0_STATUS),inb(HD0_ERROR));

  if (status&HD_ERROR) return 0;
  return 1;
}


int read_sector(unsigned int device,unsigned int logical_sec,char * data) {
  unsigned int cyl,head,sec;
  unsigned int logical_sector=logical_sec;

  //  printf("Read Sectors...\n");
  sec=logical_sector%hd0_sectors + 1;
  logical_sector/=hd0_sectors;
  head=logical_sector%hd0_heads;
  logical_sector/=hd0_heads;
  cyl=logical_sector;
  //  printf("Logical sector %u translated into %u:%u:%u\n",logical_sec,cyl,head,sec);
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
  //printf("Logical sector %u translated into %u:%u:%u\n",tmp,cyl,head,sec);
  return write_sector_hw(device,cyl,head,sec,data);
}

void check_hd(int device) {
  word data[512];

  PID t;

  printf("[IDE] : STARTING hd check\n");
  t=8;
  while (t>0) {
    t--;
    if (reset_controller()) printf("Controller BUSY\n");
    if (!select_device(device)) printf("ERROR SELECT DEVICE\n");
    else {
      printf("[IDE] : Retrying controller reset\n");
      t=0;
    }
  }
  printf("[IDE] : Controller reset end\n");

  //  printf("controller 0: %x (error %x), controller 1: %x\n",
  //	 inb(HD0_STATUS),inb(HD0_ERROR),inb(0x177));
  //  outb(HDC_DIAGNOSTICS,HD0_COMMAND);
  //  printf("Wating for IRQ\n");
  //  while (irqe==0) ; irqe--;
  //  P(sem);
  printf("[IDE] : IRQ received\n");
  if (!select_device(device)) printf("ERROR SELECT DEVICE\n");
  outb(0x10,HD0_COMMAND);
  //  while (irqe==0) ; irqe--;
  //  P(sem);
  ret=wait((EVENT)"INTERRUPT14",&t);
  if (ret!=0) {
    printf("Error waiting INTERRUPT14 event (%d)\n",ret);
    Exit();
  }

  t=inb(HD0_STATUS);
  t=inb(HD0_STAT);
  //  printf("controller 0: %x (error %x), controller 1: %x\n",
  //	 inb(HD0_STATUS),inb(HD0_ERROR),inb(0x177));

  if (!select_device(device)) printf("ERROR SELECT DEVICE\n");
  //  printf("Waiting for controller\n");
  //  printf("controller 0: %x (error %x), controller 1: %x\n",
  //	 inb(HD0_STATUS),inb(HD0_ERROR),inb(0x177));
  while (!is_ready())  ;
  
  
  outb(HDC_IDENTIFY_DRIVE,HD0_COMMAND);
  //  printf("Waiting for data\n");

  //  while (irqe==0) ; irqe--;
  //  P(sem);
  ret=wait((EVENT)"INTERRUPT14",&t);
  if (ret!=0) {
    printf("Error waiting INTERRUPT14 event (%d)\n",ret);
    Exit();
  }

  //  printf("controller 0: %x (error %x), controller 1: %x\n",
  //	 inb(0x1F7),inb(0x1F1),inb(0x177));
  //  port_read(HD0_DATA,data,256);
  for (t=0;t<256;t++) data[t]=inw(HD0_DATA);
  if ((data[0] &0x8000)==1) printf("ATAPI device\n"); 
  else printf("ATA device\n");
  
  if (data[10]==0) printf("No Serial number\n");
  else {
    printf("Serial Number : ");
    for (t=10;t<20;t++) printf("%c%c",data[t]>>8,data[t]&0xFF);
    printf("\n");
  }
  
  if (data[0x2E/2]==0) printf("No Firmware Revision\n");
  else {
    printf("Firmware Revision : ");
    for (t=0x2E/2;t<4+0x2E/2;t++) printf("%c%c",data[t]>>8,data[t]&0xFF);
    printf("\n");
  }

  if (data[0x36/2]==0) printf("No Model number\n");
  else {
    printf("Model Number : ");
    for (t=0x36/2;t<20+0x36/2;t++) printf("%c%c",data[t]>>8,data[t]&0xFF);
    printf("\n");
  }
  
  printf("Physical Cylinders : %u, Heads : %u, Sectors : %u\n",
	 data[1],data[3],data[6]);
  printf("Logical Cylinders : %u, Heads : %u, Sectors : %u\n",
	 data[54],data[55],data[56]);
  if (USE_BIOS_GEOMETRY==0) {
    if (device==0) {
      hd0_sectors=data[56];
      hd0_heads=data[55];
      hd0_cylinders=data[54];
    } else {
      hd1_sectors=data[56];
      hd1_heads=data[55];
      hd1_cylinders=data[54];

    }
  }
}

char parttable[1024];

#include <protocols/drv2mps.h>

// Parameters definitions

int max_parts=0;

#define PARAM_GEOMETRY 1

int main(void) {
  MpsRetVal ret;
  char *params;
  char * msg;
  DISK_COMMAND * answ;
  msginfo info;

  int i;

  int parameter;
  PartTableRec * ptable;



  for (i=0;i<MAX_IDE_PARTITIONS;i++)
    partitions[i].active=0;
  
  /* Start initialization. */
  if ((ret=DriverInit(&params))!=0) {
    printf("[IDE] ***** Error in initialization phase: %s!!!\n",
	   MpsStrerror(ret));
    exit();
  }

  /* Parameters processing. */
  while (*params) {
    parameter=0;
    // <--- Here params contains the name of parameter ...

    if (strcasecmp("geometry",params)==0) {
      parameter=PARAM_GEOMETRY;
    } else {
      printf("Unknown Parameter ( %s ) \n",params);
    }
    while (*params++) ;
    switch (parameter) {
    case PARAM_GEOMETRY:
      if (strcasecmp("bios",params)==0) {
        USE_BIOS_GEOMETRY=1;
      } else if (strcasecmp("disk",params)==0) {
        USE_BIOS_GEOMETRY=0;
      } else {
        printf("Unknoun value ( %s ) for parameter geometry\n",params);
      }
    }
    // <--- Here params contains the value of parameter. It can be 0 ...
    //printf("'%s'\n",params);
    while (*params++) ;
  }

  /* PROCESS PARAM STRING */

  //  sem=get_HW_Sem(0xE);
  ret=getevent("INTERRUPT14");
  if (ret!=0) {
    printf("[IDE] Error getting INTERRUPT14 event\n");
    exit();
  }
  printf("IDE Driver (pid==%d)\n",getpid());
  //printf("Semaforo %x\n",sem);
  printf("Probing channel 0, drive 0...\n");
  if (USE_BIOS_GEOMETRY==1)
    get_BIOS_geometry();
  check_hd(0);
  /*
    printf("Starting format\n");
    for (ddi=0;i<10;i++) {
    printf("Formatting %d\n");
    format_sector_hw(0,i,0,1);
    }
  */  
  printf("Finished checking HD...\n");
  {
    int pi;
    char nome[20];
    int starts,ends;


    if (read_sector(0,0,&parttable[0])==0) { 
      printf("Boot Sector Read ERROR!\n"); 
    } 
    printf("Sector 0 read\n");
    {
      MpsRetVal errcode;
      vfs_message * vmsg,* vans;
      msginfo info;

      errcode=GetDriverPid("system","vfs",&vfs_pid,MPS_WAITRUN);
      if (errcode || (vfs_pid==0)) {
        printf("[IDE] ***** Error in getting VFS pid: %s!!!\n",
	       MpsStrerror(errcode));
        exit();
      }

      vmsg=getbmsg();
      vmsg->class=VFS_PUBLISH_PARTITION;
      strcpy(vmsg->name,"hda");
      vmsg->tag=0;

      partitions[0].active=1;
      partitions[0].Preceeding_Sectors=0;
      partitions[0].Length=0;
      partitions[0].os=0;

      send_msg(vfs_pid,vmsg,sizeof(*vmsg));
      vans=recv_msg(&info);
      if (vans->class==VFS_ERROR) 
	printf("VFS_ERROR\n");
      freemsg(vans);

      ptable=(PartTableRec*)(parttable+0x1BE);
      for (pi=0;pi<4;pi++) {

	if (ptable[pi].os!=0) {
	  printf("Partition Found at index %d\n",pi+1);
	  vmsg=getbmsg();
	  vmsg->class=VFS_PUBLISH_PARTITION;
	  sprintf(vmsg->name,"hda%d",pi+1);
	  vmsg->tag=pi+1;

	  partitions[pi+1].active=1;
	  partitions[pi+1].Preceeding_Sectors=ptable[pi].Preceeding_Sectors;
	  partitions[pi+1].Length=ptable[pi].Length;
	  partitions[pi+1].os=ptable[pi].os;

	  printf("  start %d, end %d\n",partitions[pi+1].Preceeding_Sectors,
		 partitions[pi+1].Length+partitions[pi+1].Preceeding_Sectors);
	  send_msg(vfs_pid,vmsg,sizeof(*vmsg));
	  vans=recv_msg(&info);
	  if (vans->class==VFS_ERROR) 
	    printf("VFS_ERROR\n");
	  /*
	    if (vans->class==VFS_SUCCESS)
	    printf("VFS_SUCCESS\n");
	  */
	}
      }    
    }
  }

  /* This ends initialization phase. */
  if ((ret=DriverStart("hd_device","ide",DRV_I_MONO,0,1,
		       "This driver tries to support the hd ATA interface"))!=0) {
    printf("[IDE] ***** ERROR: MPS don't allow me to register: %s!!!\n",
	   MpsStrerror(ret));
    exit();
  }

  /*
    msg=getbmsg();
    ((INIT_COMMAND*)msg)->command=INIT_FINISHED;
    send_msg(INIT,msg,sizeof(INIT_COMMAND));
  */
  while (1) {
    //    printf("Waiting message (zzzz)....\n");
    msg=recv_msg(&info);
    //printf("GOT MESSAGE!\n");
    while (1) {
      answ=getbmsg();
      *(answ)=*((DISK_COMMAND*)msg);
      answ->next_cmd=0;
      switch (((DISK_COMMAND*)msg)->command) {
      case DSK_CMD_READ_SECTOR:         /* Logical Sector Read */
	answ->buffer_dim=512;
	answ->next_cmd=0;
	read_sector(answ->arg1,answ->arg2+partitions[answ->tag].Preceeding_Sectors,((char*)answ)+sizeof(DISK_COMMAND));
	send_msg(info.mittente,answ,sizeof(DISK_COMMAND)+512);
	break;
      case DSK_CMD_WRITE_SECTOR:        /* Logical Sector Write */
	answ->buffer_dim=0;
	answ->next_cmd=0;
	write_sector(answ->arg1,answ->arg2+partitions[answ->tag].Preceeding_Sectors,((char*)answ)+sizeof(DISK_COMMAND));
	send_msg(info.mittente,answ,sizeof(DISK_COMMAND));
	break;
      }
      if (((DISK_COMMAND*)msg)->next_cmd==0) break;
      else msg=msg+(((DISK_COMMAND*)msg)->next_cmd);
    }
    freemsg(msg);
  }
  printf("Warning! IDE process has finished.\n");
  DriverExit();
}
