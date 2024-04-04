/*!	\file include/kernel/shell.h
 *	\brief MiniShell header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-07\n
 *	\note Copyright (C) 2003-2004 Andrea Righi
 */

#ifndef SHELL_H
#define SHELL_H

//! The name & version of the shell.
#define SH_VERSION	"MiniShell version 0.04.00"

//! The number of commands supported.
#define SH_COMMANDS	( sizeof(commands)/sizeof(CMD_t) )

//! A command format structure.
typedef struct CMD {
	byte index;
	char *name;
	char *usage;
	char *help;
} CMD_t;

// --- Prototypes ----------------------------------------------------- //

void shell( int argc, char **argv );

#endif

