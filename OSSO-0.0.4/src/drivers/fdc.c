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
 * file : drivers/fdc.c                                                    *
 * Floppy disk driver                                                      *
 ***************************************************************************

 ***************************************************************************
 * $Id: fdc.c,v 1.9 2001/08/22 14:19:26 iurlano Exp $
 ***************************************************************************
 *
 * $Log: fdc.c,v $
 * Revision 1.9  2001/08/22 14:19:26  iurlano
 * Added partition support to the FAT driver. Tested with hda3 and hda4 and seems working
 *
 * Revision 1.8  2001/08/18 16:22:46  iurlano
 * WORK on fdc and dma. Still no success ...
 *
 * Revision 1.7  2001/08/15 20:06:36  iurlano
 * Added DMA management functions into the kernel and library
 *
 * Revision 1.6  2001/08/13 23:55:27  iurlano
 * FDC Work in progress, but needs DMA
 * DMA work started.
 * Some warnings fixed in kernel code
 *
 * Revision 1.5  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.4  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.3  2001/06/10 22:46:37  iurlano
 * Added hiworld program to xconfig panel
 *
 */

#include <printf.h>   /* printf */
#include <protocols/disk.h>
#include <protocols/init.h>
#include <events.h>
#include <drivers.h>
#include <protocols/drv2mps.h>
#include <messages.h>
#include <dma_memory.h>
#include <dma.h>

#include <io.h>

#define Floppy_DMA 2

int current_track=0;

#define      FDC_MAX_DRIVES 2      //compatible with most drive controllers

#define DEBUG_FDC

unsigned int FDC_BASE_IO = 0x03F0; //the base address of the controller
//the io port offsets from the base
#define      FDC_DSR_A     0x0000  //disk status register A
#define      FDC_DSR_B     0x0001  //disk status register B
#define      FDC_DOR       0x0002  //Digital output register
#define      FDC_MSR       0x0004  //Main Status Register
#define      FDC_DATA      0x0005  //multi purpose data register
#define      FDC_DIR       0x0007  //digital input register(READ)
#define      FDC_CCR       0x0007  //Configuration Control Register (write)

//masks for base+0 FDC_DSR_A   (read only) //not on all controllers
#define      FDC_DIRECTION          0x01  //1 = forward direction?
#define      FDC_WRITE_PROTECTED    0x02  //1 = write protected
#define      FDC_INDEX              0x04  //1 = index?
#define      FDC_HEAD_1_SELECTED    0x08  //1 = HEAD 1    0 = HEAD 0
#define      FDC_TRACK_0            0x10  //1 = TRACK 0
#define      FDC_STEP               0x20  //1 = step?
#define      FDC_SECOND_DRIVE       0x40  //1 = second drive attached
#define      FDC_INTERRUPT_PENDING  0x80  //1 = interrupt pending

//masks for base+1 FDC_DSR_B   (read only)
#define      FDC_MOTOR_0_ENABLED    0x01  //1 = motor is on
#define      FDC_MOTOR_1_ENABLED    0x02  //1 = motor is on
#define      FDC_WRITE_ENABLED      0x04  //1 = writing mode is enabled
#define      FDC_READ_DATA_MODE     0x08  //1 = read data mode
#define      FDC_WRITE_DATA_MODE    0x10  //1 = write data mode
#define      FDC_DRIVE_SELECTED     0x20  //1 = DRIVE 1    0 = DRIVE 0

//masks for base+2 FDC_DOR   (write only)
#define      FDC_DRIVE_SELECT       0x03  //bottom bits are drive 1=FD0 2=FD1 3=FD2 4=FD3
#define      FDC_ENABLE             0x04  //enable the FDC 0 = hold at reset
#define      FDC_DMA_IO             0x08  //ENABLE DMA & IO
#define      FDC_MOTOR_0            0x10  //turn on motor 0
#define      FDC_MOTOR_1            0x20  //turn on motor 1
#define      FDC_MOTOR_2            0x40  //turn on motor 2
#define      FDC_MOTOR_3            0x80  //turn on motor 3

