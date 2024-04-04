/*!	\file drivers/ata/IdeAta.c
 *	\brief IDE driver::ATA Protocol.
 *	\author Luca Giovacchini
 *	\date Last update: 2003-11-07
 *	\note Copyright (&copy;) 2003 Luca Giovacchini
 *
 *	This driver is based on Atadrv by Hale Landis
 *	but it is completely rearranged for the minirighi32.
 *	\n
 *	<b>IMPORTANT!!!</b>\n
 *	Here you can find sub implementing tha ATA-Standard protocols.
 *	Each Hd command must be executed throught one of these
 *	protocols.
 *	Parameters meanings are command dependent.
 *	Refer to Ata-4 document to see what protocol to use for each
 *	command and the meaning of each parameter.
 *	Some protocols are not developed exactly as T13 says.
 *
 *	Read IdeDebug for Error Return Code Explain.
 */

#include <const.h>
#include <string.h>

#include <arch/clock.h>
#include <arch/i386.h>
#include <arch/mem.h>
#include <arch/interrupt.h>

#include <kernel/Ide.h>
#include <kernel/IdeLow.h>
#include <kernel/IdeTimer.h>
#include <kernel/IdeDebug.h>
#include <kernel/video.h>

#include <kernel/IdeAta.h>

extern IdeChannel_Struct * CurrentChannel;

// ******************* IdeDeviceDetection *******************
// This sub Detect if some sort of device/s exist 
// in the selected ide channel, it support the single
// device configuration see Ata-4 9.16.2
// **********************************************************
void IdeDeviceDetection (int Dev) 
{
	byte Sc,Sn;

	// Select the device
	SetDevBit(Dev);

	// we must try to issue a command (ex. write and read a readable and writeable register)
	// See Ata-4 9.16.2
	
	// try to write some value (do not use 00 or FF)
	OutPortAta(CC_SECC, 0x55 );
	OutPortAta(CC_SECN, 0xaa );
	OutPortAta(CC_SECC, 0xaa );
	OutPortAta(CC_SECN, 0x55 );
	OutPortAta(CC_SECC, 0x55 );
	OutPortAta(CC_SECN, 0xaa );
   
	// read if last value are stored correctly
	Sc = InPortAta( CC_SECC );
	Sn = InPortAta( CC_SECN );

	if ( ( Sc == 0x55 ) && ( Sn == 0xaa ) )	   
	{
		// we think (but not sure) there is a device but we don't know what type		
		CurrentChannel->Device[Dev].Type=CC_DEVTYPE_UNKN;
	}
	else
	{
		// we think (but not sure) there isn't a device
		CurrentChannel->Device[Dev].Type=CC_DEVTYPE_NONE;
	}
}

// ***************** IdeDeviceTypeDetection *****************
// try to identify the type of device/s present
// **********************************************************
void IdeDeviceTypeDetection (int Dev)
{
	byte Sc,Sn,Cl,Ch,St;

	if ( CurrentChannel->Device[Dev].Type != CC_DEVTYPE_NONE )
	{
		SetDevBit(Dev);

		Sc = InPortAta( CC_SECC );
		Sn = InPortAta( CC_SECN );
		if ( ( Sc == 0x01 ) && ( Sn == 0x01 ) ) 
		{
			Cl = InPortAta( CC_CYLL );
			Ch = InPortAta( CC_CYLH );
			St = InPortAta( CC_STAT );
			if ( ( Cl == 0x14 ) && ( Ch == 0xEB ) )
				CurrentChannel->Device[Dev].Type=CC_DEVTYPE_ATAPI;
			else if ( ( Cl == 0x00 ) && ( Ch == 0x00 ) && ( St != 0x00 ) )
				CurrentChannel->Device[Dev].Type=CC_DEVTYPE_ATA;
			else
				CurrentChannel->Device[Dev].Type=CC_DEVTYPE_UNKN;	
		}
		else
			CurrentChannel->Device[Dev].Type=CC_DEVTYPE_NONE;
	}
}


