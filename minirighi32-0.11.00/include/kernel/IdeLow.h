/*!	\file include/kernel/IdeLow.h
 *	\brief IDE driver::Low level routines header.
 *	\author Luca Giovacchini
 *	\date Last update: 2003-11-07
 *	\note Copyright (&copy;) 2003 Luca Giovacchini
 *
 *	<b>IMPORTANT!!!</b>\n
 *	Here you can find the base sub for communicating with
 *	Ide Devices.
 *	First subs expose a simpler interface to the
 *	rest of the driver, creating a simpler logical view of
 *	registers and most important data.
 *	You must not issue commands to device only with these subs,
 *	but you must use IdeAta subs (Ata protocol).
 */

#ifndef IDELOW_H
#define IDELOW_H

// primary host adapter at I/O address 1FX/3FX 
// secondary host adapter at I/O adress 17X/37X
// Each host adapter can have up to two hard drives MASTER/SLAVED off it.
// 1FX  0001 1111 XXXX  Primary Command Registers
// 3FX  0011 1111 XXXX  Primary Control Registers

// 17X  0001 0111 XXXX  Alternate Command Registers
// 37X  0011 0111 XXXX  Alternate Control Registers

// I/O Ports //
// 1FX and 17X use same command so we'll use they as a base port + command offset

// For a very complete documentation of these ports and all commands you
// must read T13 Documentation for ATA 1-2-3-4 etc. etc.,(for example you
// can read (www.T13.org) d1153r18.pdf (330pag. all on ATA-4)

//**********************************************
// COMMAND REGISTER BASE IO PORT
//**********************************************
// Base port for controller registers
#define HDC_BASEPRI        0x1F0 //Base port for primary channel register 
#define HDC_BASESEC        0x170 //Base port for secondary channel register
#define HDC_CONTROLGAP     0x200 //Bas+Gap=Control register address (3F0 and 370)
// IMPORTANT: HDC_ADDRESS PORT IN THE PRIMARY CHANNEL = 0x3F0+7=0x3F7 IS SHARED WITH FLOPPY (PAY ATTENTION)
// Interrupt number
#define HDC_INTPRI         14    //Int num for primary channel's device
#define HDC_INTSEC         15    //Int num for secondary channel's device

//**********************************************
// REGISTERS OFFSET PORT
//**********************************************

// COMMAND REGISTER OFFSET PORT
#define HDC_DATA       0     // r/w  Data Register 
#define HDC_ERR        1     // r    Error Register   w   Feaure Register
#define HDC_SECC       2     // r/w  Sector Count,  how many sectors to read/write
#define HDC_SECN       3     // r/w  Sector number, the actual sector wanted
#define HDC_CYLL       4     // r/w  Cylinder low, cylinders is 0-1024
#define HDC_CYLH       5     // r/w  Cylinder high, this makes up the rest of the 1024
#define HDC_DEVH       6     // r/w  Device/Head 
#define HDC_STATUS     7     // r    Primary Status Register  w   Command Register
// CONTROL REGISTER OFFSET PORT
// alternate is like status but reading status register clear pending interrupts while 
// reading alternate do not clear pending interrupt
#define HDC_ASTAT      6     // r    Alternate Status Register     w  Device control register
// i haven't found information about next register, the only thing is:
// returns the drive select and head select addresses for the drive currently selected
#define HDC_ADD        7     // Device Address 

//**********************************************
// DEVICE HEAD REGISTER BIT
//**********************************************
// Bit 7 and 5 = 1 This is needed only for ATA 1 standard
#define HDC_DEVH_OBSOLETE 0xa0 
// Bit 4 is for selecting the device
#define HDC_DEVH_DEV0     HDC_DEVH_OBSOLETE | 0x00  // Device Head 10100000
#define HDC_DEVH_DEV1     HDC_DEVH_OBSOLETE | 0x10  // Device Head 10110000
#define HDC_DEVH_LBA      0x40                     // Lba Mode    01000000

//**********************************************
// DEVICE CONTROL REGISTER BIT  (WRITE) 
//**********************************************
#define HDC_DEVC_HD15   0x08  // Bit 4 = 1 This is needed only for older ATA standard
#define HDC_DEVC_SRST   0x04  // Bit 2 : Soft Reset
#define HDC_DEVC_NIEN   0x02  // Bit 1 : Enable=1 Disable=0 interrupts


//**********************************************
// STATUS AND ALTERNATE REGISTER SIGNAL (READ)
//**********************************************
// Primary And Alternate Register Bit and Command
#define HDC_STAT_BSY     0x80  // busy
#define HDC_STAT_RDY     0x40  // ready
#define HDC_STAT_DF      0x20  // device fault
#define HDC_STAT_WFT     0x20  // write fault (old name)
#define HDC_STAT_SKC     0x10  // seek complete
#define HDC_STAT_SERV    0x10  // service
#define HDC_STAT_DRQ     0x08  // data request
#define HDC_STAT_CORR    0x04  // corrected
#define HDC_STAT_IDX     0x02  // index
#define HDC_STAT_ERR     0x01  // error (ATA)
#define HDC_STAT_CHK     0x01  // check (ATAPI)

