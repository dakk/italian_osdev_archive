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


/************************************ OSSO ***********************************
 *  File: kernel/PnP.c                                                       *
 *  Plug And Play & BIOS32 functions                                         *
 *****************************************************************************

 *****************************************************************************
 * $Id: PnP.c,v 1.7 2001/03/06 11:50:26 iurlano Exp $
 *****************************************************************************
 *
 * $Log: PnP.c,v $
 * Revision 1.7  2001/03/06 11:50:26  iurlano
 * Added functions for smp multitasking (not yet tested)
 *
 * Revision 1.6  2001/03/05 23:02:40  iurlano
 * Some corrections
 * Little modifies to Makefile
 *
 * Revision 1.5  2000/12/06 21:08:50  iurlano
 * Thread work in progress
 * Include files work
 * Cut some warnings
 *
 * Revision 1.4  2000/11/09 14:30:32  iurlano
 * Added pci.c module for accessing pci with configuration mechanism 1
 *    without BIOS
 *
 * Revision 1.3  2000/08/13 20:00:33  iurlano
 * Translation in progress.
 * Corrected some warnings.
 *
 * Revision 1.2  2000/07/27 12:13:07  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:08  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.2  2000/02/18 10:05:52  iurlano
 * Aggiunto codice PCI iniziale
 *
 * Revision 1.1.1.1  2000/01/19 10:42:41  pecciant
 *
 *
 * Revision 1.2  1999/11/25 11:22:29  iurlano
 * Pulizia e riordino
 *
 *
 *****************************************************************************/

// This should NOT be part of the kernel. It's here for testing purpose

/*
  Warning : Changed lcall (%%esi) in lcall *(%%esi)
  now compiles without warning, but it's not tested
*/

#include <PnP.h>
#include <string.h>
#include <printk.h>
#include <constants.h>
#include <pci.h>

void (*PnPEntry) (void);
void (*BIOS32Entry) (void);

void * PnPStructure;

typedef struct __BIOS32_H {
  char signature[4];
  void (*BSD)(void);
  byte version;
  byte header_length;
  byte checksum;
  byte reserved[5];
} BIOS_32;

typedef struct __BIOS_32_SERVICE {
  dword status;
  dword Base_Adress;
  dword Code_Seg_Size;
  dword Offset;
} BIOS_32_SERVICE;

int Check_PnP(void) {
  int address;
  address=0xF0000;
  while (address<=0xFFFF0) {
    if (strncmp((char*)address,PnP_SIGNATURE,4)==0) {
      printk("PnP found at %x\n",address);
      printk("PnP Ver : %d.%d\n",
	     ((byte*)address)[4]>>4,((byte*)address)[4]&15);
      PnPStructure=(void*)address;
      return 1;
    }
    address+=0x10;
  }
  return 0;
}


BIOS_32 * bios_32;

static struct {
  dword address;
  word segment;
} PCI32_Entry_Point;

static struct {
  dword address;
  word segment;
} PNP32_Entry_Point;

BIOS_32_SERVICE PnP;
BIOS_32_SERVICE PCI;

dword _PCI_Read_Dword(dword coords,dword regnum) {
  dword r;
  __asm__("lcall (%%esi)":
	  "=c" (r)
	  :"S" (&PCI32_Entry_Point),
	  "b" (coords),
	  "a" (0xB10A),
	  "D" (regnum));

  return r;
}

word _PCI_Read_Word(dword coords,dword regnum) {
  word r;
  __asm__("lcall (%%esi)":
	  "=c" (r)
	  :"S" (&PCI32_Entry_Point),
	  "b" (coords),
	  "a" (0xB109),
	  "D" (regnum));

  return r;
}


byte _PCI_Read_Byte(dword coords,dword regnum) {
  word r;
  __asm__("lcall (%%esi)":
	  "=c" (r)
	  :"S" (&PCI32_Entry_Point),
	  "b" (coords),
	  "a" (0xB108),
	  "D" (regnum));

  return r;
}


int Check_BIOS32(void) {
  int address;
  dword a,b,c,d,entry,result;
  dword buses,i,id,kk;
  address=0xF0000;
  while (address<=0xFFFF0) {
    if (strncmp((char*)address,"_32_",4)==0) {
      bios_32=(BIOS_32*)address;
      break;
    }
    address+=0x10;
  }
  printk("BIOS32 Service Directory found at address : %x\n",bios_32);
  
  PCI32_Entry_Point.address=(dword)bios_32->BSD;
  PCI32_Entry_Point.segment=KERNEL_CODE_SEGMENT_SEL;
  PNP32_Entry_Point.address=(dword)bios_32->BSD;
  PNP32_Entry_Point.segment=KERNEL_CODE_SEGMENT_SEL;
  __asm__ ("lcall (%%edi)":
	   "=a" (PCI.status),
	   "=b" (PCI.Base_Adress),
	   "=c" (PCI.Code_Seg_Size),
	   "=d" (PCI.Offset):
	   "a" (0x49435024), "b" (0),"D" (&PCI32_Entry_Point));
  printk("Result %x; base %x;\n",PCI.status,PCI.Base_Adress);
  PCI32_Entry_Point.address=PCI.Base_Adress+PCI.Offset;
  printk("PCI BIOS Installation check: ");
  __asm__ ("lcall (%%esi)":
	   "=a" (a),
	   "=b" (b),
	   "=c" (c),
	   "=d" (d),
	   "=D" (entry)
	   :
	   "a" (0xB101),
	   "D" (0),
	   "S" (&PCI32_Entry_Point));
  if (d!=0x20494350) printk("Not Installed.\n"); else {
    printk("Version %x.%x; entry %x\n",(b>>8)&255,b&255,entry);
    buses=c&255;
    printk("Bus number : %x\n",buses+1);
    for (i=0;i<=buses;i++) {
      for (id=0;id<32;id++) {
	result=PCI_Read_Word((i<<8)|(id<<3),0);
	if (result!=0xFFFF) {
	  for (kk=0;kk<8;kk++) {
	    result=PCI_Read_Word((i<<8)|(id<<3)|kk,0);
	    if (result!=0xFFFF) {
	      printk("Device Found: Bus %x, Device %x, Function %x - Vendor Id %x, Device Id %x\n",i,id,kk,result,PCI_Read_Word((i<<8)|(id<<3)|kk,2));
	    }
	    if ((PCI_Read_Byte((i<<8)|(id<<3)|kk,0xE)&0x80)==0) break;
	  }
	}
      }
    }
  }


  return 0;
}