//masks for base+4 FDC_MSR (read only)
#define      FDC_DRIVE_0_SEEKING    0x01  //1 = Busy
#define      FDC_DRIVE_1_SEEKING    0x02  //1 = Busy
#define      FDC_DRIVE_2_SEEKING    0x04  //1 = Busy
#define      FDC_DRIVE_3_SEEKING    0x08  //1 = Busy
#define      FDC_COMMAND_BUSY       0x10  //1 = Executing a command
#define      FDC_NON_DMA_MODE       0x20  //1 = NON DMA
#define      FDC_IO_DIRECTION       0x40  //1 = FDC->CPU 0=CPU->FDC
#define      FDC_DATA_READY         0x80  //1 = ready for IO

unsigned char FDC_DOR_VALUE;
unsigned char FDC_CURRENT_COMMAND=0;

//MASKS FOR THE COMMANDS WHICH MAY ALSO REQUIRE OTHER PARAMETERS TO BE OR'D IN
#define      FDC_READ_DATA          0x06
#define      FDC_READ_DELETED_DATA  0x0C
#define      FDC_WRITE_DATA         0x05
#define      FDC_WRITE_DELETED_DATA 0x09
#define      FDC_READ_TRACK         0x02
#define      FDC_VERIFY             0x16
#define      FDC_VERSION            0x10
#define      FDC_FORMAT_TRACK       0x0D
#define      FDC_SCAN_EQUAL         0x11   //(help_pc)? or 0x10 (intel)
#define      FDC_SCAN_LOW_OR_EQUAL  0x19
#define      FDC_SCAN_HIGH_OR_EQUAL 0x1D
#define      FDC_CALIBRATE          0x07
#define      FDC_SENSE_INTERRUPT    0x08
#define      FDC_SPECIFY            0x03
#define      FDC_SENSE_DRIVE_STATUS 0x04
#define      FDC_DRIVE_SPECIFICATION 0x8E
#define      FDC_SEEK               0x0F
#define      FDC_CONFIGURE          0x13
#define      FDC_RELATIVE_SEEK      0x8F

/*****************************************************************************
 *
 * the following are commands for reading and writing to the fdc controller
 * commands and data from the controller
 *      fdc_read(&value);  //value gets the data read
 *      fdc_write(value);  //value gets written to the fdc
 * these commands return 0 on failure or 1 on success
*****************************************************************************/

/* polls the drive for 1000000 counts to attempt to read a data when it becomes
 * ready.  Timeout returns a value of 0, success returns a value of 1
 * the value read is 0 on failure and the value on success. */
unsigned int fdc_read(unsigned char* value)
{
    unsigned int counter = 100000;
    unsigned char status;
    while (counter--) {
        status=inb(FDC_BASE_IO+FDC_MSR);
//        status=status & (FDC_DATA_READY|FDC_IO_DIRECTION);   //test the two bits
        if ((status&(FDC_DATA_READY|FDC_IO_DIRECTION)) == (FDC_DATA_READY|FDC_IO_DIRECTION)) {    //ready for fdc to cpu transfer
            //data is ready to be read
            *value=inb(FDC_BASE_IO+FDC_DATA);
            if((status&FDC_NON_DMA_MODE) == FDC_NON_DMA_MODE) {
                //data
                return 2;
            } else {
                //result
                return 1;
            }
        }
	inb(0x80);
    }
#ifdef DEBUG_FDC
    printf("\nfdc_read(&value); timed out!!!\n");
#endif
    //timed out
    *value=0;
    return 0;
}

/* polls the drive for 1000000 counts to attempt to write a data when it becomes
 * ready.  Timeout returns a value of 0, success returns a value of 1 */
unsigned int fdc_write(unsigned char value)
{
    unsigned int counter=100000;
    unsigned char status;
    while (counter--) {
        status=inb(FDC_BASE_IO+FDC_MSR);
        status=status & (FDC_DATA_READY|FDC_IO_DIRECTION);   //test the two bits
        if (status == (FDC_DATA_READY)) {                     //ready for cpu to fdc transfer
            //data is ready to be written
            outb(value,FDC_BASE_IO+FDC_DATA);
            return 1;
        }
	inb(0x80);
    }
#ifdef DEBUG_FDC
    printf("\nfdc_write(value); timed out!!!\n");
#endif
    return 0;
}

/*****************************************************************************
 *
 * The following commands ar for setting the mode and ressint the fdc to known
 * states and modes of operation.
 * fdc_hold_reset();  //will hold the fdc in a reset state without losing the
 *                    //current settings for motors on and DMA/IO settings
 * fdc_release_reset();  //will release the fdc from the reset state without
 *                    //losing the current setting
 * fdc_full_reset();  //will fully reset the drive and all setting and values
 *                    //then start the fdc in a known state
 * These commands return 1 on success and 0 on failure
 *
*****************************************************************************/

