/*!	\file drivers/speaker/speaker.c
 *	\brief System speaker driver.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-02
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <const.h>
#include <arch/clock.h>
#include <arch/i386.h>
#include <kernel/speaker.h>

//! \brief Start a sound using the speaker.
//! \param frequency The frequency of the sound.
void sound(uint32_t frequency)
{
	dword x;
	uint32_t flags;

	if ((frequency<19) || (frequency>22000)) return;

	x = TIMER_FREQ / frequency;

	local_irq_save( flags );

	outportb(0x61, inportb(0x61) | 3);
	outportb(0x43, 0xB6);
	outportb(0x42, x & 0xFF);
	outportb(0x42, x >> 8);

	local_irq_restore( flags );
}

//! \brief Turn off the speaker.
void nosound()
{
	uint32_t flags;

	local_irq_save( flags );
	outportb(0x61, inportb(0x61) & 0xFC);
	local_irq_restore( flags );
}

//! \brief Play a system beep.
void beep()
{
	sound(SYSTEM_BEEP_FREQ);
	delay(SYSTEM_BEEP_TIME);
	nosound();
}
