/*!	\file include/kernel/speaker.h
 *	\brief Header for the speaker.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-02
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef SPEAKER_H
#define SPEAKER_H

#include <const.h>

/** \ingroup Kernel
 *  \defgroup KSpeaker System Speaker
 *  The system speaker driver.
 *  @{
 */

//! Frequency of the system beep.
#define SYSTEM_BEEP_FREQ	0x533
//! The system beep duration (in ms).
#define SYSTEM_BEEP_TIME	20

void sound(uint32_t frequency);
void nosound();
void beep();

/** @} */ // end of KSpeaker

#endif