/* will disable the fdc by holding it at reset*/
unsigned int fdc_hold_reset(void)
{
    FDC_DOR_VALUE=(~FDC_ENABLE)&FDC_DOR_VALUE;  //clear the reset bit keep other bits
    outb(FDC_DOR_VALUE,FDC_BASE_IO+FDC_DOR);  //write it
    return 1;
}

/* will enable the fdc by releasing the reset*/
unsigned int fdc_release_reset(void)
{
    FDC_DOR_VALUE=(FDC_ENABLE)|FDC_DOR_VALUE;  //set the reset bit keep other bits
    outb(FDC_DOR_VALUE,FDC_BASE_IO+FDC_DOR);  //write it
    return 1;
}

extern unsigned int fdc_command_sense_interrupt(void);
/* will disable the fdc then re-enable it to a known state*/
unsigned int fdc_full_reset(unsigned char option)
{
  FDC_DOR_VALUE=0;  //hold at reset and kill IO/DMA and kill all motors
#ifdef DEBUG_FDC
  printf("\nsetting FDC_DOR=0x%x\n",FDC_DOR_VALUE);
#endif
  outb(FDC_DOR_VALUE,FDC_BASE_IO+FDC_DOR);

  FDC_DOR_VALUE=(FDC_ENABLE);
  if (option == FDC_DMA_IO) {
    FDC_DOR_VALUE=(FDC_DOR_VALUE|FDC_DMA_IO);
  }
#ifdef DEBUG_FDC
  printf("\nsetting FDC_DOR=0x%x\n",FDC_DOR_VALUE);
#endif
  outb(FDC_DOR_VALUE,FDC_BASE_IO+FDC_DOR);

  {
    unsigned int int_value=0,x;
    for (x=1; x <= 10;x++) {
      int_value = fdc_command_sense_interrupt();
      if (int_value != 0x80) {
#ifdef DEBUG_FDC
	printf("Test %d = 0x%x\n",x,fdc_command_sense_interrupt());
#endif
      }
    }
  }
  return 1;
}

/*****************************************************************************
 *
 * The following commands are for activating and stopping the given drive
 *      fdc_start_drive(drive_number);    //will start the motor of the given drive
 *      fdc_stop_drive(drive_number);     //will stop the motor of the given drive
 *      fdc_select_drive(drive_number);   //will set the drive as active for commands
 *      fdc_activate_drive(drive_number); //will start the motor and activate
 *      fdc_deactivate_drive(drive_number); //will stop the motor and deactivate
 * these commands return  on success and 0 on failure
 * only one drive can be active at a time due to limitations of the FDC
 * this is set by the fdc_select_drive
 * generally only the fdc_activate_drive(num) and fdc_deactivate_drive(num)
 * are the only two function you need
*****************************************************************************/

/* This will start the motor on the selected drive */
unsigned int fdc_start_drive_motor(unsigned int drive_number)
{
    if (drive_number < FDC_MAX_DRIVES) {
        if (drive_number == 0) {
            FDC_DOR_VALUE=FDC_DOR_VALUE|FDC_MOTOR_0;
        } else if (drive_number == 1) {
            FDC_DOR_VALUE=FDC_DOR_VALUE|FDC_MOTOR_1;
        } else if (drive_number == 2) {
            FDC_DOR_VALUE=FDC_DOR_VALUE|FDC_MOTOR_2;
        } else if (drive_number == 3) {
            FDC_DOR_VALUE=FDC_DOR_VALUE|FDC_MOTOR_3;
        } else {
            //not a valid setting
            return 0;
        }
#ifdef DEBUG_FDC
        printf("\nsetting FDC_DOR=0x%x\n",FDC_DOR_VALUE);
#endif
        outb(FDC_DOR_VALUE,FDC_BASE_IO+FDC_DOR);
        return 1;
    }
    return 0;
}