// ********************* IdeSoftReset ***********************
// Execute a SoftReset protocol (on the channel), after that
// we must wait for device/s previouslt found to be ready
// See ATA-2 Section 9.2, ATA-3 Section 9.2, ATA-4 Section 9.3
// Errors int his sub are not critical
// **********************************************************
int IdeSoftReset( int SkipFlag, int UseInterrupt ) 
{
	byte Sc,Sn,DevCtrl;
	byte Status;
	int Err1=FALSE;
	int Err2=FALSE;
	int Err3=FALSE;
	Timer_Struct Timer;

	// setup register values
	DevCtrl = HDC_DEVC_HD15 | ( UseInterrupt ? 0 : HDC_DEVC_NIEN );

	// initialize the command timeout counter
	Timer=TimerStart(HDC_ATATIMEOUT);

	// Set and then reset the soft reset bit in the Device Control register.
	// for at least x Ns (x may change with Ata version)
	// This causes device 0 be selected   
	// and both device initiate reset protocol.
	// Both Device shall be ready within 31 s
	if ( ! SkipFlag )
	{
		OutPortAta( CC_DEVC, DevCtrl | HDC_DEVC_SRST );
		DELAY400NS;
		OutPortAta( CC_DEVC, DevCtrl );
		DELAY400NS;
	}
	

	// If there is a device 0, wait for device 0 to set BSY=0.
	if ( CurrentChannel->Device[CC_DEV0].Type != CC_DEVTYPE_NONE ) 
	{
		AtapiDelay(CC_DEV0);
		if (WaitForBsy(Timer,CC_STAT))
			Err1=TRUE;
	}	
	

	// If there is a device 1, wait until device 1 allows
	// register access.
	if ( CurrentChannel->Device[CC_DEV1].Type != CC_DEVTYPE_NONE ) {
		AtapiDelay(CC_DEV1);

		while ( TRUE )
		{
			SetDevBit(CC_DEV1);

			Sc = InPortAta( CC_SECC );
			Sn = InPortAta( CC_SECN );
			if ( ( Sc == 0x01 ) && ( Sn == 0x01 ) )
				break;

			if ( TimerElapsed(Timer) )
			{
				Err2=TRUE;		
				break;
			}
			GoIdle();
		}

		// Now check if drive 1 set BSY=0.
		if ( !Err1 && !Err2 ) 
		{
			Status=InPortAta( CC_STAT );
			if ( Status & HDC_STAT_BSY )
				Err3=TRUE;
		}
	}	

	// RESET_DONE:
	SetFirstDevBit();	

	if (Err3)
		return 3;
	else if (Err1 && Err2)
		return 4;
	else if (Err2)
		return 2;
	else if (Err1)
		return 1;
		
	return 0;
}

// ******************* IdeDeviceSelection *******************
// Execute Selection Device Protocol (see Ata-4 9.6)
// Before issue a NonData, DataIn or DataOut protocol for
// a particular device we must wait device to be ready
// This is a little different from ata-4 in signal checked
// **********************************************************
int IdeDeviceSelection( int Dev )
{
	byte Status;
	Timer_Struct Timer;

	// PAY ATTENTION HERE
	// The caller may want to issue a command to a device that doesn't
	// exist (for example, Exec Dev Diag), so if we see this,
	// just select that device, skip all status checking and return.
	// We assume the caller knows what they are doing!

	if ( CurrentChannel->Device[Dev].Type != CC_DEVTYPE_ATA && CurrentChannel->Device[Dev].Type != CC_DEVTYPE_ATAPI )
	{
		// select the device and return
		SetDevBit(Dev);
		return 0; 
	}

	Timer=TimerStart(HDC_ATATIMEOUT);
   // The rest of this is the normal ATA stuff for device selection
   // and we don't expect the caller to be selecting a device that
   // does not exist.
   // We don't know which drive is currently selected but we should
   // wait for it to be not BUSY.  Normally it will be not BUSY
   // unless something is very wrong!
	if (WaitForBsy(Timer,CC_STAT))	
		return 11;		

   // Here we select the drive we really want to work with by
   // putting 0xA0 or 0xB0 in the Drive/Head register (1f6).
	SetDevBit(Dev);

   // If the selected device is an ATA device,
   // wait for it to have READY and SEEK COMPLETE
   // status.  Normally the drive should be in this state unless
   // something is very wrong (or initial power up is still in
   // progress).  For any other type of device, just wait for
   // BSY=0 and assume the caller knows what they are doing.

	while ( TRUE )
	{
		Status = InPortAta( CC_STAT );
		if ( CurrentChannel->Device[Dev].Type == CC_DEVTYPE_ATA )
		{
			if ( ( Status & ( HDC_STAT_BSY | HDC_STAT_RDY | HDC_STAT_SKC ) )
						== ( HDC_STAT_RDY | HDC_STAT_SKC ) )
				break;
		}
		else
		{
			if ( ( Status & HDC_STAT_BSY ) == 0 )
	            break;
		}
		if ( TimerElapsed(Timer) )
			return 12;
	}

	return 0;
}

