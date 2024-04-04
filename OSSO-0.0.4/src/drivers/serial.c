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
 * file : drivers/serial.                                                  *
 * Start of serial driver                                                  *
 ***************************************************************************

 ***************************************************************************
 * $Id: serial.c,v 1.17 2001/08/22 17:28:40 steros Exp $
 ***************************************************************************
 *
 * $Log: serial.c,v $
 * Revision 1.17  2001/08/22 17:28:40  steros
 * Interrupts seems to work, but communication is too slow!!!
 *
 * Revision 1.16  2001/08/21 17:20:40  steros
 * Polling should work, interrupt seems only read...
 *
 * Revision 1.15  2001/08/18 15:18:29  steros
 * Serial driver: new version. It has already to be tested.
 *
 * Revision 1.14  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.13  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.12  2001/06/24 17:04:39  steros
 * Bugfixes.
 *
 * Revision 1.11  2001/06/03 09:52:27  steros
 * Rewritten autodetect.
 *
 * Revision 1.10  2001/05/30 21:25:05  steros
 * Corrected serial input as substitute of keyboard: now it work!
 * And other little adjustment...
 *
 * Revision 1.9  2001/05/30 10:56:33  iurlano
 * bugfixes
 *
 * Revision 1.8  2001/05/28 18:05:33  steros
 * Oops, I haven't noticed that cvs scrambled serial.c! Corrected.
 *
 * Revision 1.7  2001/05/27 20:22:53  iurlano
 * Added experimental driver config feature
 *
 * Revision 1.6  2001/05/27 17:33:36  steros
 * Added other log & serial features.
 * Logging and serial still have strange behaviour and possible bugs...
 *
 * Revision 1.5  2001/05/26 14:56:14  steros
 * Added serial logging facility.
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
 ***************************************************************************/

#include <printf.h>
#include <semaphores.h>
#include <drivers.h>
#include <memory.h>
#include <string.h>
#include <types.h>
#include <io.h>
#include <types.h>
#include <messages.h>
#include <processes.h>
#include <events.h>
#include <drivers/protocols/drv2mps.h>
#include <../Config_Global.h>



#define TEST



/* Configuration for IBM compatible PC: do NOT modify! */
#define SERIAL_BASE_CLOCK 115200   /*  1.843.200 Hz / 16 */
#define SERIAL_PORT_NUM   4


#define DEFAULT_INPUT_BUFFER_SIZE   32768
#define DEFAULT_OUTPUT_BUFFER_SIZE  32768
#define DEFAULT_COMMAND_BUFFER_SIZE    64
//#define BUFFER_EDGE_LIMIT 1024

#define VERBOSE



typedef enum {
  UARTNOTPR = 0,    // Not present.
  UART8250,         // 8250.
  UART16450,        // 16450 or 8250 with scratch reg.
  UART16550_NOSCR,  // 16550 with no scratch register.
  UART16550,        // 16550.
  UART16550A_NOSCR, // 16550A with no scratch register.
  UART16550A        // 16550A.
} UART_Type;


typedef enum {
  PARITY_NONE  = 0x00,
  PARITY_ODD   = 0x08,
  PARITY_EVEN  = 0x18,
  PARITY_MARK  = 0x28,
  PARITY_SPACE = 0x38
} Parity_Type;


typedef enum {
  LENGTH_D5 = 0,
  LENGTH_D6 = 1,
  LENGTH_D7 = 2,
  LENGTH_D8 = 3
} Length_Type;


typedef enum {
  STOP_S1 = 0,
  STOP_S2 = 4,
} Stop_Type;


typedef enum {
  FLOW_NO,
  FLOW_RTSCTS,
  FLOW_XONXOFF,
} Flow_Type;


typedef enum {
  COM_INT_CANREAD  = 0x1,
  COM_INT_CANWRITE = 0x2,
  COM_INT_ERROR    = 0x4,
  COM_INT_MODEM    = 0x8
} Int_Type;


typedef struct {
  int  size;
  byte *data;
  int  head;
  int  tail;
//  int  low_limit;
//  int  high_limit;
} combuffer_t;


typedef struct {
  bool           active;
  bool           initialized;
  bool           FlowHalted;
  bool           xOffReceived;
  int            ioport;
  UART_Type      type;
  Flow_Type      flow;
  int            irq;
  combuffer_t    BufferIn;
  combuffer_t    BufferCmd;   /* This has higer priority than out. */
  combuffer_t    BufferOut;
  Bool_Semaphore SemFlow;     /* Semaphore for flow control. */
  Semaphore      SemInEmpty;
  Semaphore      SemInFull;
  Semaphore      SemCmdFull;
  Semaphore      SemCmdOutEmpty;   /* Set when cmd AND out are empty. */
  Semaphore      SemOutFull;

  /* Data for interrupt management.*/
  Semaphore SemReadInt;
  Semaphore SemWriteInt;

  /* Data for polling/thread management.*/
  int Thread_In;
  int Thread_Out;

} SerPort_t;