/* This will stop the motor on the selected drive */
unsigned int fdc_stop_drive_motor(unsigned int drive_number)
{
    if (drive_number < FDC_MAX_DRIVES) {
        if (drive_number == 0) {
            FDC_DOR_VALUE=(FDC_DOR_VALUE&(~FDC_MOTOR_0));
        } else if (drive_number == 1) {
            FDC_DOR_VALUE=(FDC_DOR_VALUE&(~FDC_MOTOR_1));
        } else if (drive_number == 2) {
            FDC_DOR_VALUE=(FDC_DOR_VALUE&(~FDC_MOTOR_2));
        } else if (drive_number == 3) {
            FDC_DOR_VALUE=(FDC_DOR_VALUE&(~FDC_MOTOR_3));
        } else {
            //not a valid setting
            return 0;
        }
#ifdef DEBUG_FDC
        printf("\nsetting FDC_DOR=0x%x\n",FDC_DOR_VALUE);
#endif
        outb(FDC_DOR_VALUE,FDC_BASE_IO+FDC_DOR);
        return 1;
    }
    return 0;
}

/* This will make the selected drive the active drive for commands */
unsigned int fdc_select_drive_active(unsigned int drive_number)
{
    if (drive_number < FDC_MAX_DRIVES) {
        if (drive_number == 0) {
            FDC_DOR_VALUE=(FDC_DOR_VALUE&0xFC)|0;
        } else if (drive_number == 1) {
            FDC_DOR_VALUE=(FDC_DOR_VALUE&0xFC)|1;
        } else if (drive_number == 2) {
            FDC_DOR_VALUE=(FDC_DOR_VALUE&0xFC)|2;
        } else if (drive_number == 3) {
            FDC_DOR_VALUE=(FDC_DOR_VALUE&0xFC)|3;
        } else {
            //not a valid setting
            return 0;
        }
        outb(FDC_DOR_VALUE,FDC_BASE_IO+FDC_DOR);
        return 1;
    }
    return 0;
}

/* This will start the motor and select the drive for commands */
unsigned int fdc_activate_drive(unsigned int drive_number)
{
    unsigned int result=0;
    if (fdc_select_drive_active(drive_number)) {
        if (fdc_start_drive_motor(drive_number)) {
            result = 1;
        } else {
            result = 0;
        }
    } else {
        result = 0;
    }
    return result;
}

/* This will start the motor and select the drive for commands */
unsigned int fdc_deactivate_drive(unsigned int drive_number)
{
    unsigned int result=0;
    if (fdc_select_drive_active(drive_number)) {
        if (fdc_stop_drive_motor(drive_number)) {
            result = 1;
        } else {
            result = 0;
        }
    } else {
        result = 0;
    }
    return result;
}

/*****************************************************************************
 *
 * The following are commands that the floppy controller can execute
 * Each has their own arguments and values
 *
*****************************************************************************/
#define FDC_MT          0x80  //Multi track treats head 0 and 1 as one large track
#define FDC_MFM         0x40  //selects double density mode
#define FDC_SK          0x20  //skip deleted data or skip

#define FDC_HDS         0x04
#define FDC_DS          0x03  //drive select bits

//our little asm routine
unsigned int fdc_read_data_block(unsigned char* data_block) {
  int i,c,t;
  
  for(i=0;i<512;i++) {
    c=2000000;
    while (c>0) {
      c--;
      if((t=inb(FDC_MSR))&0xC0==0xC0) {
	if(t&0x20==0x20) {
	  data_block[i]=inb(FDC_DATA);
	  c=-10;
	} else printf("Status!!!!\n");      
      }      
      inb(0x80);
    }
    if (c!=-10) {
      printf("TIMEOUT!\n");
      return i;
    }
  }
  return i;
}

