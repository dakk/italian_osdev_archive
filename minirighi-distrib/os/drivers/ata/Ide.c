/*!	\file drivers/ata/Ide.c
 *	\brief IDE driver.
 *	\author Luca Giovacchini
 *	\date Last update:\n
 *		2004-03-24 Andrea Righi:
 *			Added some printk() log messages.\n
 *	\note Copyright (&copy;) 2003 Luca Giovacchini
 *
 *	This driver is based on Atadrv by Hale Landis
 *	but it is completely rearranged for the minirighi32.
 *	\n
 *	<b>IMPORTANT!!!</b>\n
 *	Here you can find what the driver should expose to the rest
 *	of the world.
 *
 *	Read IdeAta, IdeLow, IdeDebug and IdeTimer comments for better
 *	understand how this driver works.
 *	Implementing new ide commands is very very simple.
 *	All the major part of the Ata protocol is in "IdeAta.c".
 *	You only need to look for Ata-4 references, search for a
 *	command and see what protocol and what parameter you have
 *	to use.
 *	I have written specific subroutines to read, write and identify
 *	identifying devices, but they only simplify the interface for
 *	the more generic ata protocol.
 *
 *	I have already set inizialization of Ide Driver in main.c so
 *	you only need to:
 *	\li select the channel you want to use (with SelectAtaChannel);
 *	\li check CurrentChannel to find a specific device (directly
 *	with CurrentChannel var);
 *	\li use one of the specific command sub (i.e. ReadSectorLba,
 *	WriteSectorLba, ...)
 */

#include <string.h>

#include <arch/i386.h>
#include <arch/interrupt.h>
#include <arch/mem.h>
#include <arch/clock.h>

#include <kernel/debug.h>
#include <kernel/IdeTimer.h>
#include <kernel/IdeDebug.h>
#include <kernel/IdeLow.h>
#include <kernel/video.h>
#include <kernel/IdeAta.h>
#include <kernel/keyboard.h>

#include <kernel/Ide.h>

extern IdeChannel_Struct * CurrentChannel;


// static BusMaster_Struct BusMaster; // UNDER CONSTRUCTION


void ShowDeviceSubData(int Dev)
{

	Device_Struct * CurDev=&CurrentChannel->Device[Dev];

	if ( CurDev->Type==CC_DEVTYPE_ATA )
	{
		printk( KERN_INFO "\t\tDevice %d: %s\n", Dev, CurDev->Info.ModelNumber );
		printk( KERN_INFO "\t\t    Type: ATA %d - UDMA %d\n",
			CurDev->Info.HighestAtaSupported,
			CurDev->Info.HighestUDMASupported );
		printk( KERN_INFO "\t\t    Firmware Rev: %s\n", CurDev->Info.FirmwareRev );
		printk( KERN_INFO "\t\t    SerialNumber: %s\n", CurDev->Info.SerialNumber );
		printk( KERN_INFO "\t\t    Capacity: %i MB\n",CurDev->Info.CapacityLba / 2 / 1024 );
	}
	else if ( CurDev->Type==CC_DEVTYPE_NONE )
		printk( KERN_INFO "\t\tDevice %d NOT FOUND\n", Dev );
	else if ( CurDev->Type==CC_DEVTYPE_ATAPI )
		printk( KERN_INFO "\t\tDevice %d FOUND Type: ATAPI\n", Dev );
	else
		printk( KERN_INFO "\t\tDevice %d FOUND Type: UNKNOW\n", Dev );
}

void ShowIdeSubData()
{
	printk( KERN_INFO "%s():\n", __FUNCTION__ );
	SelectAtaChannel(CC_PRIMARY);
	printk( KERN_INFO "\tPRIMARY CHANNEL\n" );
	ShowDeviceSubData(CC_DEV0);
	ShowDeviceSubData(CC_DEV1);
	SelectAtaChannel(CC_SECONDARY);
	printk( KERN_INFO "\tSECONDARY CHANNEL\n" );
	ShowDeviceSubData(CC_DEV0);
	ShowDeviceSubData(CC_DEV1);
}


// ********************* ScanDevice *************************
// Initialize, reset and scan for all device in a channel
// The channel must be selected before calling this
// All information were stored in CurrentChannel global structure
// **********************************************************
int ScanDevice(int UseInterrupt)
{
	byte DevCtrl;
	int Err=FALSE;

	// HDC_DEVC_HD15 is not needed in recent ata (is for compatibility)
	DevCtrl= HDC_DEVC_HD15 | ( UseInterrupt ? 0 : HDC_DEVC_NIEN );

	// Send To Device Control Register his Initialization Parameter
	OutPortAta(CC_DEVC,DevCtrl);

	// Try to chek for some sort of device before
	// execute some sort of command on it
	IdeDeviceDetection(CC_DEV0);
	IdeDeviceDetection(CC_DEV1);

	// select device 0
	SetDevBit(CC_DEV0);
	// Know that you know what devices exist you can
	// do a soft reset. The reset is for both device,
	// after the reset we will select Dev0 if possible
	Err=IdeSoftReset( FALSE, UseInterrupt );

	// Now we can issue command because we know how many devices
	// are on the channel and we have done the reset on they so:
	// is device really there? is it ATA or ATAPI?
	IdeDeviceTypeDetection(CC_DEV0);
	IdeDeviceTypeDetection(CC_DEV1);

	// Select first selectable device
	SetFirstDevBit();
	// take in mind that error from this function
	// are not critical, the caller can continue
	// to do his normal operation in respect
	// to DeviceType field of CurrentChannel->Device[Dev]
	return Err;
	// END: Now we have each device type in CurrentChannel
}