SerPort_t SerPort[SERIAL_PORT_NUM];


typedef struct {
  int irq;
  int thread;
  int numports;
  int ports[SERIAL_PORT_NUM];
} Handler_t;

Handler_t Handlers[SERIAL_PORT_NUM];
int NumHandlers;


/* Configuration variables. */
int Param_UseInt;
int Param_Verbose;


/*-------------------------------------------------------------------*/
/*                            Prototypes.                            */
/*-------------------------------------------------------------------*/

void PollingRead(word port);
void PollingWrite(word port);
void InterruptRead(word port);
void InterruptWrite(word port);
void MainInterruptHandler(int idx);



/*-------------------------------------------------------------------*/
/*                     Initialization functions.                     */
/*-------------------------------------------------------------------*/

/*
 Return the type of installed UART.
*/
UART_Type detect_UART(word baseaddr) {
  unsigned char x, scr=1, v;

  // First step: see if the LCR is there
  v=inb(baseaddr+3);
  outb(0x1B,baseaddr+3);
  if (inb(baseaddr+3)!=0x1B) return UARTNOTPR;
  outb(0x03,baseaddr+3);
  if (inb(baseaddr+3)!=0x03) return UARTNOTPR;
  outb(v,baseaddr+3);
  // Next thing to do is look for the scratch register
  v=inb(baseaddr+7);
  outb(0x55,baseaddr+7);
  if (inb(baseaddr+7)!=0x55) scr=0;
  outb(0xAA,baseaddr+7);
  if (inb(baseaddr+7)!=0xAA) scr=0;
  outb(baseaddr+7,v);
  // Then check if there's a FIFO
  outb(0x01,baseaddr+2);
  x=inb(baseaddr+2);
  outb(0x00,baseaddr+2); // Some old-fashioned software relies on this!
  if (!(x&0x80)) return (scr ? UART16450 : UART8250);
  if (!(x&0x40)) return (scr ? UART16550 : UART16550_NOSCR);
  return (scr ? UART16550A : UART16550A_NOSCR);
}


/*
 Look for the intlevel used by UART.
 Returns: -1 if no intlevel found, or intlevel 0-15
 Don't call it if interrupts are running!! You might lose characters.
*/
int detect_IRQ(word baseaddr) {
  char ier, mcr, imrm, imrs, maskm, masks, irqm, irqs;

//----------------------------------------------------//
//   ATTENTION: This function doesn't (yet) work!!!   //
//----------------------------------------------------//

  __asm__ volatile ("cli");  // disable all CPU interrupts
  ier=inb(baseaddr+1);     // read IER
  outb(0,baseaddr+1);      // disable all UART ints
  while ((inb(baseaddr+5)&0x20)==0);  // wait for the THR to be empty
  mcr=inb(baseaddr+4);    // read MCR
  outb(0x0F,baseaddr+4);  // connect UART to irq line
  imrm=inb(0x21);      // read contents of master ICU mask register
  imrs=inb(0xA1);      // read contents of slave ICU mask register
  outb(0x0A,0xA0);     // next read access to 0xA0 reads out IRR
  outb(0x0A,0x20);     // next read access to 0x20 reads out IRR
  outb(2,baseaddr+1);      // lets generate interrupts...
  maskm=inb(0x20);   // this clears all bits except for the one
  masks=inb(0xA0);   // that corresponds to the int
  outb(0,baseaddr+1);      // drop the int line
  maskm&=~inb(0x20); // this clears all bits except for the one
  masks&=~inb(0xA0); // that corresponds to the int
  outb(2,baseaddr+1);      // and raise it again just to be sure...
  maskm&=inb(0x20);  // this clears all bits except for the one
  masks&=inb(0xA0);  // that corresponds to the int
  outb(~maskm,0x21);   // now let us unmask this interrupt only
  outb(~masks,0xA1);
printf("SERIAL-DEBUG-a\n");
  outb(0x0C,0x20);     // enter polled mode
printf("SERIAL-DEBUG-b\n");
  outb(0x0C,0xA0);
printf("SERIAL-DEBUG-c\n");
  irqs=inb(0xA0);    // and accept the interrupt
  irqm=inb(0x20);
  inb(baseaddr+2);         // reset transmitter interrupt in UART
  outb(mcr,baseaddr+4);    // restore old value of MCR
  outb(ier,baseaddr+1);    // restore old value of IER
  if (masks) outb(0x20,0xA0);  // send an EOI to slave
  if (maskm) outb(0x20,0x20);  // send an EOI to master
  outb(imrm,0x21);     // restore old mask register contents
  outb(imrs,0xA1);
  __asm__ volatile ("sti");
  if (irqs&0x80)       // slave interrupt occured
  return (irqs&0x07)+8;
  if (irqm&0x80)       // master interrupt occured
  return irqm&0x07;
  return -1;
}