unsigned int fdc_command_read_data(unsigned char drive,
                                   unsigned char cylinder,
                                   unsigned char head,
                                   unsigned char sector,
                                   unsigned char* destination)
{
//    unsigned char count=18;

    unsigned char cmd_byte1=0;  //FDC_READ_DATA command and modifiers
    unsigned char cmd_byte2=0;  //HEAD and drive selec
    unsigned char cmd_byte3=0;  //cylinder 0 - 0xFF
    unsigned char cmd_byte4=0;  //head     0 or 1
    unsigned char cmd_byte5=0;  //sector   0 - 0xFF
    unsigned char cmd_byte6=0;  //sector size
    unsigned char cmd_byte7=0;  //sectors in this track
    unsigned char cmd_byte8=0;  //gap 3 length
    unsigned char cmd_byte9=0;  //data length if sector size = 0;


    unsigned char the_data=0;
    unsigned int  data_number=0;
    unsigned int  result_number=0;

    unsigned int successful=0;

    
    FDC_CURRENT_COMMAND=FDC_READ_DATA;

    cmd_byte1=FDC_READ_DATA|FDC_MFM;//|FDC_MT;  //or in the values for MT and MFM and SK
//    cmd_byte1=0xC6;
    cmd_byte2=((head&0x01)<<2)|(drive&FDC_DS);
//    cmd_byte2=0x00;
    cmd_byte3=cylinder;
    cmd_byte4=head;
    cmd_byte5=sector;
    cmd_byte6=0x02;  //2 = 512 bytes per sector
    cmd_byte7=sector;     //sectors in this track
    cmd_byte8=0x1B;  //gap 3 seggested for 1.44MB
    cmd_byte9=0x00;  //sector size is not zero

#ifdef DEBUG_FDC
    printf ("sending read command...\n");

    printf("cmd_byte1=%x\n",cmd_byte1);
    printf("cmd_byte2=%x\n",cmd_byte2);
    printf("cmd_byte3=%x\n",cmd_byte3);
    printf("cmd_byte4=%x\n",cmd_byte4);
    printf("cmd_byte5=%x\n",cmd_byte5);
    printf("cmd_byte6=%x\n",cmd_byte6);
    printf("cmd_byte7=%x\n",cmd_byte7);
    printf("cmd_byte8=%x\n",cmd_byte8);
    printf("cmd_byte9=%x\n",cmd_byte9);
#endif
    fdc_write(cmd_byte1);
    fdc_write(cmd_byte2);
    fdc_write(cmd_byte3);
    fdc_write(cmd_byte4);
    fdc_write(cmd_byte5);
    fdc_write(cmd_byte6);
    fdc_write(cmd_byte7);
    fdc_write(cmd_byte8);
    fdc_write(cmd_byte9);
#ifdef DEBUG_FDC
    printf ("reading...\n");
#endif
//    unsigned char data_array[1024];
//    unsigned char result_array[7];

    
    data_number = fdc_read_data_block(destination);

    do {
        successful=fdc_read(&the_data);
        if (successful == 2) {
            data_number++;
        } else {
            result_number++;
        }
    } while ((result_number < 7)&&successful);

#ifdef DEBUG_FDC
    printf("\nTotal of %d bytes were read and %d result bytes returned.\n",data_number,result_number);
#endif

    FDC_CURRENT_COMMAND = 0;

    if ((data_number == 512)&&(result_number == 7)) {
        return 1;  //successful
    } else {
        return 0;  //failure
    }

}

void fdc_command_read_deleted_data(void);
void fdc_command_write_data(void);
void fdc_command_write_deleted_data(void);
void fdc_command_read_track(void);
void fdc_command_verify(void);

/* Gets the drive version */
/* returns the version number if successuful or 0 on failure */
/* generates no interrupts */
unsigned int fdc_command_version(void)
{
    unsigned char version=0;

    FDC_CURRENT_COMMAND=FDC_VERSION;
    fdc_write(FDC_VERSION);
    fdc_read(&version);
    FDC_CURRENT_COMMAND=0;

    return version;
}

void fdc_command_format_track(void);
void fdc_command_scan_equal(void);
void fdc_command_scan_low_or_equal(void);
void fdc_command_scan_high_or_equal(void);
void fdc_command_calibrate(unsigned char drive)
{
    FDC_CURRENT_COMMAND=FDC_CALIBRATE;

    fdc_write(FDC_CALIBRATE);
    fdc_write(drive&FDC_DS);
    FDC_CURRENT_COMMAND=0;
    


};

/*  */
unsigned int fdc_command_sense_interrupt(void)
{
    unsigned char STR0=0;
    unsigned char PCN=0;
    FDC_CURRENT_COMMAND=FDC_SENSE_INTERRUPT;
    fdc_write(FDC_SENSE_INTERRUPT);
    fdc_read(&STR0);
    if (STR0 != 0x80) {
        //command was not bad so we have to read byte
        fdc_read(&PCN);
    }
    FDC_CURRENT_COMMAND=0;
    return STR0;

}

