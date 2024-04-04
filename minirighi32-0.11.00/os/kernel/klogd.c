/*!	\file kernel/klogd.c
 *	\brief The kernel log daemon.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-23
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#include <const.h>
#include <errno.h>
#include <stdio.h>

#include <arch/clock.h>
#include <arch/i386.h>
#include <kernel/config.h>
#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/serial.h>
#include <kernel/time.h>

//! Log level colors.
static char log_level_cols [] =
{
	RED,		// KERN_EMERG
	LIGHT_RED,	// KERN_ALERT
	YELLOW,		// KERN_CRIT
	GREEN,		// KERN_ERR
	LIGHT_GREEN,	// KERN_WARNING
	WHITE,		// KERN_NOTICE
	LIGHT_GREY,	// KERN_INFO
	GREY,		// KERN_DEBUG
};

#ifdef CONFIG_REMOTE_KDBG
//! Print a buffer to the serial port.
static __INLINE__ void tty_putchar( char c )
{
	timeout_t to;

	// Poll the line status register (base_port+5) and
	// set a timeout of 1 sec.
	set_timeout( &to, 1000 );
	while(
		((in(CONFIG_REMOTE_KDBG_PORT+5) & 0x64)==0)
		&&
		!(is_timeout(&to))
	);
	// Send the character to the serial port.
	out( CONFIG_REMOTE_KDBG_PORT, c );
}
#endif

//! The kernel log daemon.
void klogd( int argc, char **argv )
{
	char log_buf[ LOG_BUF_LEN ];
	int len, i, color;

	// Open the log.
	sys_syslog( 0, NULL, 0 );
	for( ; ; )
	{
		while( (len=sys_syslog(2, log_buf, sizeof(log_buf)))>0 )
		{
			// Get the log level.
			color = log_level_cols[ log_buf[1]-'0' ];
			// Set the console color.
			set_color( color );
			// Print the messages.
			for( i=3; i<len; i++ )
			{
				if( log_buf[i]==0 )
				{
					if( i==(len-1) )
						break;
					// Get the log level.
					color = log_level_cols[ log_buf[i+2]-'0' ];
					// Set the console color.
					set_color( color );
					i+=3;
				}
				else
				{
					kputchar( log_buf[i] );
					#ifdef CONFIG_REMOTE_KDBG
					tty_putchar( log_buf[i] );
					#endif
				}
			}
		}
	}
	// Close the log.
	sys_syslog( 1, NULL, 0 );
}