//**********************************************
// COMMAND CODE
// Most mandtory and optional ATA commands (from ATA-3),
//**********************************************
#define HDC_CMD_CFA_ERASE_SECTORS            0xC0
#define HDC_CMD_CFA_REQUEST_EXT_ERR_CODE     0x03
#define HDC_CMD_CFA_TRANSLATE_SECTOR         0x87
#define HDC_CMD_CFA_WRITE_MULTIPLE_WO_ERASE  0xCD
#define HDC_CMD_CFA_WRITE_SECTORS_WO_ERASE   0x38
#define HDC_CMD_CHECK_POWER_MODE1            0xE5
#define HDC_CMD_CHECK_POWER_MODE2            0x98
#define HDC_CMD_DEVICE_RESET                 0x08
#define HDC_CMD_EXECUTE_DEVICE_DIAGNOSTIC    0x90
#define HDC_CMD_FLUSH_CACHE                  0xE7
#define HDC_CMD_FORMAT_TRACK                 0x50
#define HDC_CMD_IDENTIFY_DEVICE              0xEC
#define HDC_CMD_IDENTIFY_PACKET_DEVICE       0xA1
#define HDC_CMD_IDLE1                        0xE3
#define HDC_CMD_IDLE2                        0x97
#define HDC_CMD_IDLE_IMMEDIATE1              0xE1
#define HDC_CMD_IDLE_IMMEDIATE2              0x95
#define HDC_CMD_INITIALIZE_DRIVE_PARAMETERS  0x91
#define HDC_CMD_INITIALIZE_DEVICE_PARAMETERS 0x91
#define HDC_CMD_NOP                          0x00
#define HDC_CMD_PACKET                       0xA0
#define HDC_CMD_READ_BUFFER                  0xE4
#define HDC_CMD_READ_DMA                     0xC8
#define HDC_CMD_READ_DMA_QUEUED              0xC7
#define HDC_CMD_READ_MULTIPLE                0xC4
#define HDC_CMD_READ_SECTORS                 0x20
#define HDC_CMD_READ_VERIFY_SECTORS          0x40
#define HDC_CMD_RECALIBRATE                  0x10
#define HDC_CMD_SEEK                         0x70
#define HDC_CMD_SET_FEATURES                 0xEF
#define HDC_CMD_SET_MULTIPLE_MODE            0xC6
#define HDC_CMD_SLEEP1                       0xE6
#define HDC_CMD_SLEEP2                       0x99
#define HDC_CMD_STANDBY1                     0xE2
#define HDC_CMD_STANDBY2                     0x96
#define HDC_CMD_STANDBY_IMMEDIATE1           0xE0
#define HDC_CMD_STANDBY_IMMEDIATE2           0x94
#define HDC_CMD_WRITE_BUFFER                 0xE8
#define HDC_CMD_WRITE_DMA                    0xCA
#define HDC_CMD_WRITE_DMA_QUEUED             0xCC
#define HDC_CMD_WRITE_MULTIPLE               0xC5
#define HDC_CMD_WRITE_SECTORS                0x30
#define HDC_CMD_WRITE_VERIFY                 0x3C


