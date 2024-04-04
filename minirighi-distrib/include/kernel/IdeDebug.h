/*!	\file include/kernel/IdeDebug.h
 *	\brief IDE driver::Debug header.
 *	\author Luca Giovacchini
 *	\date Last update: 2003-11-07
 *	\note Copyright (&copy;) 2003 Luca Giovacchini
 *
 *	This driver is based on Atadrv by Hale Landis
 *	but it is completely rearranged for the minirighi32.
 *	\n
 *	<b>IMPORTANT!!!</b>\n
 *	Here you can find code for debugging and showing errors.
 */


#ifndef IDETRACE_H
#define IDETRACE_H

#include <const.h>

#define DBG_SHOWMAT_ASCII 1
#define DBG_SHOWMAT_HEX   2

// Command names structure store string name for each command code
typedef struct CmdNames_Struct
{
   byte CmdCode;
   char * CmdName;
}  CmdNames_Struct;

// Error names structure store string error message for each error code
typedef struct ErrNames_Struct
{
   byte ErrCode;
   char * ErrName;
}  ErrNames_Struct;

char * GetIdeErrorMessage(int Err);
void ShowIdeErrorMessage(int Err,int Dump);
void ShowIdeRegisterDump();
void PrintByteHex(byte Data);
void ShowMatrixData(char * DataBuffer,int Col,int BytePerCol,int RowToWait,int Len, int Mode);



#endif