// ********************* IdentifyDevice *********************
// Execute Identify Device Command
// Store in Buffer the 256 word of Device Info
// Translate Buffer in DeviceInfo_Struct
// This sub works only on Ata device, to get information from 
// Atapi you should use IdentifyPacketDevice command
// **********************************************************
int IdentifyDevice (int Dev) 
{
	int i;
	word Buffer[256];
	word * Tmp;
	int Err=0;
	DeviceInfo_Struct * TmpDev=(DeviceInfo_Struct *) &(CurrentChannel->Device[Dev].Info);

	//kprintf("\n\rIdentify Dev=%d",Dev);

	// Be sure to reset the buffer
	bzero( Buffer, sizeof(Buffer) );

	// Get 256 word of device info
	Err = IdePioDataInLba(Dev, HDC_CMD_IDENTIFY_DEVICE, 0, 0, 0L, (word *) &Buffer , 1, 0, TRUE);
	if ( Err ) 
		return Err;

	// now the buffer contain raw word of info
	// we must translate it in a more readable struct
	//kprintf("\n\r Fill Structure");
	TmpDev->Ata			=(byte) GETBITW(Buffer[0],15);
	TmpDev->Removable		=(byte) GETBITW(Buffer[0],7);
	TmpDev->NotRemovable		=(byte) GETBITW(Buffer[0],6);
	TmpDev->LogicalCylinders	=(word) Buffer[1];
	TmpDev->LogicalHeads		=(word) Buffer[3];
	TmpDev->LogicalSectorsPerTrack=(word) Buffer[6];

	Tmp=(word *) &(TmpDev->SerialNumber);
	for (i=10;i<=19;i++)
	{
		*Tmp=SWAPBYTEW(Buffer[i]);
		Tmp++;
	}
	TmpDev->SerialNumber[21]='\0';
	

	Tmp=(word *) &(TmpDev->FirmwareRev);
	for (i=23;i<=26;i++)
	{
		*Tmp=SWAPBYTEW(Buffer[i]);
		Tmp++;
	}
	TmpDev->FirmwareRev[9]='\0';

	Tmp=(word *) &(TmpDev->ModelNumber);
	for (i=27;i<=46;i++)
	{
		*Tmp=SWAPBYTEW(Buffer[i]);
		Tmp++;
	}
	TmpDev->ModelNumber[41]='\0';

	TmpDev->MaxMulitSectPerBlock	= (byte) GETBYTEW(Buffer[47],0);
	TmpDev->StandByTimerSupport	= (byte) GETBITW(Buffer[49],13);
	TmpDev->IORDYSupport     	= (byte) GETBITW(Buffer[49],11);
	TmpDev->IORDYDisabled      	= (byte) GETBITW(Buffer[49],10);	
	TmpDev->PioMode			= (byte) GETBYTEW(Buffer[51],1);
	TmpDev->ValidUDMAMode		= (byte) GETBITW(Buffer[53],2);
	TmpDev->ValidCycleTime		= (byte) GETBITW(Buffer[53],1);
	TmpDev->ValidCurLogicalValue= (byte) GETBITW(Buffer[53],0);
	TmpDev->CurLogicalCylinders = (word) Buffer[54];
	TmpDev->CurLogicalHeads     = (word) Buffer[55];
	TmpDev->CurLogicalSectorsPerTrack = (word) Buffer[56];
	TmpDev->CapacityInSectors   = (dword) MAKEDWORDW(Buffer[58],Buffer[57]);
	TmpDev->ValidCurMultiSectPerInt  = (byte) GETBITW(Buffer[59],8);
	TmpDev->CurMultiSectPerInt  = (byte) GETBYTEW(Buffer[59],0);
	TmpDev->CapacityLba         = (dword) MAKEDWORDW(Buffer[61],Buffer[60]);

	TmpDev->MultiDMASupported = ((byte) GETBYTEW(Buffer[63],1)) & 0x07; // only 3 least sig bit
	TmpDev->MultiDMASelected  = ((byte) GETBYTEW(Buffer[63],0)) & 0x07; // only 3 least sig bit

	TmpDev->AdvPioModeSupported = (byte) GETBYTEW(Buffer[64],0);
	TmpDev->MinCycleTime        = (word) Buffer[65];
	TmpDev->RecCycleTime        = (word) Buffer[66];
	TmpDev->MinPioCycleTime     = (word) Buffer[67];
	TmpDev->MinPioCyleTimeFlow  = (word) Buffer[68];
	TmpDev->QueueDepth          = (byte) GETBYTEW(Buffer[75],0) & 0x1F;
	
	TmpDev->AtaSupported        = (word) Buffer[80];

	i=14;
	while ( i>0 && (GETBITW(Buffer[80],i)==0) )
		i--;

	TmpDev->HighestAtaSupported=(byte) i;

	if ( Buffer[81]==0xFFFF )
		TmpDev->MinorVersion    = (word) Buffer[81];
	else
		TmpDev->MinorVersion    = (word) Buffer[81];
	
	TmpDev->SFSNopSupported            = (byte) GETBITW(Buffer[82],14);
	TmpDev->SFSReadBufferSupported     = (byte) GETBITW(Buffer[82],13);
	TmpDev->SFSWriteBufferSupported    = (byte) GETBITW(Buffer[82],12);
	TmpDev->SFSProtectedAreaSupported  = (byte) GETBITW(Buffer[82],10);
	TmpDev->SFSDeviceResetSupported    = (byte) GETBITW(Buffer[82],9);
	TmpDev->SFSServiceIntSupported     = (byte) GETBITW(Buffer[82],8);
	TmpDev->SFSReleaseIntSupported     = (byte) GETBITW(Buffer[82],7);
	TmpDev->SFSLookAheadSupported      = (byte) GETBITW(Buffer[82],6);
	TmpDev->SFSWriteCacheSupported     = (byte) GETBITW(Buffer[82],5);
	TmpDev->SFSPacketSupported         = (byte) GETBITW(Buffer[82],4);
	TmpDev->SFSPowerManagSupported     = (byte) GETBITW(Buffer[82],3);
	TmpDev->SFSRemovableMediaSupported = (byte) GETBITW(Buffer[82],2);
	TmpDev->SFSSecurityModeSupported   = (byte) GETBITW(Buffer[82],1);
	TmpDev->SFSSmartSupported          = (byte) GETBITW(Buffer[82],0);
	TmpDev->SFSRemMediaNotifSupported  = (byte) GETBITW(Buffer[83],4);
	TmpDev->SFSAdvPowerManagSupported  = (byte) GETBITW(Buffer[83],3);
	TmpDev->SFSCFASupported            = (byte) GETBITW(Buffer[83],2);
	TmpDev->SFSRWDmaQueuedSupported    = (byte) GETBITW(Buffer[83],1);
	TmpDev->SFSDownMicrocodeSupported  = (byte) GETBITW(Buffer[83],1);

	TmpDev->SFEServiceIntEnalbed       = (byte) GETBITW(Buffer[85],8);
	TmpDev->SFEReleaseIntEnabled       = (byte) GETBITW(Buffer[85],7);
	TmpDev->SFELookAheadEnabled        = (byte) GETBITW(Buffer[85],6);
	TmpDev->SFEWriteCacheEnabled       = (byte) GETBITW(Buffer[85],5);
	TmpDev->SFESecurityModeEnabled     = (byte) GETBITW(Buffer[85],1);
	TmpDev->SFESmartEnabled            = (byte) GETBITW(Buffer[85],0);
	TmpDev->SFERemMediaNotifEnabled    = (byte) GETBITW(Buffer[86],4);
	TmpDev->SFEAdvPowerManagEnabled    = (byte) GETBITW(Buffer[86],3);

	TmpDev->UDMASelected = ((byte) GETBYTEW(Buffer[88],1)) & 0x3F; // filtro i primi 2 bit
	TmpDev->UDMASupported =((byte) GETBYTEW(Buffer[88],0)) & 0x3F;
	
	i=5;
	while ( i>=0 && (GETBITW(Buffer[88],i)==0) )
		i--;

	TmpDev->HighestUDMASupported=(byte) i;

	TmpDev->SecurityEraseTime    = (word) Buffer[89];
	TmpDev->SecurityEnEraseTime  = (word) Buffer[90];
	TmpDev->CurAPMValue          = (word) Buffer[91];

	TmpDev->SecurityLevel        = (byte) GETBITW(Buffer[128],8);
	TmpDev->SecurityEnErase      = (byte) GETBITW(Buffer[128],5);
	TmpDev->SecurityCountExpired = (byte) GETBITW(Buffer[128],4);
	TmpDev->SecurityFrozen       = (byte) GETBITW(Buffer[128],3);
	TmpDev->SecurityLocked       = (byte) GETBITW(Buffer[128],2);
	TmpDev->SecurityEnabled      = (byte) GETBITW(Buffer[128],1);
	TmpDev->SecuritySupported    = (byte) GETBITW(Buffer[128],0);

	Tmp=(word *) &(TmpDev->VendorSpecific);
	for (i=129;i<=159;i++)
	{
		*Tmp=(word) Buffer[i];
		Tmp++;
	}
	return 0;
}