// *********************** IdeNonData ***********************
// NonData Command Protocol See Ata-4 9.9 for a complete list
// of command executable with this protocol
// **********************************************************
int IdeNonData( int Dev, int Cmd,
                   int Feat, int SecC,
                   dword Cyl, int Head, int Sect, int UseInterrupt )
{
	byte SecCnt;
	byte SecNum;
	byte DevHead;
	byte DevCtrl;
	byte CylLow;
	byte CylHigh;
	byte Status;
	int TimeOut=FALSE;
	int Err=FALSE;
	Timer_Struct Timer;

	DevCtrl = HDC_DEVC_HD15 | (UseInterrupt ? 0 : HDC_DEVC_NIEN);
	DevHead = CurrentChannel->Device[Dev].RegBit;
	DevHead = DevHead | ( Head & 0x4f );
	CylLow = Cyl & 0x00ff;
	CylHigh = ( Cyl & 0xff00 ) >> 8;


	Timer=TimerStart(HDC_ATATIMEOUT);

	if ( Cmd != HDC_CMD_DEVICE_RESET )
	{
		// Select the drive - call the sub_select function.
		// Quit now if this fails.

		Err=IdeDeviceSelection(Dev);
		if ( Err )
			return Err;

		// Set up all the registers except the command register.

		OutPortAta( CC_DEVC, DevCtrl );
		OutPortAta( CC_FEAT, Feat );
		OutPortAta( CC_SECC, SecC );
		OutPortAta( CC_SECN, Sect );
		OutPortAta( CC_CYLL, CylLow );
		OutPortAta( CC_CYLH, CylHigh );
		OutPortAta( CC_DEVH, DevHead );
	}


	OutPortAta( CC_CMD, Cmd );
	DELAY400NS;

	AtapiDelay(CC_DEV0);
	AtapiDelay(CC_DEV1);	

	if ( ( Cmd == HDC_CMD_EXECUTE_DEVICE_DIAGNOSTIC ) &&
       ( CurrentChannel->Device[CC_DEV0].Type == CC_DEVTYPE_NONE ) )
	{
    
		while ( TRUE )
		{
			OutPortAta( CC_DEVH, HDC_DEVH_DEV1 );
			DELAY400NS;
			SecCnt = InPortAta( CC_SECC );
			SecNum = InPortAta( CC_SECN );
			if ( ( SecCnt == 0x01 ) && ( SecNum == 0x01 ) )
				break;
			if ( TimerElapsed(Timer) )
			{
				Err=24;
				break;
			}
		}
	}
	else
	{		
		if ( Cmd == HDC_CMD_DEVICE_RESET || !UseInterrupt )
		{
			TimeOut=WaitForBsy(Timer,CC_ASTAT);
			Err=23;
		}
		else
		{
			TimeOut=WaitForInt(Timer);
			Err=22;	
		}
	}
	// We MUST read status register even if we have a timeout
	// for clear pending interrupt
	// Read the primary status register.  In keeping with the rules
	// stated above the primary status register is read only
	// ONCE.
	Status = InPortAta( CC_STAT );

	// Error if BUSY, DEVICE FAULT, DRQ or ERROR status now.

	if (Err)
		return Err;

	if ( Status & ( HDC_STAT_BSY | HDC_STAT_DF | HDC_STAT_DRQ | HDC_STAT_ERR ) ) 
		return 21;

	return 0;
}