/*
 Initialize structures to be used by driver.
 This must be the first function to call!!!
*/
void Init(void) {
  word *conf_addr = (word *)0x400;
  SerPort_t *sp=SerPort;
  int i=0;

  for (; i<SERIAL_PORT_NUM; sp++, i++) {
    if (sp->active && !sp->initialized) {
      if (sp->ioport==-1) sp->ioport=conf_addr[i];
      sp->type=(sp->ioport ? detect_UART(sp->ioport) : 0);
      if (sp->irq==-1) sp->irq=(sp->ioport ? detect_IRQ(sp->ioport) : 0);

      /* Clear buffer. */
      if (sp->BufferIn.data) {
        freeosso(sp->BufferIn.data);
        sp->BufferIn.data=0;
      }
      sp->BufferIn.size=0;
      sp->BufferIn.head=0;
      sp->BufferIn.tail=0;
      if (sp->BufferOut.data) {
        freeosso(sp->BufferOut.data);
        sp->BufferOut.data=0;
      }
      sp->BufferOut.size=0;
      sp->BufferOut.head=0;
      sp->BufferOut.tail=0;

      /* Needed by XON/XOFF. */
      if (sp->BufferOut.data) {
        freeosso(sp->BufferCmd.data);
        sp->BufferCmd.data=0;
      }
      sp->BufferCmd.size=0;
      sp->BufferCmd.head=0;
      sp->BufferCmd.tail=0;

      /* Init semaphores. */
      Init_Bool_Semaphore((dword)&(sp->SemFlow),0);
      Init_Semaphore((dword)&(sp->SemInEmpty),0);
      Init_Semaphore((dword)&(sp->SemInFull),DEFAULT_INPUT_BUFFER_SIZE);
      Init_Semaphore((dword)&(sp->SemCmdFull),DEFAULT_COMMAND_BUFFER_SIZE);
      Init_Semaphore((dword)&(sp->SemCmdOutEmpty),0);
      Init_Semaphore((dword)&(sp->SemOutFull),DEFAULT_OUTPUT_BUFFER_SIZE);

      //if (Param_UseInt) {
        Init_Semaphore((dword)&(sp->SemReadInt),0);
        Init_Semaphore((dword)&(sp->SemWriteInt),1);
      //}

      if (sp->ioport>=0 && (!Param_UseInt || sp->irq>=0)) {
        sp->initialized=1;
        sp->BufferIn.size=DEFAULT_INPUT_BUFFER_SIZE;
        sp->BufferIn.data=(char *)mallosso(sp->BufferIn.size);
        //sp->BufferIn.low_limit=BUFFER_EDGE_LIMIT;
        //sp->BufferIn.high_limit=sp->BufferIn.size-BUFFER_EDGE_LIMIT;
        sp->BufferOut.size=DEFAULT_OUTPUT_BUFFER_SIZE;
        sp->BufferOut.data=(char *)mallosso(sp->BufferOut.size);
        //sp->BufferOut.low_limit=BUFFER_EDGE_LIMIT;
        //sp->BufferOut.high_limit=sp->BufferOut.size-BUFFER_EDGE_LIMIT;

        /* Needed by XON/XOFF. */
        sp->BufferCmd.size=DEFAULT_COMMAND_BUFFER_SIZE;
        sp->BufferCmd.data=(char *)mallosso(sp->BufferCmd.size);
        //sp->BufferCmd.low_limit=BUFFER_EDGE_LIMIT;
        //sp->BufferCmd.high_limit=sp->BufferCmd.size-BUFFER_EDGE_LIMIT;

      } else {
        sp->active=0;
        sp->initialized=0;
      }

      if (Param_Verbose) {
        if (sp->initialized) {
          printf("Serial port %d> I/O port 0x%x (UART type: ",i,sp->ioport);
          switch (sp->type) {
            case UART8250:
              printf("8250");
              break;
            case UART16450:
              printf("16450");
              break;
            case UART16550:
              printf("16550");
              break;
            case UART16550A:
              printf("16550A");
              break;
            default:
              printf("unknown");
          }
          printf(")");
          if (Param_UseInt) {
            printf(", interrupt ");
            if (sp->irq<0)
              printf("[%s]",sp->irq==-1 ? "not found" : "none");
            else
              printf("%d",sp->irq);
          }
          printf(".\n");
        } else
          printf("Serial port %d> not installed.\n");
      }
    }
  }
}