void ShowDeviceDataRowName(char * Name,int Col)
{
	kprintf(Name);
	gotoxy(Col,-1);
	kprintf("=");
}


void ShowDeviceDataRowValue(char * Name, int Col, char * Format, dword Value)
{
	ShowDeviceDataRowName(Name,Col);
	kprintf(Format, Value);
}

void ShowDeviceDataRowString(char * Name, int Col, char * Value)
{
	ShowDeviceDataRowName(Name,Col);
	kprintf("%s", Value);
}


void ShowDeviceDataRowSup(char * Name,int Col, byte Supported)
{
	ShowDeviceDataRowName(Name,Col);
	kprintf("Supported:%s",Bit2YesNo(Supported));
}

void ShowDeviceDataRowSupEn(char * Name,int Col,byte Supported,byte Enabled)
{
	ShowDeviceDataRowName(Name,Col);
	kprintf("Supported:%s",Bit2YesNo(Supported));
	kprintf("  -  ");
	kprintf("Enabled:%s",Bit2YesNo(Enabled));
}

void ShowDeviceDataRowBitNum(word BitCode,byte StartFrom, byte EndTo, byte StartPrintNum)
{
	int i;
	byte Val=BitCode;

	if ( StartFrom )
		Val=Val >> StartFrom;

	for (i=0;i<=EndTo-StartFrom;i++)
	{
		if (Val & 0x01)
			kprintf("%d  ",StartPrintNum+i);
		Val=Val >> 1;
	}
}


