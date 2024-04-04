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
 * file : drivers/rtl8139.c
 * Description: drivers for the realtek 8139 network cards 
 ***************************************************************************

 ***************************************************************************
 * $IdS
 ***************************************************************************
 *
 * $LogS
 *
 ***************************************************************************/

/* Code taken from :
     rtl8139.c: A RealTek RTL8129/8139 Fast Ethernet driver for Linux.
     Written 1997-2000 by Donald Becker. */


#include <io.h>
#include <printf.h>
#include <processes.h>
#include <semaphores.h>
#include <drivers.h>
#include <messages.h>
#include <protocols/disk.h>
#include <protocols/init.h>
#include <pci.h>
#include <types.h>

#include <protocols/mpsint.h>
#include <protocols/drv2mps.h>

enum ClearBitMasks {
	MultiIntrClear = 0xF000,
	ChipCmdClear = 0xE2,
	Config1Clear = (1<<7)|(1<<6)|(1<<3)|(1<<2)|(1<<1),
};

enum ChipCmdBits {
	CmdReset = 0x10,
	CmdRxEnb = 0x08,
	CmdTxEnb = 0x04,
	RxBufEmpty = 0x01,
};

#define EE_WRITE_CMD    5
#define EE_READ_CMD     6
#define EE_ERASE_CMD    7

/*  EEPROM_Ctrl bits. */

#define EE_SHIFT_CLK    0x04    /* EEPROM shift clock. */
#define EE_CS                   0x08    /* EEPROM chip select. */
#define EE_DATA_WRITE   0x02    /* EEPROM chip data in. */
#define EE_WRITE_0              0x00
#define EE_WRITE_1              0x02
#define EE_DATA_READ    0x01    /* EEPROM chip data out. */
#define EE_ENB                  (0x80 | EE_CS)

/* MII serial management: mostly bogus for now. */
/* Read and write the MII management registers using software-generated
   serial MDIO protocol.
   The maximum data clock rate is 2.5 Mhz.  The minimum timing is usually
   met by back-to-back PCI I/O cycles, but we insert a delay to avoid
   "overclocking" issues. */
#define MDIO_DIR		0x80
#define MDIO_DATA_OUT	0x04
#define MDIO_DATA_IN	0x02
#define MDIO_CLK		0x01
#define MDIO_WRITE0 (MDIO_DIR)
#define MDIO_WRITE1 (MDIO_DIR | MDIO_DATA_OUT)

#define PCI_USES_IO		1
#define PCI_USES_MEM		2
#define PCI_USES_MASTER 	4

#define PCI_COMMAND		0x04	/* 16 bits */

/*  enum RTL8129_registers { */
/*    MAC0=0,						/* Ethernet hardware address. */
/*    MAR0=8,						/* Multicast filter. */
/*    TxStatus0=0x10,				/* Transmit status (Four 32bit registers). */
/*    TxAddr0=0x20,				/* Tx descriptors (also four 32bit). */
/*    RxBuf=0x30, RxEarlyCnt=0x34, RxEarlyStatus=0x36, */
/*    ChipCmd=0x37, RxBufPtr=0x38, RxBufAddr=0x3A, */
/*    IntrMask=0x3C, IntrStatus=0x3E, */
/*    TxConfig=0x40, RxConfig=0x44, */
/*    Timer=0x48,					/* A general-purpose counter. */
/*    RxMissed=0x4C,				/* 24 bits valid, write clears. */
/*    Cfg9346=0x50, Config0=0x51, Config1=0x52, */
/*    FlashReg=0x54, GPPinData=0x58, GPPinDir=0x59, MII_SMI=0x5A, HltClk=0x5B, */
/*    MultiIntr=0x5C, TxSummary=0x60, */
/*    MII_BMCR=0x62, MII_BMSR=0x64, NWayAdvert=0x66, NWayLPAR=0x68, */
/*    NWayExpansion=0x6A, */
/*    /* Undocumented registers, but required for proper operation. */
/*    FIFOTMS=0x70,	/* FIFO Control and test. */
/*    CSCR=0x74,	/* Chip Status and Configuration Register. */
/*    PARA78=0x78, PARA7c=0x7c	/* Magic transceiver parameter register. */
/*  }; */