/*
 Initialize the port with specified parameters.
 Returns 0 if ok.
*/
int InitPort(word port, int speed, Parity_Type parity,
             Length_Type length, Stop_Type stop, Flow_Type flow)
{
  SerPort_t *sp=&(SerPort[port]);
  word p=sp->ioport;
  int c;

  //^^^ Se viene richiamata 2 volte bisogna distruggere i threads
  //    creati precedentemente?!?

  if (!sp->active) return -1;
  if (!sp->initialized) {
    Init();
    if (!sp->initialized) return -2;
  }

  outb(0,p+1);      /* Turn off interrupts. */

  /* Set Baud rate. */
  outb(0x80,p+3);   /* SET DLAB ON */
  outw((SERIAL_BASE_CLOCK+(speed>>1))/speed,p);

  /* Set communication type. */
  outb(parity && length && stop,p+3);
  sp->flow=flow;
  sp->FlowHalted=0;
  sp->xOffReceived=0;

  /* FIFO Control Enabling*/
  outb(0xC7,p+2);
  if (!(inb(p+2)&0x40)) outb(0x0,p+2);

  outb(0x0B,p+4);   /* Turn on DTR, RTS, and OUT2. */

  if (Param_UseInt) {   /* Install the interrupt handler. */
    /* Look for an already active handler to use. */
    for (c=0; c<NumHandlers && Handlers[c].irq!=sp->irq; c++);
    if (c>=NumHandlers) {
      /* Handler not found, I create it. */
      Handlers[NumHandlers].irq=sp->irq;
      Handlers[NumHandlers].ports[0]=port;
      Handlers[NumHandlers].numports=1;
      Handlers[NumHandlers].thread=
          Create_Thread((Thread_Function)MainInterruptHandler,
                        NumHandlers);
      NumHandlers++;
    } else {
      /* Handler found: add port to this. */
      Handlers[c].ports[Handlers[c].numports]=port;
      Handlers[c].numports++;
    }
    sp->Thread_In=Create_Thread((Thread_Function)InterruptRead,port);
    sp->Thread_Out=Create_Thread((Thread_Function)InterruptWrite,port);
    outb((COM_INT_CANREAD | COM_INT_CANWRITE |
         COM_INT_ERROR   | COM_INT_MODEM),      p+1);
  } else {   /* Install the polling threads. */
    sp->Thread_In=Create_Thread((Thread_Function)PollingRead,port);
    sp->Thread_Out=Create_Thread((Thread_Function)PollingWrite,port);
  }

  return 0;
}



/*-------------------------------------------------------------------*/
/*                   Buffers management functions.                   */
/*-------------------------------------------------------------------*/


/*
 Add a byte to command buffer updating semaphores.
*/
void AddByteToCommandBuffer(int port, byte b) {
  SerPort_t *sp=&(SerPort[port]);
  combuffer_t *bc=&(sp->BufferCmd);

  /* Check if cmd buffer is full. */
  P((dword)&(sp->SemCmdFull));
  /* Add byte to buffer. */
  bc->data[bc->tail]=b;
  bc->tail=(bc->tail+1)%bc->size;
  /* If locked, unlock the process that read out/cmd buffer. */
  V((dword)&(sp->SemCmdOutEmpty));
  Bool_V((dword)&(sp->SemFlow));  /* Commands are always send. */
}


/*
 Add a byte to output buffer updating semaphores.
*/
void AddByteToOutputBuffer(int port, byte b) {
  SerPort_t *sp=&(SerPort[port]);
  combuffer_t *bo=&(sp->BufferOut);

  /* Check if out buffer is full. */
  P((dword)&(sp->SemOutFull));
  /* Add byte to buffer. */
  bo->data[bo->tail]=b;
  bo->tail=(bo->tail+1)%bo->size;
  /* If locked, unlock the process that read out/cmd buffer. */
  V((dword)&(sp->SemCmdOutEmpty));
}