void fdc_command_specify(void)
{
    FDC_CURRENT_COMMAND=FDC_SPECIFY;
    fdc_write(FDC_SPECIFY);
    fdc_write(0);     //maximum HUT, HLT, SRT wait times
    fdc_write(0x01);  //non_dma_mode
    FDC_CURRENT_COMMAND=0;
};
void fdc_command_sense_drive_status(void);
void fdc_command_drive_specification(void)
{
    FDC_CURRENT_COMMAND=FDC_DRIVE_SPECIFICATION;
    fdc_write(FDC_DRIVE_SPECIFICATION);
    fdc_write(0x00);
    fdc_write(0x20);
    fdc_write(0xc0);

    FDC_CURRENT_COMMAND=0;

}

void fdc_command_seek(unsigned char drive,
                      unsigned char cylinder,
                      unsigned char head)
{
    unsigned char cmd_byte1=0;  //FDC_SEEK command and modifiers
    unsigned char cmd_byte2=0;  //HEAD and drive selec
    unsigned char cmd_byte3=0;  //cylinder 0 - 0xFF
    
    FDC_CURRENT_COMMAND=FDC_SEEK;

    cmd_byte1=FDC_SEEK|FDC_MFM;//|FDC_MT;  //or in the values for MT and MFM and SK
    cmd_byte2=((head&0x01)<<2)|(drive&FDC_DS);
    cmd_byte3=cylinder;

    fdc_write(cmd_byte1);
    fdc_write(cmd_byte2);
    fdc_write(cmd_byte3);

    FDC_CURRENT_COMMAND=0;

}
void fdc_command_configure(void)
{
    fdc_write(FDC_CONFIGURE);
    fdc_write(0);
    fdc_write(0x4F);   //EIS  set fifo 16 bytes
    fdc_write(0);

};
void fdc_command_relative_seek(void);


/*****************************************************************************
 *
 * The following are test routines for building the driver and testing
 * Not intended for any really useful work
 *
*****************************************************************************/


void fdc_setup(void)
{
  unsigned int int_value=0,x;
#ifdef DEBUG_FDC
  printf("Doing Reset...\n");
#endif
  fdc_full_reset(0);

#ifdef DEBUG_FDC
  printf("Specify command...\n");
#endif
  fdc_command_specify();
#ifdef DEBUG_FDC
  printf("Configure command...\n");
#endif
  fdc_command_configure();

#ifdef DEBUG_FDC
  printf("Sensing interrupts...\n");
#endif

  for (x=1; x <= 10;x++) {
    int_value = fdc_command_sense_interrupt();
    if (int_value != 0x80) {
#ifdef DEBUG_FDC
      printf("Test %d = 0x%x\n",x,fdc_command_sense_interrupt());
#endif
    }
  }
}

void fdc_drive_setup(unsigned char drive)
{
  int i;
  unsigned int int_value=0,x;
#ifdef DEBUG_FDC
  printf("Activating drive 0...\n");
#endif
  fdc_activate_drive(drive);

#ifdef DEBUG_FDC
  printf("Calibrating drive 0...\n");
#endif
  fdc_command_calibrate(drive);
  for(i=0;i<1000000;i++);

#ifdef DEBUG_FDC
  printf("Sensing interrupt...\n");
#endif

  for (x=1; x <= 10;x++) {
    int_value = fdc_command_sense_interrupt();
    if (int_value != 0x80) {
#ifdef DEBUG_FDC
      printf("Test %d = 0x%x\n",x,fdc_command_sense_interrupt());
#endif
    }
  }

#ifdef DEBUG_FDC
  printf("Deactivating drive 0...\n");
#endif
  fdc_deactivate_drive(drive);
}

unsigned int fdc_read_sector(unsigned char drive_number,
                             unsigned int sector_number,
                             unsigned char* destination)
{
    //lets convert linear sector numbers into c h s format based on
    //c = 80  0 - 79
    //h = 2   0 - 1
    //s = 18  1 - 18
    unsigned int successful=0;
    unsigned int attempts=3;

    unsigned int max_cylinders=80;
    unsigned int max_heads=2;
    unsigned int max_sectors=18;

    unsigned int read_cylinder=0;
    unsigned int read_head=0;
    unsigned int read_sector=0;

    unsigned int remainder=0;
    

    read_cylinder=sector_number/(max_heads*max_sectors);
    remainder=sector_number%(max_heads*max_sectors);
    read_head=remainder/max_sectors;
    read_sector=remainder%max_sectors + 1;
    

#ifdef DEBUG_FDC
    printf("Attempting TO Read Sector %d, at CYL=%d HD=%d SECT=%d ...",sector_number,read_cylinder,read_head,read_sector);
#endif
    if (read_cylinder < max_cylinders) {
        while ((!successful)&&(attempts > 0)) {
            if (fdc_command_read_data(drive_number,read_cylinder,read_head,read_sector,destination)) {
                successful=1;
                attempts--;
            } else {
                attempts--;
            }
            
        }
    }

#ifdef DEBUG_FDC
    if  (successful) {
        printf("successfully\n");
    } else {
        printf("failed!!!\n");
    }
#endif
    return successful;
}