// ******************** IdePioDataInLba *********************
// Decompose Lba 48-bit to be used in Cyl Head Sect Register
// THIS IS NOT A LBA2CHS TRANSLATION BUT A DECOMPOSITION
// OF 48 BIT IN 16+16+16, See IdePioDataIn Coment
// **********************************************************
int IdePioDataInLba(int Dev, int Cmd,
					int Feat, int SecC, uint64 Lba,
					word * Buffer,
					int NumSect, int MultiCnt, int UseInterrupt)
{
	dword Cyl;
	int Head, Sect;

	// Decompose Lba part for Sector register
	Sect = (int) ( Lba & 0x000000ffL );
	Lba = Lba >> 8;
	// Decompose lba part for Cylinder register
	Cyl = (int) ( Lba & 0x0000ffff );
	Lba = Lba >> 16;
	// Decompose last part of lba address to be used in Head 
	// register and set the bit to indicate that this is Lba
	// and not head number
	Head = ( (int) ( Lba & 0x0fL ) ) | HDC_DEVH_LBA;
	// Call Chs function with value meaning Lba address
	return IdePioDataIn( Dev, Cmd, Feat, SecC,
                         Cyl, Head, Sect, Buffer,
                         NumSect, MultiCnt, UseInterrupt);
}

// ********************** IdePioDataIn **********************
// Execute the PioDataIn protocol, this must be used for 
// all PioDataIn Command (see Ata-4 9.7) such as IdentifyDevice.
// Parameter and Register are Command Dependent so they need a
// little explain
// Command like Identify Device want Sect = 0
// But they transfer a "sector" from hd, this is why NumSec is 1
// Command like ReadSector Transfer X Sector so
// Sect=x, and NumSect=x
// Sect : Number of sector to transfer from hd
// NumSect : Number of 256 word to transfer from hd buffer
// MultiCnt : Sector per block in ReadMultiple
// **********************************************************
int IdePioDataIn(   int Dev, int Cmd,
					int Feat, int SecC,
                    dword Cyl, int Head, int Sect,
                    word * Buffer,
                    int NumSect, int MultiCnt, int UseInterrupt)
{
	byte DevHead;
	byte DevCtrl;
	byte CylLow;
	byte CylHigh;
	byte Status;
	dword WordCnt=0;
	int TimeOut=FALSE;
	int Err=FALSE;
	Timer_Struct Timer;
	
	// setup register values and adjust parameters
	DevCtrl = HDC_DEVC_HD15 | (UseInterrupt ? 0 : HDC_DEVC_NIEN);
	DevHead = CurrentChannel->Device[Dev].RegBit;
	// with 4f i let pass head number and lba bit
	DevHead = DevHead | ( Head & 0x4f );
	CylLow = Cyl & 0x00ff;
	CylHigh = ( Cyl & 0xff00 ) >> 8;

	// these commands transfer only 1 sector
	if (	( Cmd == HDC_CMD_IDENTIFY_DEVICE )
			|| ( Cmd == HDC_CMD_IDENTIFY_PACKET_DEVICE )
			|| ( Cmd == HDC_CMD_READ_BUFFER )
		)

		NumSect = 1;

	if ( Cmd != HDC_CMD_READ_MULTIPLE )
		MultiCnt=1;
	
	// Set command time out.
	Timer=TimerStart(HDC_ATATIMEOUT);

	//Select device if it is not busy
	Err=IdeDeviceSelection( Dev );
	if ( Err )
		return Err;	

	// Set up all the registers except the command register.
	OutPortAta( CC_DEVC, DevCtrl );
	OutPortAta( CC_FEAT, Feat );
	OutPortAta( CC_SECC, SecC );
	OutPortAta( CC_SECN, Sect );
	OutPortAta( CC_CYLL, CylLow );
	OutPortAta( CC_CYLH, CylHigh );
	OutPortAta( CC_DEVH, DevHead );
	
	// Start the command by setting the Command register.  The drive
	// should immediately set BUSY status.
	CurrentChannel->IntDone = FALSE;
	OutPortAta(CC_CMD,Cmd);	
	
	// We MUST waste some time by reading the alternate status a few times,
	// for gives drive the time to set BUSY in the status register (400ns).
	DELAY400NS;

	// Loop to read each sector.
	while ( TRUE )
	{
		// NOTE NOTE NOTE ...  The primary status register (1f7) MUST NOT be
		// read more than ONCE for each sector transferred!  When the
		// primary status register is read, the drive resets IRQ 14.  The
		// alternate status register (3f6) can be read any number of times.
		// After INT 7x read the the primary status register ONCE
		// and transfer the 256 words (REP INSW).  AS SOON as BOTH the
		// primary status register has been read AND the last of the 256
		// words has been read, the drive is allowed to generate the next
		// IRQ 14 (newer and faster drives could generate the next IRQ 14 in
		// 50 microseconds or less).  If the primary status register is read
		// more than once, there is the possibility of a race between the
		// drive and the software and the next IRQ 14 could be reset before
		// the system interrupt controller sees it.

		// Wait for INT or wait for not BUSY or wait for time out.

		AtapiDelay(Dev);

		if (UseInterrupt)
			TimeOut=WaitForInt(Timer);
		else
			TimeOut=WaitForBsy(Timer,CC_ASTAT);


		// Read the primary status register even if there was a timeout
		// to clear pending interrupt.  
		// In keeping with the rules stated above the primary status
		// register is read only ONCE.
		Status = InPortAta( CC_STAT );


		if (TimeOut)
			return ( UseInterrupt ? 34 : 35);


		// If BSY=0 and DRQ=1, transfer the data,
		// even if we find out there is an error later.
		if ( ( Status & ( HDC_STAT_BSY | HDC_STAT_DRQ ) ) == HDC_STAT_DRQ )
		{
			// determine the number of sectors to transfer
			// MultiCnt is >1 only on Read Multiple command
			// Read Multiple need that we use Set Multiple to
			// indicate numsect per block to transfer. An interrupt
			// is sent only after each block and not after each sector
			WordCnt = MultiCnt ? MultiCnt : 1;
			if ( WordCnt > NumSect )
				WordCnt = NumSect;
			WordCnt = WordCnt * 256;

			// A sector is 512 byte or 256 word			
			// Do the REP INSW to read the data for one block.
			InPortAtaMul(CC_DATA,Buffer,WordCnt);

			DELAY400NS;    // delay so device can get the status updated

			// Note: The drive should have dropped DATA REQUEST by now.  If there
			// are more sectors to transfer, BUSY should be active now (unless
			// there is an error).

			// Decrement the count of sectors to be transferred
			// and increment buffer address.
			NumSect = NumSect - 1;

			Buffer+=WordCnt;			
		}
		else
			return 32;			

		// So was there any error condition?
		if ( Status & ( HDC_STAT_BSY | HDC_STAT_DF | HDC_STAT_ERR ) )
			return 31;

		// DRQ should have been set -- was it?
		if ( ( Status & HDC_STAT_DRQ ) == 0 )
			return 32;			

		// If all of the requested sectors have been transferred, make a
		// few more checks before we exit.
		if ( NumSect < 1 )
		{
			// Since the drive has transferred all of the requested sectors
			// without error, the drive should not have BUSY, DEVICE FAULT,
			// DATA REQUEST or ERROR active now.
			AtapiDelay(Dev);

			Status = InPortAta( CC_STAT );
			if ( Status & ( HDC_STAT_BSY | HDC_STAT_DF | HDC_STAT_DRQ | HDC_STAT_ERR ) )
				return 33;

			// All sectors have been read without error
			break;   
		}

      // This is the end of the read loop.  If we get here, the loop is
      // repeated to read the next sector.  Go back 
	}
	
	return 0;
}