void ShowDeviceData(int Dev,int Col)
{

	DeviceInfo_Struct * TmpDev=(DeviceInfo_Struct *) &(CurrentChannel->Device[Dev].Info);

	kprintf("\n\r***** GENERAL INFORMATION *****");
	ShowDeviceDataRowString("\n\rModel Name",Col,TmpDev->ModelNumber);	
	ShowDeviceDataRowString("\n\rFirmware Revision",Col,TmpDev->FirmwareRev);
	ShowDeviceDataRowString("\n\rSerial Number",Col,TmpDev->SerialNumber);

	ShowDeviceDataRowName("\n\rType",Col);
	if (CurrentChannel->Device[Dev].Type==CC_DEVTYPE_ATA )
		kprintf("ATA");
	else if (CurrentChannel->Device[Dev].Type==CC_DEVTYPE_ATAPI )
		kprintf("ATAPI");
	else if (CurrentChannel->Device[Dev].Type==CC_DEVTYPE_UNKN )
		kprintf("UNKNOW");

	if ( TmpDev->Removable )
		kprintf(" - Removable");
	if ( TmpDev->NotRemovable )
		kprintf(" - Not Removable");


	kprintf("\n\r\n\r***** STANDARD SUPPORTED AND SELECTED *****");

	ShowDeviceDataRowName("\n\rAta Supported",Col);	
	if ( (TmpDev->AtaSupported == 0x0000) || (TmpDev->AtaSupported == 0xFFFF) )
		kprintf("UNKNOW");
	else
		// bit 0 is reserved
		ShowDeviceDataRowBitNum(TmpDev->AtaSupported,1,14,1);

	ShowDeviceDataRowName("\n\rUdma Supported",Col);	
	if ( TmpDev->ValidUDMAMode )
	{
		
		if ( TmpDev->UDMASupported )
			// Udma star from 0
			ShowDeviceDataRowBitNum((word) TmpDev->UDMASupported,0,7,0);			
		else
			kprintf("NONE");
	}
	else
		kprintf("UNKNOW");


	ShowDeviceDataRowName("\n\rUdma Selected",Col);
	if ( TmpDev->ValidUDMAMode )
	{
		if ( TmpDev->UDMASelected )
			ShowDeviceDataRowBitNum((word) TmpDev->UDMASelected,0,7,0);			
		else
			kprintf("NONE");
	}
	else
		kprintf("UNKNOW");

	ShowDeviceDataRowName("\n\rMultiDma Supported",Col);
	if ( TmpDev->MultiDMASupported )
		ShowDeviceDataRowBitNum((word) TmpDev->UDMASupported,0,2,0);		
	else
		kprintf("NONE");

	ShowDeviceDataRowName("\n\rMultiDma Selected",Col);
	if ( TmpDev->MultiDMASelected  )
		ShowDeviceDataRowBitNum((word) TmpDev->UDMASelected,0,2,0);		
	else
		kprintf("NONE");


	ShowDeviceDataRowValue("\n\rStandard Pio Mode",Col,"%d",TmpDev->PioMode);

	ShowDeviceDataRowName("\n\rAdvanced Pio Mode Supported",Col);	
	if ( TmpDev->AdvPioModeSupported )
		ShowDeviceDataRowBitNum(TmpDev->AdvPioModeSupported,0,7,3);	
	else
		kprintf("NONE");
	
	WaitKeyPress(MSG_PRESSKEY);	

	kprintf("\n\r\n\r***** DEVICE LOGICAL INFORMATION *****");
	ShowDeviceDataRowValue("\n\rLogical Cylinders",Col,"%u",TmpDev->LogicalCylinders);
	ShowDeviceDataRowValue("\n\rLogical Heads",Col,"%u",TmpDev->LogicalHeads);
	ShowDeviceDataRowValue("\n\rLogical Sectors",Col,"%u",TmpDev->LogicalSectorsPerTrack);
	if ( TmpDev->ValidCurLogicalValue )
	{
		ShowDeviceDataRowValue("\n\rCurrent Logical Cylinders",Col, "%u",TmpDev->CurLogicalCylinders);
		ShowDeviceDataRowValue("\n\rCurrent Logical Heads",Col,"%u",TmpDev->CurLogicalHeads);
		ShowDeviceDataRowValue("\n\rCurrent Logical Sectors",Col,"%u",TmpDev->CurLogicalSectorsPerTrack);
	}
	else
	{
		ShowDeviceDataRowString("\n\rCurrent Logical Cylinders",Col,"UNKNOW");
		ShowDeviceDataRowString("\n\rCurrent Logical Heads",Col,"UNKNOW");
		ShowDeviceDataRowString("\n\rCurrent Logical Sectors",Col,"UNKNOW");
	}
	ShowDeviceDataRowValue("\n\rCapacity Sectors Current CHS",Col,"%u",TmpDev->CapacityInSectors);
	ShowDeviceDataRowValue("\n\rCapacity Sectors",Col,"%u",TmpDev->CapacityLba);
	ShowDeviceDataRowValue("\n\rCapacity in MB",Col,"%u",TmpDev->CapacityLba / 2 / 1024);

	ShowDeviceDataRowSupEn("\n\rIORDY",Col,TmpDev->IORDYSupport,TmpDev->IORDYDisabled);
	
	WaitKeyPress(MSG_PRESSKEY);	

	kprintf("\n\r\n\r***** DEVICE FEATURES AND SETTING INFORMATION *****");
	ShowDeviceDataRowSup("\n\rR W Dma Queued",Col,TmpDev->SFSRWDmaQueuedSupported);
	ShowDeviceDataRowValue("\n\rQueue Depth",Col,"%u",TmpDev->QueueDepth);
	ShowDeviceDataRowValue("\n\rMax Sect Per Block (in Multi)",Col,"%u",TmpDev->MaxMulitSectPerBlock);
	ShowDeviceDataRowSupEn("\n\rAdv Power Management",Col,TmpDev->SFSAdvPowerManagSupported,TmpDev->SFEAdvPowerManagEnabled);
	ShowDeviceDataRowValue("\n\rAdv Power Management Level",Col,"%u",TmpDev->CurAPMValue);

	ShowDeviceDataRowSupEn("\n\rLook Ahead",Col,TmpDev->SFSLookAheadSupported,TmpDev->SFELookAheadEnabled);
	ShowDeviceDataRowSupEn("\n\rRelease Interrupt",Col,TmpDev->SFSReleaseIntSupported,TmpDev->SFEReleaseIntEnabled);
	ShowDeviceDataRowSupEn("\n\rRemovable Media Notif",Col,TmpDev->SFSRemMediaNotifSupported,TmpDev->SFERemMediaNotifEnabled);
	ShowDeviceDataRowSupEn("\n\rService Interrupt",Col,TmpDev->SFSServiceIntSupported,TmpDev->SFEServiceIntEnalbed);
	ShowDeviceDataRowSupEn("\n\rSmart",Col,TmpDev->SFSSmartSupported,TmpDev->SFESmartEnabled);
	ShowDeviceDataRowSupEn("\n\rWrite Cache",Col,TmpDev->SFSWriteCacheSupported,TmpDev->SFEWriteCacheEnabled);
	ShowDeviceDataRowSup("\n\rRead Buffer",Col,TmpDev->SFSReadBufferSupported);
	ShowDeviceDataRowSup("\n\rWrite Buffer",Col,TmpDev->SFSWriteBufferSupported);
	ShowDeviceDataRowSup("\n\rNop",Col,TmpDev->SFSNopSupported);
	ShowDeviceDataRowSup("\n\rPacket",Col,TmpDev->SFSPacketSupported);
	ShowDeviceDataRowSup("\n\rPower Management",Col,TmpDev->SFSPowerManagSupported);
	ShowDeviceDataRowSup("\n\rProtected Area",Col,TmpDev->SFSProtectedAreaSupported);
	ShowDeviceDataRowSup("\n\rDownload Microcode",Col,TmpDev->SFSDownMicrocodeSupported);
	ShowDeviceDataRowSup("\n\rCFA",Col,TmpDev->SFSCFASupported);
	ShowDeviceDataRowSup("\n\rDevice Reset",Col,TmpDev->SFSDeviceResetSupported);
	ShowDeviceDataRowSup("\n\rStandBy Timer",Col,TmpDev->StandByTimerSupport);

	WaitKeyPress(MSG_PRESSKEY);	


	kprintf("\n\r\n\r***** DEVICE TIMING INFORMATION *****");
	ShowDeviceDataRowSup("\n\rStandBy Timer",Col,TmpDev->StandByTimerSupport);
	
	if (TmpDev->ValidCycleTime )
	{
		ShowDeviceDataRowValue("\n\rMin Multi Dma Cycle Time (ns)",Col,"%u",TmpDev->MinCycleTime);
		ShowDeviceDataRowValue("\n\rRecc MultiDma Cycle Time (ns)",Col,"%u",TmpDev->RecCycleTime);
		ShowDeviceDataRowValue("\n\rMin Pio Cycle Time (ns)",Col,"%u",TmpDev->MinPioCycleTime);
		ShowDeviceDataRowValue("\n\rMin Pio Cycle Time Flow (ns)",Col,"%u",TmpDev->MinPioCyleTimeFlow);
	}
	else
	{
		kprintf("\n\rTiming Information Unavaiable");
	}

	kprintf("\n\r\n\r***** SECURITY INFORMATION *****");
	ShowDeviceDataRowSupEn("\n\rSecurity",Col,TmpDev->SecuritySupported,TmpDev->SecurityEnabled);
	ShowDeviceDataRowSupEn("\n\rSecurity Mode",Col,TmpDev->SFSSecurityModeSupported,TmpDev->SFESecurityModeEnabled);
	ShowDeviceDataRowValue("\n\rSecurity Erase Time (min)",Col,"%u",(int) TmpDev->SecurityEraseTime*2);
	ShowDeviceDataRowValue("\n\rEnh Security Erase Time (min)",Col,"%u",(int) TmpDev->SecurityEnEraseTime*2);
	
	ShowDeviceDataRowName("\n\rSecurity Level",Col);	
	if ( TmpDev->SFESecurityModeEnabled )
	{
		if (TmpDev->SecurityLevel)
			kprintf("Max");
		else
			kprintf("High");
	}
	else
	{
		kprintf("Disabled");
	}

	ShowDeviceDataRowString("\n\rSecurity Frozen",Col,Bit2YesNo(TmpDev->SecurityFrozen));
	kprintf("\n\rSecurity CountExpired");
	gotoxy(Col,-1);
	kprintf("=%s",Bit2YesNo(TmpDev->SecurityCountExpired));
	kprintf("\n\rSecurity Locked");
	gotoxy(Col,-1);
	kprintf("=%s",Bit2YesNo(TmpDev->SecurityLocked));
	ShowDeviceDataRowSup("\n\rSecurity Enh Erase Supported",Col,TmpDev->SecurityEnErase);

	kprintf("\n\r\n\r END DEVICE INFORMATION \n\r");
}