// We use it for store data returned by Identify Device command
typedef struct DeviceInfo_Struct
{
	byte Ata;						// Word  0 bit 15 bit coded bit 0 = ata 1 bit 1 = ata 2 ...
	byte Removable;					// Word  0 bit 7 bool
	byte NotRemovable;				// Word  0 bit 6 bool
	word LogicalCylinders;			// Word  1
	word LogicalHeads;				// Word  3
	word LogicalSectorsPerTrack;	// Word  6
	char SerialNumber[21];			// Word 10-19 ascii
	char FirmwareRev[9];			// Word 23-26 ascii
	char ModelNumber[41];			// Word 27-46 ascii
	byte MaxMulitSectPerBlock;		// Word 47 bit 7-0
	byte StandByTimerSupport;		// Word 49 bit 13
	byte IORDYSupport;				// Word 49 bit 11
	byte IORDYDisabled;             // Word 49 bit 10
	byte PioMode;					// Word 51 bit 15-8  // only 0 1 2 value
	byte ValidUDMAMode;				// Word 53 bit 2 bool if Udma info are valid
	byte ValidCycleTime;			// Word 53 bit 1
	byte ValidCurLogicalValue;		// Word 53 bit 0

	word CurLogicalCylinders;		// Word 54
	word CurLogicalHeads;			// Word 55
	word CurLogicalSectorsPerTrack; // Word 56
	dword CapacityInSectors;		// Word 57-58
	byte ValidCurMultiSectPerInt;   // Word 59 bit 8
	byte CurMultiSectPerInt;		// Word 59 bit 7-0
	dword CapacityLba;				// Word 60-61
	// Word 63
	// Bit 10 9 8 and bit 2 1 0
	byte MultiDMASelected;			// Word 63 bit coded bit 0 = udma 0 bit 1 = udma 1 ...
	byte MultiDMASupported;			// Word 63 bit coded bit 0 = udma 0 bit 1 = udma 1 ...

	byte AdvPioModeSupported;       // Word 64 bit 7-0	// bit coded bit 0 = pio 3 bit 1 = pio 4 ....
	word MinCycleTime;				// Word 65
	word RecCycleTime;				// Word 66
	word MinPioCycleTime;			// Word 67
	word MinPioCyleTimeFlow;		// Word 68
	byte QueueDepth;                // Word 75 bit:0-4
	// Word 80
	word AtaSupported;	// bit coded, bit 0 = reserved, bit 1 = ata1, bit 2 = ata2 ....

	byte HighestAtaSupported;  // This is not in DeviceInfo data but is calculated
	
	word MinorVersion;            // Word 81 only if != FFFF & != 0000
	// Word 82  Should not be 0000 or FFFF
	byte SFSNopSupported;				// 14
	byte SFSReadBufferSupported;		// 13
	byte SFSWriteBufferSupported;		// 12
	byte SFSProtectedAreaSupported;		// 10
	byte SFSDeviceResetSupported;		//  9
	byte SFSServiceIntSupported;		//  8
	byte SFSReleaseIntSupported;		//  7
	byte SFSLookAheadSupported;			//  6
	byte SFSWriteCacheSupported;		//  5
	byte SFSPacketSupported;			//  4
	byte SFSPowerManagSupported;		//  3
	byte SFSRemovableMediaSupported;	//  2
	byte SFSSecurityModeSupported;		//  1
	byte SFSSmartSupported;				//  0
	// Word 83
	byte SFSRemMediaNotifSupported;		//  4
	byte SFSAdvPowerManagSupported;		//  3
	byte SFSCFASupported;				//  2
	byte SFSRWDmaQueuedSupported;		//  1
	byte SFSDownMicrocodeSupported;		//  0
	// Word 85
	byte SFEServiceIntEnalbed;			//  8
	byte SFEReleaseIntEnabled;			//  7
	byte SFELookAheadEnabled;			//  6
	byte SFEWriteCacheEnabled;			//  5
	byte SFESecurityModeEnabled;		//  1
	byte SFESmartEnabled;				//  0
	// Word 86
	byte SFERemMediaNotifEnabled;		//  4
	byte SFEAdvPowerManagEnabled;		//  3
	// Word 88
	// bit 13 12 11 10 9 8 and 5 4 3 2 1 0
	byte UDMASelected;  // bit 13 -8
	byte UDMASupported; // bit 5 - 0

	byte HighestUDMASupported;  // This is not in DeviceInfo data but is calculated

	word SecurityEraseTime;		// Word 89
	word SecurityEnEraseTime;	// Word 90
	word CurAPMValue;			// Word 91
	// Word 128
	byte SecurityLevel;			// bit 8
	byte SecurityEnErase;		// bit 5
	byte SecurityCountExpired;  // bit 4
	byte SecurityFrozen;        // bit 3
	byte SecurityLocked;        // bit 2
	byte SecurityEnabled;		// bit 1
	byte SecuritySupported;     // bit 0;

	word VendorSpecific[31];	// Word 129-159


} DeviceInfo_Struct;

// Store global information about devices on the system
typedef struct Device_Struct
{
	byte RegBit;			// Dev/Head register identifier for this device
	DeviceInfo_Struct Info; // A lot of info for this devices (filled by identifydevice command)
	int Type;				// Type of device
} Device_Struct;

// Store global information about IdeChannels
typedef struct IdeChannel_Struct
{
	int IntDone;			// interrupt flag for this channel
	word CmdBasePort;		// base address of commands registers
	word CntBasePort;		// base address of controls registers
	word IoPort[10];		// absolute address for all ide register
	Device_Struct Device[2];// two device for each channel
	int IntNum;				// int number associeted with this channel
} IdeChannel_Struct;



void SetAtaRegisterIoPort(word CmdBase, word CntBase, int IntN) ;
void SelectAtaChannel (int Channel);
word AtaPort(word Port);
void OutPortAta(word Port, byte Val);
byte InPortAta(word Port);
void InPortAtaMul(word Port, word * Buffer,word Count);
void OutPortAtaMul(word Port, word * Buffer, word Count);
void SetDevBit(int Dev);
void SetFirstDevBit();
void Ide_Handler(word Irq);



#endif