enum RTL8139_registers {
	MAC0 = 0,		/* Ethernet hardware address. */
	MAR0 = 8,		/* Multicast filter. */
	TxStatus0 = 0x10,	/* Transmit status (Four 32bit registers). */
	TxAddr0 = 0x20,		/* Tx descriptors (also four 32bit). */
	RxBuf = 0x30,
	RxEarlyCnt = 0x34,
	RxEarlyStatus = 0x36,
	ChipCmd = 0x37,
	RxBufPtr = 0x38,
	RxBufAddr = 0x3A,
	IntrMask = 0x3C,
	IntrStatus = 0x3E,
	TxConfig = 0x40,
	ChipVersion = 0x43,
	RxConfig = 0x44,
	Timer = 0x48,		/* A general-purpose counter. */
	RxMissed = 0x4C,	/* 24 bits valid, write clears. */
	Cfg9346 = 0x50,
	Config0 = 0x51,
	Config1 = 0x52,
	FlashReg = 0x54,
	MediaStatus = 0x58,
	Config3 = 0x59,
	Config4 = 0x5A,		/* absent on RTL-8139A */
	HltClk = 0x5B,
	MultiIntr = 0x5C,
	TxSummary = 0x60,
	BasicModeCtrl = 0x62,
	BasicModeStatus = 0x64,
	NWayAdvert = 0x66,
	NWayLPAR = 0x68,
	NWayExpansion = 0x6A,
	/* Undocumented registers, but required for proper operation. */
	FIFOTMS = 0x70,		/* FIFO Control and test. */
	CSCR = 0x74,		/* Chip Status and Configuration Register. */
	PARA78 = 0x78,
	PARA7c = 0x7c,		/* Magic transceiver parameter register. */
	Config5 = 0xD8,		/* absent on RTL-8139A */
};

byte eth_addr[6];
int addr_len;

/* Syncronize the MII management interface by shifting 32 one bits out. */
static byte mii_2_8139_map[8] = {BasicModeCtrl, BasicModeStatus, 0, 0, NWayAdvert,
				 NWayLPAR, NWayExpansion, 0 };


#define RTL_R8(reg)		inb (((unsigned long)io_base) + (reg))
#define RTL_R16(reg)		inw (((unsigned long)io_base) + (reg))
#define RTL_R32(reg)		((unsigned long) inl (((unsigned long)io_base) + (reg)))
#define RTL_W8(reg, val8)	outb ((val8), ((unsigned long)io_base) + (reg))
#define RTL_W16(reg, val16)	outw ((val16), ((unsigned long)io_base) + (reg))
#define RTL_W32(reg, val32)	outl ((val32), ((unsigned long)io_base) + (reg))

#define mdio_delay()	inb(mdio_addr)

static void mdio_sync(dword mdio_addr) {
	int i;
	for (i = 32; i >= 0; i--) {
		outb(MDIO_WRITE1, mdio_addr);
		mdio_delay();
		outb(MDIO_WRITE1 | MDIO_CLK, mdio_addr);
		mdio_delay();
	}
	return;
}

static int mdio_read(dword base_addr, int phy_id, int location)
{
	long mdio_addr = base_addr + Config4;
	int mii_cmd = (0xf6 << 10) | (phy_id << 5) | location;
	int retval = 0;
	int i;

	if (phy_id > 31) {	/* Really a 8139.  Use internal registers. */
		return location < 8 && mii_2_8139_map[location] ?
			inw(base_addr + mii_2_8139_map[location]) : 0;
	}
	mdio_sync(mdio_addr);
	/* Shift the read command bits out. */
	for (i = 15; i >= 0; i--) {
		int dataval = (mii_cmd & (1 << i)) ? MDIO_DATA_OUT : 0;

		outb(MDIO_DIR | dataval, mdio_addr);
		mdio_delay();
		outb(MDIO_DIR | dataval | MDIO_CLK, mdio_addr);
		mdio_delay();
	}

	/* Read the two transition, 16 data, and wire-idle bits. */
	for (i = 19; i > 0; i--) {
		outb(0, mdio_addr);
		mdio_delay();
		retval = (retval << 1) | ((inb(mdio_addr) & MDIO_DATA_IN) ? 1 : 0);
		outb(MDIO_CLK, mdio_addr);
		mdio_delay();
	}
	return ((retval>>1) & 0xffff);
}