// ************************ InitIde *************************
// Initialization of the driver.
// Prepare data structure, check fo device on each channel
// and fill structure with channel and device info
// **********************************************************
void __INIT__ InitIde()
{
	extern void Ide_Handler_Pri, Ide_Handler_Sec;

	// --- modified by: Andrea Righi -------------------------------//
	// Install the IRQ handlers for both IDE channels		//
	install_trap_handler(IDEP_IRQ, &Ide_Handler_Pri);
	install_trap_handler(IDES_IRQ, &Ide_Handler_Sec);
	// -------------------------------------------------------------//

	// Initialize structure for primary channel
	SelectAtaChannel(CC_PRIMARY);
	SetAtaRegisterIoPort(HDC_BASEPRI,HDC_BASEPRI+HDC_CONTROLGAP,HDC_INTPRI);

	// Initialize devices
	ScanDevice(TRUE);
	if ( CurrentChannel->Device[CC_DEV0].Type==CC_DEVTYPE_ATA )
		IdentifyDevice(CC_DEV0);
	if ( CurrentChannel->Device[CC_DEV1].Type==CC_DEVTYPE_ATA )
		IdentifyDevice(CC_DEV1);

	// Initialize structure for secondary channel
	SelectAtaChannel(CC_SECONDARY);
	SetAtaRegisterIoPort(HDC_BASESEC,HDC_BASESEC+HDC_CONTROLGAP,HDC_INTSEC);

	// Initialize devices
	ScanDevice(TRUE);
	if ( CurrentChannel->Device[CC_DEV0].Type==CC_DEVTYPE_ATA )
		IdentifyDevice(CC_DEV0);
	if ( CurrentChannel->Device[CC_DEV1].Type==CC_DEVTYPE_ATA )
		IdentifyDevice(CC_DEV1);

	ShowIdeSubData();
}



