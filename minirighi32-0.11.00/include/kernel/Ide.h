/*!	\file include/kernel/Ide.h
 *	\brief IDE driver header.
 *	\author Luca Giovacchini
 *	\date Last update: 2003-11-07
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

#ifndef IDE_H
#define IDE_H

#include <const.h>

#define IDE_DEBUG      1 // 1=compila codice di debug  0=non compila codice di debug


//**********************************************
// LOGICAL REGISTERS, INDEX, TYPE
// used with CurrentChannel
//**********************************************
#define CC_PRIMARY         0     
#define CC_SECONDARY       1     

// Index of IdeChannel_Struct.IOPort (see above)
#define CC_DATA      0     // Data Register
#define CC_ERR       1     // Error Register
#define CC_FEAT      1     // Features Register
#define CC_SECC      2     // Sector Count Register
#define CC_SECN      3     // Sector Number Register
#define CC_CYLL      4     // Cylinder Low Register
#define CC_CYLH      5     // Cylinder High Register
#define CC_DEVH      6     // Device/Head Register
#define CC_STAT      7     // Status Register
#define CC_CMD       7     // Command Register
#define CC_ASTAT     8     // Alternate Status Register
#define CC_DEVC      8     // Device Command Register
#define CC_ADDR      9     // Address Register

#define CC_DEV0          0
#define CC_DEV1          1
#define CC_DEVTYPE_NONE  0
#define CC_DEVTYPE_UNKN  1
#define CC_DEVTYPE_ATA   2
#define CC_DEVTYPE_ATAPI 3

//**********************************************
// SUB AND STRUCTURE
//**********************************************

#define GETBITB(Data,nBit) ( ((byte) Data >> nBit) & 0x01 )
#define GETBITW(Data,nBit) ( ((word) Data >> nBit) & 0x0001 )
#define GETBITD(Data,nBit) ( ((dword) Data >> nBit) & 0x00000001 )
#define GETNIBBLEB(Data,nNibble) ( ((byte) Data >> (nNibble<<2)) & 0x0F)
#define GETNIBBLEW(Data,nNibble) ( ((word) Data >> (nNibble<<2)) & 0x000F)
#define GETNIBBLED(Data,nNibble) ( ((dword) Data >> (nNibble<<2)) & 0x0000000F)
#define GETBYTEW(Data,nByte) ( ((word) Data >> (nByte<<3)) & 0x00FF )
#define GETBYTED(Data,nByte) ( ((dword) Data >> (nByte<<3)) & 0x000000FF)
#define GETWORDD(Data,nWord) ( ((dword) Data >> (nWord<<4)) & 0x0000FFFF )
#define MAKEDWORDW(Word1,Word0) ( (((dword) Word1 << 16) | (word) Word0) )
#define SWAPBYTEW(Data) ( (GETBYTEW(Data,0) << 8) | (GETBYTEW(Data,1)) )

// the "NO " with the space is to let they be 3 char len
#define Bit2YesNo(Exp) ( (char *) (Exp ? "YES" : "NO ") )


typedef struct BusMaster_Struct
{
	int Config;
	dword BaseAddress;
	unsigned long * PrdBufPtr;  // PRD address (seg:off, off is 0)
	dword DmaPciPrdAddr;
	byte PrdBuf[64];	// Physical region descriptor table (for Dma 32)
	dword PrdBufPtrLow;	// 32-bit prd ptr upper 16 bits
	dword PrdBufPtrHigh;	// 32-bit prd ptr lower 16 bits
	byte StatReg;          // save BM status reg bits
	byte RWControl;        // read/write control bit setting
} BusMaster_Struct;


void ShowDeviceSubData(int Dev);
void ShowIdeSubData();
int ScanDevice(int UseInterrupt) ;
int IdentifyDevice (int Dev) ;
void ShowDeviceDataRowName(char * Name,int Col);
void ShowDeviceDataRowValue(char * Name, int Col, char * Format, dword Value);
void ShowDeviceDataRowString(char * Name, int Col, char * Value);
void ShowDeviceDataRowSup(char * Name,int Col, byte Supported);
void ShowDeviceDataRowSupEn(char * Name,int Col,byte Supported,byte Enabled);
void ShowDeviceDataRowBitNum(word BitCode,byte StartFrom, byte EndTo, byte StartPrintNum);
void ShowDeviceData(int Dev,int Col);
void InitIde();
int ReadSectorsLba(int Dev, uint64 Lba, int NumSect, word * Buffer,int UseInterrupt);
int WriteSectorsLba(int Dev, uint64 Lba, int NumSect, word * Buffer,int UseInterrupt);


/*
byte InPortPci(word Port);
void SetUpXfer( int dir, long count, unsigned seg, unsigned off );
int DmaPciAta( int Dev, int Cmd,
                 int Feat, int SecC,
                 dword Cyl, int Head, int Sect,
                 unsigned Seg, unsigned Off );
int DmaPciAtaLba( int Dev, int Cmd,
                            int Feat, int SecC,
                            uint64 Lba,
                            unsigned Seg, unsigned Off );
void EnableChanPci() ;
void DisableChanPci();
void OutPortPci(word Port, byte Val);
*/
//**********************************************
// BUS MASTER REGISTER BASE IO PORT
//**********************************************
#define BMCR_IO_ADDR 0xFF00   // YOU MUST SUPPLY THIS VALUE
#define BM_COMMAND_REG     0              // offset to command reg
#define BM_CR_MASK_READ    0x00           // read from memory
#define BM_CR_MASK_WRITE   0x08           // write to memory
#define BM_CR_MASK_START   0x01           // start transfer
#define BM_CR_MASK_STOP    0x00           // stop transfer

#define BM_STATUS_REG     2            // offset to status reg
#define BM_SR_MASK_SIMPLEX 0x80           // simplex only
#define BM_SR_MASK_DRV1    0x40           // drive 1 can do dma
#define BM_SR_MASK_DRV0    0x20           // drive 0 can do dma
#define BM_SR_MASK_INT     0x04           // INTRQ signal asserted
#define BM_SR_MASK_ERR     0x02           // error
#define BM_SR_MASK_ACT     0x01           // active

#define BM_PRD_ADDR_LOW   4            // offset to prd addr reg low 16 bits
#define BM_PRD_ADDR_HIGH  6            // offset to prd addr reg high 16 bits


#endif