static void mdio_write(dword base_addr, int phy_id, int location, int value)
{
  long mdio_addr = base_addr + Config4;
  int mii_cmd = (0x5002 << 16) | (phy_id << 23) | (location<<18) | value;
  int i;

  if (phy_id > 31) {	/* Really a 8139.  Use internal registers. */
    if (location < 8  &&  mii_2_8139_map[location])
      outw(value, base_addr + mii_2_8139_map[location]);
    return;
  }
  mdio_sync(mdio_addr);

  /* Shift the command bits out. */
  for (i = 31; i >= 0; i--) {
    int dataval = (mii_cmd & (1 << i)) ? MDIO_WRITE1 : MDIO_WRITE0;
    outb(dataval, mdio_addr);
    mdio_delay();
    outb(dataval | MDIO_CLK, mdio_addr);
    mdio_delay();
  }
  /* Clear out extra bits. */
  for (i = 2; i > 0; i--) {
    outb(0, mdio_addr);
    mdio_delay();
    outb(MDIO_CLK, mdio_addr);
    mdio_delay();
  }
  return;
}
#define NUM_TX_DESC	4	  /* Number of Tx descriptor registers. */

struct rtl8129_private {
  struct net_device *next_module;
  void *priv_addr;					/* Unaligned address for kfree */
  int chip_id;
  int drv_flags;
//  struct pci_dev *pci_dev;
//  struct net_device_stats stats;
//  struct timer_list timer;	/* Media selection timer. */
  unsigned char *rx_ring;
  unsigned int cur_rx;		/* Index into the Rx buffer of next Rx pkt. */
  unsigned int rx_config;
  unsigned int cur_tx, dirty_tx, tx_flag;
  unsigned long tx_full;				/* The Tx queue is full. */
  /* The saved address of a sent-in-place packet/buffer, for skfree(). */
//  struct sk_buff* tx_skbuff[NUM_TX_DESC];
  unsigned char *tx_buf[NUM_TX_DESC];	/* Tx bounce buffers */
  unsigned char *tx_bufs;				/* Tx bounce buffer region. */
  char phys[4];						/* MII device addresses. */
  word advertising;					/* NWay media advertisement */
  char twistie, twist_row, twist_col;	/* Twister tune state. */
  unsigned int full_duplex:1;			/* Full-duplex operation requested. */
  unsigned int duplex_lock:1;
  unsigned int default_port:4;		/* Last dev->if_port value. */
  unsigned int media2:4;				/* Secondary monitored media port. */
  unsigned int medialock:1;			/* Don't sense media type. */
  unsigned int mediasense:1;			/* Media sensing in progress. */
};

