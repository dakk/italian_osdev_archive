/*!	\file include/kernel/IdeAta.h
 *	\brief IDE driver::ATA Protocol header.
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

#ifndef IDEATA_H
#define IDEATA_H

#include <const.h>


int IdeDeviceSelection( int Dev );
void IdeDeviceDetection (int Dev);
void IdeDeviceTypeDetection (int Dev);
int IdeSoftReset( int SkipFlag, int UseInterrupt );
int IdeNonData(		int Dev, int Cmd,
					int Feat, int SecC,
					dword Cyl, int Head, 
					int Sec, int UseInterrupt );
int IdePioDataInLba(int Dev, int Cmd,
					int Feat, int SecC, uint64 Lba,
					word * Buffer,
					int NumSect, int MultiCnt, int UseInterrupt);
int IdePioDataIn(	int Dev, int Cmd,
					int Feat, int SecC,
                    dword Cyl, int Head, int Sect,
                    word * Buffer,
                    int NumSect, int MultiCnt, int UseInterrupt);
int IdePioDataOutLba(int Dev, int Cmd,
					int Feat, int SecC, uint64 Lba,
					word * Buffer,
					int NumSect, int MultiCnt, int UseInterrupt);
int IdePioDataOut(	int Dev, int Cmd,
					int Feat, int SecC,
                    dword Cyl, int Head, int Sect,
                    word * Buffer,
                    int NumSect, int MultiCnt, int UseInterrupt);


#endif

