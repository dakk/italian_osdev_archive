/*!	\file include/kernel/IdeTimer.h
 *	\brief IDE driver::Timing header.
 *	\author Luca Giovacchini
 *	\date Last update: 2003-11-07
 *	\note Copyright (&copy;) 2003 Luca Giovacchini
 *
 *	This driver is based on Atadrv by Hale Landis
 *	but it is completely rearranged for the minirighi32.
 *	\n
 *	<b>IMPORTANT!!!</b>\n
 *	Here you can find all subs regarding waiting and timing.
 *	Look at the specific sub comment.
 */

#ifndef IDETIMER_H
#define IDETIMER_H

#include <const.h>


// This macro provides a small delay that is used in several
// places in the ATA command protocols:
// 1) It is recommended that the host delay 400ns after
//    writing the command register.
// 2) ATA-4 has added a new requirement that the host delay
//    400ns if the DEV bit in the Device/Head register is
//    changed.  This was not recommended or required in ATA-1,
//    ATA-2 or ATA-3.  This is the easy way to do that since it
//    works in all PIO modes.
// 3) ATA-4 has added another new requirement that the host delay
//    after the last word of a data transfer before checking the
//    status register.  This was not recommended or required in
//    ATA-1, ATA-2 or ATA-3.  This is the easy to do that since it
//    works in all PIO modes.

// Waste some time by reading the alternate status a few times.
// This gives the drive time to set BUSY in the status register on
// really fast systems.  If we don't do this, a slow drive on a fast
// system may not set BUSY fast enough and we would think it had
// completed the command when it really had not even started the
// command yet.
#define DELAY400NS  { InPortAta( CC_ASTAT ); InPortAta( CC_ASTAT );  \
                      InPortAta( CC_ASTAT ); InPortAta( CC_ASTAT ); }

#define HDC_ATATIMEOUT   31000
#define HDC_ATAPIDELAY     150

#define MSG_PRESSKEY "Press any key to continue"

// This struct implement a Timer
// Starting a timer means filling this struct with timer data
// and next we can check when a timer is elapsed
// Timer subs and this struct permit to create a lot of timer
// concurrently
typedef struct Timer_Struct
{
	dword Start;
	dword Stop;
	int Cross;
} Timer_Struct;


void GoIdle();
void Delay(dword ms);
Timer_Struct TimerStart(dword ms);
int TimerElapsed(Timer_Struct Timer);
int WaitForInt(Timer_Struct Timer);
int WaitForBsy(Timer_Struct Timer,word Port);
void AtapiDelay(int Dev);
void WaitKeyPress(char * Message);

#endif