/*
 Get a byte from command or output buffer updating semaphores.
*/
byte GetByteFromOutCmdBuffer(int port) {
  SerPort_t *sp=&(SerPort[port]);
  combuffer_t *bc=&(sp->BufferCmd);
  combuffer_t *bo=&(sp->BufferOut);
  byte b;

  /* Check if cmd/out buffers are empty. */
  P((dword)&(sp->SemCmdOutEmpty));

  /* Wait if command buffer is empty and output flow is stopped. */
  // State of CTR on port is not semaphorized because I can't do
  // a Bool_V on sem!!!
  if (bc->tail==bc->head && sp->xOffReceived)
    Bool_P((dword)&(sp->SemFlow));


  /* If present get data from command buffer. */
  if (bc->tail!=bc->head) {
    b=bc->data[bc->head];
    bc->head=(bc->head+1)%bc->size;
    V((dword)&(sp->SemCmdFull));   /* Unlock cmd buffer writer. */
    return b;
  }

  /* If present get data from output buffer. */
  if (bo->tail!=bo->head) {
    b=bo->data[bo->head];
    bo->head=(bo->head+1)%bo->size;
    V((dword)&(sp->SemOutFull));   /* Unlock out buffer writer. */
    return b;
  }

  /*DBG*/ printf("[SERIAL] ***** GetByteFromOutCmdBuffer ERROR *****\n");

  return 0;
}


/*
 This function add a char to input buffer managing flow control.
*/
void AddByteToInputBuffer(int port, byte character) {
  SerPort_t *sp=&(SerPort[port]);
  combuffer_t *bf=&(sp->BufferIn);

  //^^^ It' right for FLOW_NO so?!?
  P((dword)&(sp->SemInFull));

  /* Put character in buffer. */
  bf->data[bf->tail]=character;

  if (sp->flow!=FLOW_NO) {
    /* Check for incoming XON/XOFF. */
    if (sp->flow==FLOW_XONXOFF) {
      if (character==0x11) sp->xOffReceived=0;     /* XON */
      else
        if (character==0x13) sp->xOffReceived=1;   /* XOFF */
        else
          bf->tail=(bf->tail+1)%bf->size;   /* If not keep new byte. */
    } else
      bf->tail=(bf->tail+1)%bf->size;   /* If not keep new byte. */

    /* Check if outgoing must be temporized. */
    if (!sp->FlowHalted) {
      //if ((bf->tail>=bf->head && bf->head-bf->tail+bf->size<bf->low_limit) ||
      //    (bf->tail<bf->head && bf->head-bf->tail<bf->low_limit))
      if ((bf->tail>=bf->head && bf->head-bf->tail+bf->size==0) ||
          (bf->tail<bf->head && bf->head-bf->tail==0))
      {
        if (sp->flow==FLOW_RTSCTS)
          outb(inb(sp->ioport+4)&0xFD,sp->ioport+4);   /* Lower RTS. */
        else
          if (sp->flow==FLOW_XONXOFF)
            AddByteToCommandBuffer(port,0x13);   /* Send XOFF. */
        sp->FlowHalted=1;
      }
    }
  }
  V((dword)&(sp->SemInEmpty));
}


/*
 Get a byte from input buffer updating semaphores.
*/
byte GetByteFromInputBuffer(int port) {
  SerPort_t *sp=&(SerPort[port]);
  combuffer_t *bi=&(sp->BufferIn);
  byte b;

  /* Check if input buffers is empty. */
  P((dword)&(sp->SemInEmpty));

  /* If present get data from input buffer. */
  if (bi->tail!=bi->head) {
    b=bi->data[bi->head];
    bi->head=(bi->head+1)%bi->size;

    V((dword)&(sp->SemInFull));   /* Unlock input buffer writer. */
    /* Unlock serial reader thread if needed. */
    if (/*sp->flow!=FLOW_NO &&*/ sp->FlowHalted) {
      //if ((bf->tail>=bf->head && bf->head-bf->tail+bf->size>0) ||
      //    (bf->tail<bf->head && bf->head-bf->tail>0)) {
        if (sp->flow==FLOW_RTSCTS)
          outb(inb(sp->ioport+4)|0x02,sp->ioport+4);   /* Raise RTS. */
        else
          if (sp->flow==FLOW_XONXOFF)
            AddByteToCommandBuffer(port,0x11);   /* Send XON. */
        sp->FlowHalted=0;
        Bool_V((dword)&(sp->SemFlow));     /* Unlock flow buffer. */
      //}
    }
    return b;
  }

  /*DBG*/ printf("[SERIAL] ***** GetByteFromInputBuffer ERROR *****\n");

  return 0;
}



/*-------------------------------------------------------------------*/
/*                             Polling.                              */
/*-------------------------------------------------------------------*/


void writebyte_poll(word port, byte ch) {
  while (!(inb(SerPort[port].ioport+5)&0x20));  //^^^ Delay?!?
  outb(ch,SerPort[port].ioport);
}


/*
word trygetchar(word port) {
   word x;

   x=(inw(SerPort[port].ioport+5)&0x9f)<<8;
   if (x&0x100) x|=((word)inb(SerPort[port].ioport))&0xff;
   return x;
}


char getchar_poll(word port) {
  union {
    word val;
    char character;
  } x;

  while (1) {
    x.val=trygetchar(port);
    if (!x.val) continue;  * Nothing? Continue. *
    if (x.val&0x100) return x.character;
    if (!(x.val&0xe00)) continue;  * No error condidion: continue. *
    if (Param_Verbose) {
      if (x.val&0x200) printf("[SERIAL] *** Overrun Error ***");
      if (x.val&0x400) printf("[SERIAL] *** Parity Error ***");
      if (x.val&0x800) printf("[SERIAL] *** Framing Error ***");
    }
  }
}
*/


