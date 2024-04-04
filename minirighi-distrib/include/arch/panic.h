/*!	\file include/arch/panic.h
 *	\brief Panic functions.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-21
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#ifndef PANIC_H
#define PANIC_H

#include <const.h>

// --- Prototypes ----------------------------------------------------- //

void error(const byte *msg) __NORETURN__;
void reboot() __NORETURN__;
void poweroff() __NORETURN__;

#endif