// ******************** IdePioDataOutLba *********************
// Decompose Lba 48-bit to be used in Cyl Head Sect Register
// THIS IS NOT A LBA2CHS TRANSLATION BUT A DECOMPOSITION
// OF 48 BIT IN 16+16+16, See IdePioDataOut Coment
// **********************************************************
int IdePioDataOutLba(int Dev, int Cmd,
					int Feat, int SecC, uint64 Lba,
					word * Buffer,
					int NumSect, int MultiCnt, int UseInterrupt)
{
	dword Cyl;
	int Head, Sect;

	// Decompose Lba part for Sector register
	Sect = (int) ( Lba & 0x000000ffL );
	Lba = Lba >> 8;
	// Decompose lba part for Cylinder register
	Cyl = (int) ( Lba & 0x0000ffff );
	Lba = Lba >> 16;
	// Decompose last part of lba address to be used in Head 
	// register and set the bit to indicate that this is Lba
	// and not head number
	Head = ( (int) ( Lba & 0x0fL ) ) | HDC_DEVH_LBA;
	// Call Chs function with value meaning Lba address
	return IdePioDataOut( Dev, Cmd, Feat, SecC,
                         Cyl, Head, Sect, Buffer,
                         NumSect, MultiCnt, UseInterrupt);
}