// ******************** ReadSectorsLba **********************
// Read one or more sector of data from hd
// **********************************************************
int ReadSectorsLba(int Dev, uint64 Lba, int NumSect, word * Buffer,int UseInterrupt)
{

	// kprintf("\n\rReadSectorLba: Dev=%d,Lba=%i,NumSect=%i,UseInt=%i",Dev,(int) Lba,NumSect,UseInterrupt);
	return IdePioDataInLba(Dev,HDC_CMD_READ_SECTORS,0,NumSect,Lba,Buffer,NumSect,1,UseInterrupt);
}

// ******************** WriteSectorsLba *********************
// Write one or more sector of data from hd
// **********************************************************
int WriteSectorsLba(int Dev, uint64 Lba, int NumSect, word * Buffer,int UseInterrupt)
{
	return IdePioDataOutLba(Dev,HDC_CMD_WRITE_SECTORS,0,NumSect,Lba,Buffer,NumSect,1,UseInterrupt);
}














// **********************************************************************
// **********************************************************************
// The next section is under construction and must no be used
// This is not beta nor alpha sub but very very very incomplete code
// Don't try to execute - it can damage your computer
// **********************************************************************
// **********************************************************************

// UNDER CONSTRUCTION
//See SFF-8038-r01  o Idems100 (Bus Master Programming Interface 1.0)
/*

int PciConfig(dword RegAddr)
{
	// dword Off;
	// dword Seg;
	// uint64 Lw;

	kprintf("\n\rPCI CONFIG");
	// check reg address

	if ( RegAddr & 0x0007 )          // error if regs addr
	{
		kprintf("->RegAddress Error");
		return 1;                  // are not xxx0h or xxx8h
	}

	// save the address of the bus master (SFF-8038) regs

	//dma_pci_sff_reg_addr = busMasterRegs = regAddr;
	BusMaster.BaseAddress=RegAddr;
	// disable if reg address is zero

	if ( ! RegAddr )              // if zero,
	{
		kprintf("->RegAddr Zero");
		return 0;                  // PCI DMA is disabled.
	}

	// the bus master must be setup now

	

	if ( !( inportb( BusMaster.BaseAddress  + BM_STATUS_REG ) &
			( BM_SR_MASK_DRV1 | BM_SR_MASK_DRV0 ) ) )
   {
		kprintf("->DRIVES CAN'T DO DMA");
		return 2;
   }

   // set up the PRD buffer address

   seg = FP_SEG( (unsigned char far *) prdBuf );
   off = FP_OFF( (unsigned char far *) prdBuf );
   off = off + 16;
   off = off >> 4;
   seg = seg + off;
   dma_pci_prd_addr = seg;
   prdBufPtr = MK_FP( seg, 0 );
   lw = seg;
   lw = lw << 4;
   prdBufPtrLow16 = (unsigned int) ( lw & 0x0000ffffL );
   prdBufPtrHigh16 = (unsigned int) ( ( lw & 0xffff0000L ) >> 16 );

	Seg=SEGMENT( (byte *) BusMaster.PrdBuf );
	Off=OFFSET( (byte *) BusMaster.PrdBuf );
	Off=(Off + 16) >> 4;
	Seg=Seg + Off;
	BusMaster.DmaPciPrdAddr=Seg;
	BusMaster.PrdBufPtr=LINEAR(Seg,0);
	Lw=Seg << 4;
	BusMaster.PrdBufPtrLow  = (dword) ( Lw & 0x0000ffffL );
	BusMaster.PrdBufPtrHigh = (dword) ( ( Lw & 0xffff0000L ) >> 16 );

   //  DEVO FARE UN POP DAL FRAME PERCHE IL DMA VEDE SOLO I PRIMI 16 MB
   //  USO POI PHYSICAL E PRENDO LA PARTE ALTA E BASSA




   // read the BM status reg and save the upper 3 bits.

	BusMaster.StatReg = inportb( BusMaster.BaseAddress + BM_STATUS_REG );
	BusMaster.StatReg = BusMaster.StatReg & 0xe0;

   return 0;
}

*/