/*
  Thread that read bytes from port in polling mode.
*/
void PollingRead(word port) {
  int io=SerPort[port].ioport;
  byte err, character;

  do {
    err=inw(io+5)&0x9f;
    if (err&0x01) {
      character=inb(io);
      AddByteToInputBuffer(port,character);
    }
    //^^^ else delay?!?
  } while (1);
}


/*
  Thread that write bytes to port in polling mode.
*/
void PollingWrite(word port) {
  byte character;

  do {
    character=GetByteFromOutCmdBuffer(port);
    writebyte_poll(port,character);
  } while (1);
}



/*-------------------------------------------------------------------*/
/*                            Interrupts.                            */
/*-------------------------------------------------------------------*/


/*
 Thread that write output to serial port.
 It is synchronized by semaphores & interrupts.
*/
void InterruptWrite(word port) {
  SerPort_t *sp=&(SerPort[port]);
  dword IntSem=(dword)&(sp->SemWriteInt);
  int p=sp->ioport;

  byte character;

  do {
    character=GetByteFromOutCmdBuffer(port);
    P(IntSem);
    outb(character,p);
  } while (1);
}


/*
 Thread that read input from serial port.
 It is synchronized by semaphores & interrupts.
*/
void InterruptRead(word port) {
  SerPort_t *sp=&(SerPort[port]);
  dword IntSem=(dword)&(sp->SemReadInt);
  int p=sp->ioport;

  do {
    P(IntSem);
    AddByteToInputBuffer(port,inb(p));
  } while (1);
}


/*
 Interrupt handler: this function manage only the port specified.
*/
void InterruptHandler(int port) {
  SerPort_t *sp=&(SerPort[port]);
  byte status;

  status=inb(sp->ioport+2);

  if (status&0x01) return; /* No interrupt to service for this port. */

  switch ((status&0x06)>>1) {
    case 3:   /* Error. */
        status=inb(sp->ioport+5);
        /*^^^*/
        break;
    case 2:   /* There's data to be read. */
        V((dword)&(sp->SemReadInt));
        //^^^ Attenzione a FLOW=NO!!!
        break;
    case 1:   /* I can write to port. */
        V((dword)&(sp->SemWriteInt));
        break;
    case 0:   /* Modem generated interrupt. */
        status=inb(sp->ioport+6);
        /*^^^*/
        break;
  }
}


/*
 Main interrupt handler: this is attached to a single IRQ but can
 manage different ports.
*/
void MainInterruptHandler(int idx) {
  char *irqName="INTERRUPTxx";
  Handler_t *h=&(Handlers[idx]);
  int i=h->irq;
  PID pidsem;

/*DBG*/ printf("[DBG] MainInterruptHandler> inizio\n");

  /* Set interrupt event name. */
  if (i<10) {
    irqName[9]=i + '0';
    irqName[10]='\0';
  } else {
    irqName[9]=(i/10) + '0';
    irqName[10]=(i%10) + '0';
  }
  if (getevent(irqName)!=0) {
    printf("[SERIAL] Error requesting handling of '%s' event!\n",irqName);
    return;
  }
  while (1) {
    /* Wait for interrupt. */
    i=wait((EVENT)irqName,&pidsem);
    if (i) {
      printf("[SERIAL] Error %d while waiting '%s' event!\n",i,irqName);
      continue;
    }
    /* Call specific port handlers. */
    for (i=0; i<h->numports; i++)
      InterruptHandler(h->ports[i]);
  }
}


/*-------------------------------------------------------------------*/
/*                       High level functions.                       */
/*-------------------------------------------------------------------*/


void write_char(word port, char ch) {
  AddByteToOutputBuffer(port,ch);
}


char read_char(word port) {
  return GetByteFromInputBuffer(port);
}


/* Not necessary (see below)!!!
void write_string(word port, char *s) {
  for (; *s; s++)
    AddByteToOutputBuffer(port,*s);
}
*/


/* Dangerous: can cause buffer overflow!!!
void read_string(word port, char *s) {
  for (; *s; s++)
    *s=GetByteFromInputBuffer(port);
}
*/


void write_buffer(word port, char *buf, int max) {
  for (; max; buf++, max--)
    AddByteToOutputBuffer(port,*buf);
}