// ********************** IdePioDataOut *********************
// Execute the PioDataOut protocol, this must be used for 
// all PioDataOut Command (see Ata-4 9.8)
// See also IdePioDataIn Comment
// **********************************************************
int IdePioDataOut(  int Dev, int Cmd,
					int Feat, int SecC,
                    dword Cyl, int Head, int Sect,
                    word * Buffer,
                    int NumSect, int MultiCnt, int UseInterrupt)
{
	byte DevHead;
	byte DevCtrl;
	byte CylLow;
	byte CylHigh;
	byte Status;
	int TimeOut=FALSE;
	int loopFlag = TRUE;
	dword WordCnt;
	int Err=FALSE;
	Timer_Struct Timer;

	// setup register values and adjust parameters
	DevCtrl = HDC_DEVC_HD15 | ( UseInterrupt ? 0 : HDC_DEVC_NIEN );
	DevHead = CurrentChannel->Device[Dev].RegBit;
	// with 4f i let pass head number and lba bit
	DevHead = DevHead | ( Head & 0x4F );
	CylLow = Cyl & 0x00FF;
	CylHigh = ( Cyl & 0xFF00 ) >> 8;
	// these commands transfer only 1 sector
	if ( Cmd == HDC_CMD_WRITE_BUFFER )
		NumSect = 1;
	// only Write Multiple and CFA Write Multiple W/O Erase uses multCnt
	if (    ( Cmd != HDC_CMD_WRITE_MULTIPLE )
		&& ( Cmd != HDC_CMD_CFA_WRITE_MULTIPLE_WO_ERASE )
		)
		MultiCnt = 1;

	// Set command time out.
	TimerStart(HDC_ATATIMEOUT);

	//Select device if it is not busy 
	Err=IdeDeviceSelection( Dev );
	if ( Err )
		return Err;

	// Set up all the registers except the command register.
	OutPortAta( CC_DEVC, DevCtrl );
	OutPortAta( CC_FEAT, Feat );
	OutPortAta( CC_SECC, SecC );
	OutPortAta( CC_SECN, Sect );
	OutPortAta( CC_CYLL, CylLow );
	OutPortAta( CC_CYLH, CylHigh );
	OutPortAta( CC_DEVH, DevHead );

	// Start the command by setting the Command register.  The drive
	// should immediately set BUSY status.
	CurrentChannel->IntDone = FALSE;
	OutPortAta(CC_CMD,Cmd);	

	// We MUST waste some time by reading the alternate status a few times,
	// for gives drive the time to set BUSY in the status register (400ns).
	DELAY400NS;

	// Wait for not BUSY or time out. 
	// NOTE: No interrupt is generated for the first sector of a write
	// command.  Well... that's not really true we are working with
	// a PCMCIA PC Card ATA device.
	AtapiDelay(Dev);	
	
	if ( WaitForBsy(Timer,CC_ASTAT) )
		return 47;
	
	// Need to know the current status inside the while
	// (i use astat because it not reset interrupt)
	Status=InPortAta(CC_ASTAT);

	// If we are using interrupts and we just got an interrupt, this is
	// wrong.  The drive must not generate an interrupt at this time.
	if ( loopFlag && ( UseInterrupt && CurrentChannel->IntDone ) )
		return 46;
	
	// This loop writes each sector.
	while ( loopFlag )
	{
		// NOTE NOTE NOTE ...  The primary status register (1f7) MUST NOT be
		// read more than ONCE for each sector transferred!  When the
		// primary status register is read, the drive resets IRQ 14.  The
		// alternate status register (3f6) can be read any number of times.
		// For correct results, transfer the 256 words (REP OUTSW), wait for
		// INT and then read the primary status register.  AS
		// SOON as BOTH the primary status register has been read AND the
		// last of the 256 words has been written, the drive is allowed to
		// generate the next IRQ 14 (newer and faster drives could generate
		// the next IRQ 14 in 50 microseconds or less).  If the primary
		// status register is read more than once, there is the possibility
		// of a race between the drive and the software and the next IRQ 14
		// could be reset before the system interrupt controller sees it.

		// If BSY=0 and DRQ=1, transfer the data,
		// even if we find out there is an error later.

		if ( ( Status & ( HDC_STAT_BSY | HDC_STAT_DRQ ) ) == HDC_STAT_DRQ )
		{
			// determine the number of sectors to transfer

			WordCnt = MultiCnt ? MultiCnt : 1;
			if ( WordCnt > NumSect )
				WordCnt = NumSect;
			WordCnt = WordCnt * 256;

			// Do the REP OUTSW to write the data for one block.
			OutPortAtaMul( CC_DATA, Buffer,WordCnt );

			DELAY400NS;    // delay so device can get the status updated

			// Note: The drive should have dropped DATA REQUEST and
			// raised BUSY by now.

			// Decrement the count of sectors to be transferred
			// and increment buffer address.
			NumSect = NumSect - ( MultiCnt ? MultiCnt : 1 );
			Buffer+=WordCnt;
		}
		else
			return 42;

		// So was there any error condition?
		if ( Status & ( HDC_STAT_BSY | HDC_STAT_DF | HDC_STAT_ERR ) )
			return 41;

		// DRQ should have been set -- was it?
		if ( ( Status & HDC_STAT_DRQ ) == 0 )
			return 42;

		AtapiDelay(Dev);

		// if there was a timeout we must read the status register
		// to clear pending interrupt before exit
		if ( UseInterrupt )
			TimeOut=WaitForInt(Timer);
		else
			TimeOut=WaitForBsy(Timer,CC_ASTAT);


		// Read the primary status register.  In keeping with the rules
		// stated above the primary status register is read only ONCE.
		if ( TimeOut )
			return ( UseInterrupt ? 44 : 45);

		// If all of the requested sectors have been transferred, make a
		// few more checks before we exit.

		if ( NumSect < 1 )
		{
			// Since the drive has transferred all of the sectors without
			// error, the drive MUST not have BUSY, DEVICE FAULT, DATA REQUEST
			// or ERROR status at this time.

			if ( Status & ( HDC_STAT_BSY | HDC_STAT_DF | HDC_STAT_DRQ | HDC_STAT_ERR ) )
				return 43;

			// All sectors have been written without error, go to WRITE_DONE.

			break;

		}

	
		// This is the end of the write loop.  If we get here, the loop
		// is repeated to write the next sector.  Go back
	}

	return 0;
}