#define eeprom_delay()  ind(ee_addr)
static int read_eeprom(long ioaddr, int location, int addr_len)
{
        int i;
        unsigned retval = 0;
        long ee_addr = ioaddr + Cfg9346;
        int read_cmd = location | (EE_READ_CMD << addr_len);

        outb(EE_ENB & ~EE_CS, ee_addr);
        outb(EE_ENB, ee_addr);

        /* Shift the read command bits out. */
        for (i = 4 + addr_len; i >= 0; i--) {
                int dataval = (read_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
                outb(EE_ENB | dataval, ee_addr);
                eeprom_delay();
                outb(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
                eeprom_delay();
        }
        outb(EE_ENB, ee_addr);
        eeprom_delay();

        for (i = 16; i > 0; i--) {
                outb(EE_ENB | EE_SHIFT_CLK, ee_addr);
                eeprom_delay();
                retval = (retval << 1) | ((inb(ee_addr) & EE_DATA_READ) ? 1: 0)
;
                outb(EE_ENB, ee_addr);
                eeprom_delay();
        }

        /* Terminate the EEPROM access. */
        outb(~EE_CS, ee_addr);
        return retval;
}

int main(void) {
  char * msg;
  msginfo info;
  PID INIT;
  struct rtl8129_private nnp;
  struct rtl8129_private *np=&nnp;

  dword io_base,address;
  int i,device_id;
  dword vendor,ret;
  
  printf("First anticipation of a rtl8139 driver .... mypid is %d\n",getpid());

  
  if ((ret=DriverInit(0))!=0) {
    printf("[KEYBOARD] ***** Error in initialization phase: %s!!!\n",
	   MpsStrerror(ret));
    exit();
  }
  


  printf("RTL8139 : Scanning PCI bus 0 ...\n");
  device_id=-1;
  for (i=0;i<32;i++) {
    vendor=PCI_Read_Word(0,i,0,0);
    // printf("Id %d : vendor %x (deviceid %x)\n",i,vendor,PCI_Read_Word(0,i,0,2));
    if (vendor==0x10EC) {
      if (PCI_Read_Word(0,i,0,2)==0x8139) {
        printf("RTL8139 : device found at id %d\n",i);
        device_id=i;
        break;
      }
    }

  }
  if (device_id==-1) {
    printf("RTL8139 not found!\n");
    exit();
  }
  
  PCI_Write_Word(0,device_id,0,PCI_COMMAND,PCI_Read_Word(0,device_id,0,PCI_COMMAND)| PCI_USES_IO | PCI_USES_MEM | PCI_USES_MASTER);

  /* Soft reset the chip. */
  RTL_W8 (ChipCmd, (RTL_R8 (ChipCmd) & ChipCmdClear) | CmdReset);
  for (i=0;i<12315456;i++) 1234/5*(12+i);

  /* Check that the chip has finished the reset. */
  for (i = 1000; i > 0; i--)
    if ((RTL_R8 (ChipCmd) & CmdReset) == 0)
      break;

  //  RTL_W8 (Cfg9346, Cfg9346_Unlock);
  printf("RTL8139 : PCI header type : %x\n",PCI_Read_Byte(0,device_id,0,0xE));
  io_base=PCI_Read_Dword(0,device_id,0,0x10)&0xFFFFFFFC;
  for (i=0;i<6;i++) {
    address=PCI_Read_Dword(0,device_id,0,0x10+4*i);
    if (address&1) {
      printf("i = %d; IO address : %lx\n",(byte)i,address&0xFFFFFFFE);
    } else {
      printf("i = %d; Memory address ",(int)i);
      switch (address&6) {
      case 0:
	printf("anywhere in first 4GB,");
	break;
      case 2:
	printf("below 1MB,");
	break;
      case 4:
	printf("anywhere in 64-bit address space,");
	break;
      case 6:
	printf("reserved,");
	break;
      }
      if (address&8) printf(" prefetchable : %lx\n",address&0xFFFFFFF0);
      else printf(" not prefetchable : %lx\n",address&0xFFFFFFF0);
    }
  }
  printf("RTL8139 : IO base address : %lx\n",io_base);
  printf("RTL8139 : IRQ : %d\n",PCI_Read_Byte(0,device_id,0,0x3C));
  RTL_W8 (Config1, RTL_R8 (Config1) & ~(1<<4));
  RTL_W8 (Config4, RTL_R8 (Config4) & ~(1<<2));

  outb(0x00, io_base + Config1);
  if (read_eeprom(io_base,0,8)==0x8129) addr_len=8; else addr_len=6;
  printf("RTL8139 : %x\n",read_eeprom(io_base,0,addr_len));
  for (i=0;i<3;i++) {
    ( (word*)eth_addr)[i]=read_eeprom(io_base,i+7,addr_len);
  }
  printf("RTL8139 : ethernet address = ");
  for (i=0;i<5;i++) printf("%x:",(byte)eth_addr[i]);
  printf("%x\n",eth_addr[5]);

  {
    int phy, phy_idx = 0;
    for (phy = 0; phy < 32 && phy_idx < sizeof(np->phys); phy++) {
      int mii_status = mdio_read(io_base, phy, 1);
      if (mii_status != 0xffff  &&  mii_status != 0x0000) {
	np->phys[phy_idx++] = phy;
	np->advertising = mdio_read(io_base, phy, 4);
	printf("%s: MII transceiver %d status 0x%4.4x "
	       "advertising %4.4x.\n",
	       "RTL8139", phy, mii_status, np->advertising);
      }
    }
    if (phy_idx == 0) {
      printf("%s: No MII transceivers found!  Assuming SYM "
	     "transceiver.\n",
	     "RTL8139");
      np->phys[0] = 32;
    }
  }
  return 0;
}