void read_buffer(word port, char *s, int max) {
  for (; max; s++, max--)
    *s=GetByteFromInputBuffer(port);
}



/*-------------------------------------------------------------------*/
/*                   Parameters parsing functions.                   */
/*-------------------------------------------------------------------*/


/*
 This must be moved in a ctype library file!!!
*/
int toupper(int c) {
  if (c>='a' && c<='z') c+='A'-'a';
  return c;
}


/*
 Read a number from C-format string. The base can be 8, 10 or 16.
 This have to be substituted by strtol() in the future.
*/
word StringToInt(char *s) {
  int base=10, molt=1;
  word num=0;
  char *p;
  char c;

  if (*s=='0') {
    s++;
    if (*s=='x') {
      s++;
      base=16;
    } else
      base=8;
  }
  p=s;
  /* Searching for end of number. */
  switch (base) {
    case 8:
      for (; *p>='0' && *p<='7'; p++);
      break;
    case 10:
      for (; *p>='0' && *p<='9'; p++);
      break;
    case 16:
      for (; ((*p>='0' && *p<='9') || (toupper(*p)>='A' && toupper(*p)<='F'));
        p++);
      break;
    default:
      printf("Error in StringToInt(): base not implemented!!!\n");
      return 0;
  }
  p--;
  /* Scanning number. */
  for (; p>=s; p--, molt*=base) {
    switch (base) {
      case 8:
      case 10:
        num+=(*p-'0')*molt;
        break;
      case 16:
        c=toupper(*p);
        if (c>='A' && c<='F')
          num+=(c-'A'+10)*molt;
        else
          num+=(*p-'0')*molt;
        break;
      default:
        printf("Error in StringToInt(): base not implemented!!!\n");
    }
  }
  return num;
}


/* Read IRQ values. */
void ReadIrq(char *value) {
  char *i=value, *f=value, car=*value;
  int num=0;

  while (car && num<SERIAL_PORT_NUM) {
    for (f=i; *f!='\0' && *f!=','; f++);
    car=*f;
    *f='\0';
    SerPort[num].irq=(strcmp(i,"auto") ? StringToInt(i) : -1);
    *f=car;
    i=++f;
    num++;
  }
}


/* Read IO ports values. */
void ReadIo(char *value) {
  char *i=value, *f=value, car=*value;
  int num=0;

  while (car && num<SERIAL_PORT_NUM) {
    for (f=i; *f!='\0' && *f!=','; f++);
    car=*f;
    *f='\0';
    SerPort[num].ioport=(strcmp(i,"auto") ? StringToInt(i) : -1);
    *f=car;
    i=++f;
    num++;
  }
}


/* Read ports to activate values. */
void ReadPorts(char *value) {
  char *i=value, *f=value, car=*value;
  int num=0;

  while (car) {
    for (f=i; *f!='\0' && *f!=','; f++);
    car=*f;
    *f='\0';
    num=StringToInt(i);
    *f=car;
    if (num>=0 && num<SERIAL_PORT_NUM)
      SerPort[num].active=1;
    else
      printf("[SERIAL] Port %d doesn't exist! Valid ports are 0-%d.\n",
             num,SERIAL_PORT_NUM);
    i=++f;
  }
}


/*
 Process parameters passed through command line.
*/
void ProcessParameters(char *params) {
  char *ParamNames[] = { "mode", "io", "irq", "ports", "verbose", 0 };
  int c;

  /* Default values. */
  NumHandlers=0;
  Param_UseInt=0;
  Param_Verbose=0;
  for (c=0; c<4; c++) {
    SerPort[c].active=0;
    SerPort[c].initialized=0;
    SerPort[c].ioport=-1;  /* Autodetect.                       */
    SerPort[c].irq=-2;     /* Not specified (avoid autodetect). */

    SerPort[c].BufferIn.size=0;
    SerPort[c].BufferIn.data=NULL;
    SerPort[c].BufferIn.head=0;
    SerPort[c].BufferIn.tail=0;

    SerPort[c].BufferOut.size=0;
    SerPort[c].BufferOut.data=NULL;
    SerPort[c].BufferOut.head=0;
    SerPort[c].BufferOut.tail=0;

    /* Needed by XON/XOFF. */
    SerPort[c].BufferCmd.size=0;
    SerPort[c].BufferCmd.data=NULL;
    SerPort[c].BufferCmd.head=0;
    SerPort[c].BufferCmd.tail=0;
  }

  while (*params) {
    /* Param name*/
    for (c=0; ParamNames[c] && strcmp(params,ParamNames[c]); c++);
    if (!ParamNames[c]) {
      printf("[SERIAL] '%s': unknown parameter!\n",params);
      c=-1;
    }
    while (*params) params++;   /* Move pointer to param value. */
    params++;
    /* Param value. */
    switch (c) {
      case -1:   /* Unknown parameter: nothing to do. */
          break;
      case  0:   /* mode */
          if (!strcasecmp("int",params))
            Param_UseInt=1;
          else if (!strcasecmp("poll",params))
            Param_UseInt=0;
          else
            printf("'%s': unknoun value for parameter 'mode'!\n",params);
          break;
      case  1:   /* io */
          ReadIo(params);
          break;
      case  2:   /* irq */
          ReadIrq(params);
          break;
      case  3:   /* ports */
          ReadPorts(params);
          break;
      case  4:   /* verbose */
          if (*params)
            printf("[SERIAL] 'verbose' parameter has no value!\n");
          Param_Verbose=1;
          break;
      default:
          printf("[SERIAL] '%s': parameter not yet implemented!\n",
                 ParamNames[c]);
    }
    while (*params) params++;   /* Move pointer to next param name. */
    params++;
  }
}