void fdc_test(void)
{
  unsigned int fdc_ver,s;
  byte buffer[512];
    printf("\nTesting Floppy disk Routines\n");
    fdc_full_reset(0);
    
    printf("Checking Version Information...\n");
    fdc_ver = fdc_command_version();
    if (fdc_ver == 0x80){
        //Standard controller detected
        printf("Standard Floppy Disk Controller Found\n");
    } else if (fdc_ver == 0x90) {
        //Standard controller detected
        printf("Enhanced Floppy Disk Controller Found\n");
    } else {
        //Standard controller detected
        printf("Unknown Floppy Disk Controller Version=0x%x\n",(unsigned char)fdc_ver);
    }
        
/*
    printf("\nTesting activate command\n");
    
    fdc_activate_drive(0);
    printf("You should see the FD0 light on now...\n");
    for(unsigned int delay=0;delay < 1000000;delay++);
    fdc_deactivate_drive(0);
    printf("You should see the FD0 light off now...\n");
    for(unsigned int delay=0;delay < 1000000;delay++);
    

    fdc_activate_drive(1);
    printf("You should see the FD1 light on now...\n");
    for(unsigned int delay=0;delay < 1000000;delay++);
    fdc_deactivate_drive(1);
    printf("You should see the FD1 light off now...\n");
    for(unsigned int delay=0;delay < 1000000;delay++);

    printf("\nTesting sense interrupt command\n");

    unsigned int int_value=0;
    for (unsigned int x=1; x <= 10;x++) {
        int_value = fdc_command_sense_interrupt();
        if (int_value != 0x80) {
            printf("Test %d = 0x%x\n",x,fdc_command_sense_interrupt());
        }
    }
*/
    fdc_setup();
    fdc_drive_setup(0);
    fdc_activate_drive(0);

/*
    for (unsigned int c=0;c < 80;c++) {
        for (unsigned int h=0;h < 2;h++) {
            for (unsigned int s=1;s <= 18;s++) {
                fdc_command_read_data(0,c,h,s,(unsigned char*)0x00800000);
            }
        }
    }
*/
    /*    
    for (s=0;s < 50;s++) {
        
        fdc_read_sector(0,s,buffer);
    }
    */
    fdc_read_sector(0,1,buffer);
    printf("READ : '%s'\n",buffer+3);
    fdc_deactivate_drive(0);


    
}

void fdc_initialize(void)
{
    fdc_setup();
    fdc_drive_setup(1);
    fdc_drive_setup(0);


}



/* init driver */
void main(void)
{
  int i,t;
  MpsRetVal ret;
  char *params;
  char * msg;
  DISK_COMMAND * answ;
  msginfo info;
  


  /* Start initialization. */

  printf("Starting FDC driver\n");
  if ((ret=DriverInit(&params))!=0) {
    printf("[FDC] ***** Error in initialization phase: %s!!!\n",
	   MpsStrerror(ret));
    exit();
  }

  /* Parameters processing. */
  while (*params) {
    // <--- Here params contains the name of parameter ...
    printf("Parameter %s = ",params);
    while (*params++) ;
    // <--- Here params contains the value of parameter. It can be 0 ...
    printf("'%s'\n",params);
    while (*params++) ;
  }

  t=getevent("INTERRUPT6");
  if (t!=0) {
    printf("[FDC] Error getting INTERRUPT6 event\n");
    exit();
  }

  if (DMA_Request_Channel(Floppy_DMA)!=0) {
    printf("Error Allocating DMA Channel!!\n");
    exit();
  } else printf("DMA Channel Allocated.\n");

  if ((ret=DriverStart("hd_device","fdc",DRV_I_MONO,0,1,
       "This driver tries to support the floppy driver"))!=0) {
    printf("[FDC] ***** ERROR: MPS don't allow me to register: %s!!!\n",
	   MpsStrerror(ret));
    DMA_Free_Channel(Floppy_DMA);
    exit();
  }


  fdc_test();

}

