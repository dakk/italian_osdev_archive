/*!	\file include/kernel/debug.h
 *	\brief Debug header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-04
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#ifndef DEBUG_H
#define DEBUG_H

//! The log buffer length (must be a power of 2).
#define LOG_BUF_LEN		8192

//! The log buffer mask
#define LOG_BUF_MASK		(LOG_BUF_LEN-1)

// Log levels.
#define KERN_EMERG	"<0>"	//!< system is unusable
#define KERN_ALERT	"<1>"   //!< action must be taken immediately
#define KERN_CRIT	"<2>"   //!< critical conditions
#define KERN_ERR	"<3>"   //!< error conditions
#define KERN_WARNING	"<4>"   //!< warning conditions
#define KERN_NOTICE	"<5>"   //!< normal but significant condition
#define KERN_INFO	"<6>"   //!< informational
#define KERN_DEBUG	"<7>"   //!< debug-level messages

//! The default log level: KERN_WARNING.
#define DEFAULT_CONSOLE_LOGLEVEL	4
//! The minimum console log level: KERN_ALERT.
#define MINIMUM_CONSOLE_LOGLEVEL	1
//! The maximum console log level: KERN_DEBUG (print everything!)
#define MAXIMUM_CONSOLE_LOGLEVEL	7

// --- Prototypes ----------------------------------------------------- //

long sys_syslog( int type, char *buf, int len );
int printk( const char *fmt, ... );

#endif
