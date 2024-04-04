/*!	\file drivers/ata/IdeTimer.c
 *	\brief IDE driver::Timing.
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

#include <arch/i386.h>  // for Enable() Disable()
#include <arch/clock.h>   // for Ticks

#include <kernel/console.h>   // for kprintf
#include <kernel/IdeTimer.h>
#include <kernel/IdeLow.h>
#include <kernel/Ide.h>
#include <kernel/keyboard.h>
#include <kernel/task.h>    // for Idle()

extern dword ticks;
extern IdeChannel_Struct * CurrentChannel;

// ************************* Idle ***************************
// **********************************************************
void GoIdle()
{
	uint32_t flags;

	local_irq_set( flags );
	safe_idle();
	local_irq_restore( flags );
}

// ************************ Delay ***************************
// Start a timer
// **********************************************************
void Delay(dword ms)
{

	dword Start=ticks;
	// we work with integer and not float, we have resolution greater than 1 ms
	// so we must trunc at the tick higher expected
	dword Stop;
	byte Cross;
	if ( (ms*HZ)%1000 == 0 )
		// We can wait exactely ms
		Stop=Start+ms*HZ/1000;
	else
		// We must wait little more than ms because our
		// timer resolution can't permit to wait exactely ms
		Stop=(Start+ms*HZ/1000)+1;

	Cross=(Stop<Start);
	// need to pay attention while waiting
	// for overflow of ticks and overflow of Stop
	while ( Cross ? (ticks >= Start) || (ticks < Stop) : (ticks>=Start) && (ticks<Stop) )
		GoIdle();

}

// ********************** TimerStart ************************
// Start a timer (filling a timer struct)
// **********************************************************
Timer_Struct TimerStart(dword ms) 
{	
	Timer_Struct Timer;
	Timer.Start=ticks;

	if ( ms*HZ % 1000 == 0 )
		// We can wait exactely ms
		Timer.Stop=Timer.Start+ms*HZ/1000;
	else
		// We must wait little more than ms because our
		// timer resolution can't permit to wait exactely ms
		Timer.Stop=(Timer.Start+ms*HZ/1000)+1;
	Timer.Cross=(Timer.Stop<Timer.Start);
	return Timer;
}

// ********************* TimerElapsed ***********************
// Check if timer is elapsed
// **********************************************************
int TimerElapsed(Timer_Struct Timer)
{
	// need to pay attention for overflow of ticks while waiting
	// and overflow of Stop
	return ( Timer.Cross ? (ticks < Timer.Start) && (ticks >= Timer.Stop) : (ticks<Timer.Start) || (ticks>=Timer.Stop) );
}


// ********************** WaitForInt ************************
// Wait for interrupt
// The Timer MUST be started previously or this code will
// wait an undefined period of time
// **********************************************************
int WaitForInt(Timer_Struct Timer)
{
	while (TRUE)
	{
		if (CurrentChannel->IntDone == TRUE)
		{
			// reset interrupt flag;
			CurrentChannel->IntDone=FALSE;
			return FALSE;
		}

		if ( TimerElapsed(Timer) )
			return TRUE;

		GoIdle();
	}
}

// ********************** WaitForBsy ************************
// Wait for BSY = 0
// The Timer MUST be started previously or this code will
// wait an undefined period of time
// **********************************************************
int WaitForBsy(Timer_Struct Timer,word Port)
{
	byte Status=0;
	
	while (TRUE)
	{
		Status = InPortAta( Port );       // poll for not busy
		if ( ( Status & HDC_STAT_BSY ) == 0 )
			return FALSE;
		
		if ( TimerElapsed(Timer) )               // time out yet ?
			return TRUE;
		
		GoIdle();
	}
}

// ********************** AtapiDelay ************************
// Execute a delay only for atapi device
// **********************************************************
void AtapiDelay(int Dev)
{
	if ( CurrentChannel->Device[Dev].Type == CC_DEVTYPE_ATAPI )
		delay(HDC_ATAPIDELAY);
}

// ********************** WaitKeyPress **********************
// Wait user press a key
// is for shell ide command and visualization
// **********************************************************
void WaitKeyPress(char * Message)
{
	int Tmp;
	if (Message)
		kprintf("\n\r%s\n\r",Message);
	Tmp=kgetchar();
}