/*
// UNDER CONSTRUCTION
 int DmaPciAtaLba( int Dev, int Cmd,
                            int Feat, int SecC,
                            uint64 Lba,
                            unsigned Seg, unsigned Off )
{
   dword Cyl;
   int Head, Sect;

   Sect = (int) ( Lba & 0x000000FFL );
   Lba = Lba >> 8;
   Cyl = (int) ( Lba & 0x0000FFFFL );
   Lba = Lba >> 16;
   Head = ( (int) ( Lba & 0x0FL ) ) | 0x40;

   return DmaPciAta( Dev, Cmd,
                       Feat, SecC,
                       Cyl, Head, Sect,
                       Seg, Off );
}

void EnableChanPci()
{
	OutPortPci( BM_COMMAND_REG, BusMaster.RWControl | BM_CR_MASK_START );
}
void DisableChanPci()
{
	OutPortPci( BM_COMMAND_REG, BM_CR_MASK_STOP );
}
void ClearChanPci()
{
	OutPortPci( BM_STATUS_REG, BusMaster.StatReg | BM_SR_MASK_INT | BM_SR_MASK_ERR );
}

void OutPortPci(word Port, byte Val)
{
	outportb( BusMaster.BaseAddress + Port,Val );
}

byte InPortPci(word Port)
{
	return inportb( Port );
}

*/

/*
// UNDER CONSTRUCTION
int DmaPciAta( int Dev, int Cmd,
                 int Feat, int SecC,
                 dword Cyl, int Head, int Sect,
                 unsigned Seg, unsigned Off )

{
   byte DevHead;
   byte DevCtrl;
   byte CylLow;
   byte CylHigh;
   byte Status;
   int ns;
   int Err=0;

   // setup register values

   DevCtrl = 0;      // in the old days we would set CB_DC_HD15 = 1
   DevHead = CurrentChannel->Device[Dev].RegBit;
   DevHead = DevHead | ( Head & 0x4F );
   CylLow = Cyl & 0x00FF;
   CylHigh = ( Cyl & 0xFF00 ) >> 8;

   // Quit now if the command is incorrect.

   if ( ( Cmd != HDC_CMD_READ_DMA ) && ( Cmd != HDC_CMD_WRITE_DMA ) )
   {
      return 1;
   }

   // Quit now if no dma channel set up.

   if ( ! BusMaster.BaseAddress  )
   {
      return 1;
   }

   // set up the dma transfer

   ns = SecC;
   if ( ! ns )
      ns = 256;
   SetUpXfer( Cmd == HDC_CMD_WRITE_DMA, ns * 512L, Seg, Off );

   // Set command time out.

   TimerStart(HDC_ATATIMEOUT);

   // Select the drive - call the sub_select function.
   // Quit now if this fails.

   if ( IdeDeviceSelection( Dev ) )
   {
      return 1;
   }

   // Set up all the registers except the command register.

   OutPortAta( CC_DEVC, DevCtrl );
   OutPortAta( CC_FEAT, Feat );
   OutPortAta( CC_SECC, SecC );
   OutPortAta( CC_SECN, Sect );
   OutPortAta( CC_CYLL, CylLow );
   OutPortAta( CC_CYLH, CylHigh );
   OutPortAta( CC_DEVH, DevHead );

   // For interrupt mode,
   // Take over INT 7x and initialize interrupt controller
   // and reset interrupt flag.

   //int_save_int_vect();

   // Start the command by setting the Command register.  The drive
   // should immediately set BUSY status.

   OutPortAta( CC_CMD, Cmd );

   // Waste some time by reading the alternate status a few times.
   // This gives the drive time to set BUSY in the status register on
   // really fast systems.  If we don't do this, a slow drive on a fast
   // system may not set BUSY fast enough and we would think it had
   // completed the command when it really had not even started the
   // command yet.

   DELAY400NS;

   // enable/start the dma channel.

   //trc_llt( 0, 0, TRC_LLT_DMA1 );
   EnableChanPci();

   // Data transfer...
   // the device and dma channel transfer the data here while we start
   // checking for command completion...

   // End of command...
   // if no error,
   // wait for drive to signal command completion
   // -or-
   // time out if this takes to long.

   Err=WaitForInt();


   // End of command...
   // now wait for the PCI BM DMA channel to flush the data.

   while ( 1 )
   {
	  
      if ( ( inportb( BusMaster.BaseAddress + BM_STATUS_REG )
             & BM_SR_MASK_INT )
         )                                // done yet ?
         break;                           // yes
      if ( TimerElapsed() )            // time out yet ?
      {
         
         break;
      }
   }

   #if 0
   trc_llt( 0,
            inportb( busMasterRegs + BM_STATUS_REG ),
            TRC_LLT_DEBUG );     // for debugging
   #endif

   // End of command...
   // disable/stop the dma channel

   // trc_llt( 0, 0, TRC_LLT_DMA3 );
   DisableChanPci();

   // End of command...
   // Read the primary status register.  In keeping with the rules
   // stated above the primary status register is read only
   // ONCE.

   Status = InPortAta( CC_STAT );

   // Final status check...
   // if no error, check final status...
   // Error if BUSY, DEVICE FAULT, DRQ or ERROR status now.

    if ( reg_cmd_info.ec == 0 )
   {
      if ( Status & ( CB_STAT_BSY | CB_STAT_DF | CB_STAT_DRQ | CB_STAT_ERR ) )
      {
         reg_cmd_info.ec = 74;
         trc_llt( 0, reg_cmd_info.ec, TRC_LLT_ERROR );
      }
   }

   // Final status check...
   // if no error, check make sure the device asserted the INTRQ signal.

   if ( ( reg_cmd_info.ec == 0 )
        &&
        ( ( inportb( busMasterRegs + BM_STATUS_REG ) & BM_SR_MASK_INT ) == 0 )
      )
   {
      reg_cmd_info.ec = 71;
      trc_llt( 0, reg_cmd_info.ec, TRC_LLT_ERROR );
   }

   // Final status check...
   // if any error, update total bytes transferred.

   if ( reg_cmd_info.ec == 0 )
      reg_cmd_info.totalBytesXfer = ns * 512L;
   else
      reg_cmd_info.totalBytesXfer = 0L;

   // Done...
   // Read the output registers and trace the command.

   //sub_trace_command();

   // Done...
   // For interrupt mode, restore the INT 7x vector.

   //int_restore_int_vect();

   // Done...
   // mark end of a R/W DMA command in low level trace

   //trc_llt( 0, 0, TRC_LLT_E_RWD );

   // All done.  The return values of this function are described in
   // ATAIO.H.

   if ( reg_cmd_info.ec )
      return 1;
   return 0;
}
*/

