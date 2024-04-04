/*!	\file drivers/ata/IdeLow.c
 *	\brief IDE driver::Low level routines.
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

#include <string.h>

#include <arch/clock.h>
#include <arch/i386.h>
#include <arch/interrupt.h>
#include <arch/mem.h>

#include <kernel/Ide.h>
#include <kernel/IdeAta.h>
#include <kernel/IdeDebug.h>
#include <kernel/IdeTimer.h>
#include <kernel/keyboard.h>
#include <kernel/video.h>

#include <kernel/IdeLow.h>

// store current selected channel (all command refer to this)
// volatile IdeChannel_Struct IdeChannel[2];
volatile IdeChannel_Struct * CurrentChannel;

// absolute port of the register of the primary controller channel
static IdeChannel_Struct IdePriReg;
// absolute port of the register of the secondary controller channel
static IdeChannel_Struct IdeSecReg;

// ****************** SetAtaRegisterIoPort ******************
// Set a unique logical vector with all the controller register used.
// This is used as our logical view of the register without worring about 
// underlying combination.
// **********************************************************
void SetAtaRegisterIoPort(word CmdBase, word CntBase, int IntN) 
{
	CurrentChannel->CmdBasePort=CmdBase;
	CurrentChannel->CntBasePort=CntBase;

	CurrentChannel->IoPort[ CC_DATA  ] = CmdBase + HDC_DATA;
	CurrentChannel->IoPort[ CC_ERR   ] = CmdBase + HDC_ERR;
	CurrentChannel->IoPort[ CC_SECC  ] = CmdBase + HDC_SECC;
	CurrentChannel->IoPort[ CC_SECN  ] = CmdBase + HDC_SECN;
	CurrentChannel->IoPort[ CC_CYLL  ] = CmdBase + HDC_CYLL;
	CurrentChannel->IoPort[ CC_CYLH  ] = CmdBase + HDC_CYLH;
	CurrentChannel->IoPort[ CC_DEVH  ] = CmdBase + HDC_DEVH;
	CurrentChannel->IoPort[ CC_STAT  ] = CmdBase + HDC_STATUS;
	CurrentChannel->IoPort[ CC_ASTAT ] = CntBase + HDC_ASTAT;
	CurrentChannel->IoPort[ CC_ADDR  ] = CntBase + HDC_ADD;
	CurrentChannel->Device[ CC_DEV0 ].RegBit = HDC_DEVH_DEV0;
	CurrentChannel->Device[ CC_DEV0 ].Type   = CC_DEVTYPE_NONE;
	CurrentChannel->Device[ CC_DEV1 ].RegBit = HDC_DEVH_DEV1;
	CurrentChannel->Device[ CC_DEV1 ].Type   = CC_DEVTYPE_NONE;

	CurrentChannel->IntDone = FALSE;
	CurrentChannel->IntNum = IntN;
}

// ******************** SelectAtaChannel ********************
// Instead of passing always what atachannel to use we set it here
// Other functions will use the ata channel previously set
// **********************************************************
void SelectAtaChannel (int Channel)
{
	// let primary be the default channel	
	if ( Channel == CC_SECONDARY )
	{
		CurrentChannel=&IdeSecReg;
	}
	else
	{
		CurrentChannel=&IdePriReg;	
	}
}

// ************************ AtaPort *************************
// Translate Register Port in Absolute Port
// **********************************************************
word AtaPort(word Port)
{
	return CurrentChannel->IoPort[Port];
}

// *********************** OutPortAta ***********************
// Call outport with the translated (logical-physiscal) port
// **********************************************************
void OutPortAta(word Port, byte Val) 
{
	outportb(AtaPort(Port),Val);
}

// *********************** InPortAta ************************
// Call inport with the translated (logical-physiscal) port
// **********************************************************
byte InPortAta(word Port) 
{
	return inportb(AtaPort(Port));
}

// ********************** InPortAtaMul **********************
// Call inportwmul with the translated (logical-physiscal) port
// **********************************************************
void InPortAtaMul(word Port, word * Buffer,word Count)
{
	// i suppose that user had set Buffer bigger enough!
	inportwm(AtaPort(Port),Buffer,Count);
}

// ********************** OutPortAtaMul **********************
// Call outportwmul with the translated (logical-physiscal) port
// **********************************************************
void OutPortAtaMul(word Port, word * Buffer, word Count)
{
	outportwm(AtaPort(Port),Buffer,Count);
}


// *********************** SetDevBit ************************
// Set the proper device bit in the DeviceHead register
// **********************************************************
void SetDevBit(int Dev) 
{
	OutPortAta( CC_DEVH, CurrentChannel->Device[Dev].RegBit);
	DELAY400NS;
}

// ********************* SetFirstDevBit *********************
// If possible, select a device that exists,
// try device 0 first.
// **********************************************************
void SetFirstDevBit() 
{	
	if ( CurrentChannel->Device[CC_DEV0].Type != CC_DEVTYPE_NONE )
		SetDevBit(CC_DEV0);		
	else if( CurrentChannel->Device[CC_DEV1].Type != CC_DEVTYPE_NONE ) 
		SetDevBit(CC_DEV1);		
}

// ********************** Ide_Handler ***********************
// This sub must be called only from maing OS interrupt handler
// when interrupts come so we can know it.
// **********************************************************

// --- modified by: Andrea Righi 2003-09-16 ----------------------------//
void Ide_Handler_Pri()
{
	IdePriReg.IntDone = TRUE;
}

void Ide_Handler_Sec()
{
	IdeSecReg.IntDone = TRUE;
}
// ---------------------------------------------------------------------//