int main(void) {
#ifndef TEST
  msginfo info;
  char *msg;
#endif
//  char *res_msg;
  char *params;
  int ret;


  printf("[SERIAL] My pid is %d.\n",getpid());


#ifdef SERIAL_INPUT
  printf("[SERIAL] Osso compiled with serial input can conflict with this driver.\n");
#endif
#ifdef SERIAL_LOGGING
  printf("[SERIAL] Osso compiled with logging can conflict with this driver.\n");
#endif


#ifdef TEST
  { // Wait some to avoid messages conflicts...
    int i;
    for (i=0; i<30000000; i++);
  }
  printf("------------------------ Serial start ------------------------\n");
#endif

  /* Start initialization. */
  if ((ret=DriverInit(&params))!=0) {
    printf("[SERIAL] ***** Error in initialization phase: %s!!!\n",
      MpsStrerror(ret));
    exit();
  }

  ProcessParameters(params);

  Init();


#ifdef TEST
{
  word Port     = 1;
  int  TestType = 1;   /* 1 = write, 2 = read, 4=r/w. */
  char LoopBack = 0;

  int cnt;
  char c;

  printf("\nParameters: Verbose=%s, UseInt=%s\n",
         Param_Verbose ? "YES" : "NO",Param_UseInt ? "YES" : "NO");

  InitPort(Port,115200,PARITY_NONE,LENGTH_D8,STOP_S1,FLOW_RTSCTS);

  if (LoopBack)
    outb(SerPort[Port].ioport+4,inb(SerPort[Port].ioport+4)|0x10);

  if (TestType&1) {
    int max=5, v;
    // Writing test...
    printf("Writing test...\n");
    for (cnt=max; cnt; cnt--) {
      printf("Start of cicle number %d\n",max-cnt+1);
      write_char(Port,'[');
      if (max>10) {
        v=((cnt-1)/10)%10;
        write_char(Port,v==0 ? ' ' : '0'+v);
      }
      write_char(Port,'0'+((cnt-1)%10));
      write_char(Port,']');
      for (c='A'; c<='Z'; c++) write_char(Port,c);
      for (c='a'; c<='z'; c++) write_char(Port,c);
      for (c='0'; c<='9'; c++) write_char(Port,c);
      write_char(Port,'\n');
      printf("End of cicle number %d\n",max-cnt+1);
    }
    printf("Writing test finished!\n");
  }

  if (TestType&2) {
    // Reading test...
    printf("\nReading from serial...\n");
    printf("------------------------\n");
    do {
      printf("%c",read_char(Port));
    } while (1);
  }

  if (TestType&4) {
    char *frase="Ciaopippo\n";

    printf("\nWriting\n  %s toserial...\n",frase);
    printf("<------------------------>\n\n");
    write_buffer(Port,frase,strlen(frase));
    printf("\nReading from serial...\n");
    printf("------------------------\n");
    do {
      printf("%c",read_char(Port));
    } while (1);
  }

  while(1);

}
#else

  /* This ends initialization phase. */
  if ((ret=DriverStart("io_device","serial",DRV_I_MONO,0,1,
    "This driver support standard serial ports."))!=0)
  {
    printf("[SERIAL] ***** ERROR: MPS don't allow me to register: %s!!!\n",
      MpsStrerror(ret));
    exit();
  }

  printf("[SERIAL] Ok. I'm registered to MPS.\n");


  while (1) {
    msg=recv_msg(&info);
/*--------------------------
    if (*msg=='K') {
      res_msg=getbmsg();
      *((char *)res_msg)=getchar(SERIAL_INPUT_PORT);
      send_msg(info.mittente,res_msg,1);
    }
--------------------------*/
    freemsg(msg);
  }

  DriverExit();

#endif

  return 0;
}