/*
// UNDER CONSTRUCTION
void SetUpXfer( int dir, long count, unsigned seg, unsigned off )

{

   int numPrd;             // number of PRD required
   uint64 addr;     // physical memory address
   uint64 addr1;    // physical address for 1st prd area
   uint64 addr2;    // physical address for 2nd prd area
   uint64 addr3;    // physical address for 3rd prd area
   int64 count1;            // byte count for 1st prd area
   int64 count2;            // byte count for 2nd prd area
   int64 count3;            // byte count for 3nd prd area
   int64 temp;
   uint64 * lwp;

   // disable/stop the dma channel, clear interrupt and error bits

   //trc_llt( 0, 0, TRC_LLT_DMA3 );
   DisableChanPci();
   ClearChanPci();

   // convert transfer address from seg:off to an absolute memory address

   addr = (unsigned long) seg;
   addr = addr << 4;
   addr = addr + (unsigned long) off;

   // the address for each prd is easy to compute.
   // note: the address must be an even number but that
   //       is not checked here.

   addr1 = addr;
   addr2 = ( addr1 & 0xffff0000L ) + 0x00010000L;
   addr3 = ( addr2 & 0xffff0000L ) + 0x00010000L;

   // set up the 1st, 2nd and 3rd prd's as required.
   // note: count must be an even number but that is
   //       not checked here!

   numPrd = 1;                      // assume only 1 prd
   count1 = count;
   count2 = 0L;
   count3 = 0L;
   temp = addr2 - addr1;            // size of 1st prd area
   if ( count > temp )              // does it fit?
   {
      count = count - temp;         // no...
      numPrd = 2;                   // need 2 prd
      count1 = temp;
      count2 = count;
      temp = addr3 - addr2;         // size of 2nd prd area
      if ( count > temp )           // does it fit?
      {
         count = count - temp;      // no...
         numPrd = 3;                // need 3 prd
         count2 = temp;
         count3 = count;
      }
   }

   // set the end bit in the prd list

   if ( numPrd == 3 )
      count3 = count3 | 0x80000000L;
   else
   if ( numPrd == 2 )
      count2 = count2 | 0x80000000L;
   else
      count1 = count1 | 0x80000000L;

   // build the prd list in the prd buffer

   lwp = prdBufPtr;
   * lwp = addr1;
   lwp ++ ;
   * lwp = count1 & 0x8000ffffL;
   lwp ++ ;
   * lwp = addr2;
   lwp ++ ;
   * lwp = count2 & 0x8000ffffL;
   lwp ++ ;
   * lwp = addr3;
   lwp ++ ;
   * lwp = count3 & 0x8000ffffL;

   // set the prd list address

   OutPortPci( BM_PRD_ADDR_LOW, prdBufPtrLow16 );
   OutPortPci( BM_PRD_ADDR_HIGH, prdBufPtrHigh16 );

   // set the read/write control:
   // PCI reads for ATA Write DMA commands,
   // PCI writes for ATA Read DMA commands.

   if ( dir )
      BusMaster.RWControl = BM_CR_MASK_READ;     // ATA Write DMA
   else
      BusMaster.RWControl = BM_CR_MASK_WRITE;    // ATA Read DMA
   OutPortPci( BM_COMMAND_REG, BusMaster.RWControl );
}
*/



