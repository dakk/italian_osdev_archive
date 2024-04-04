/*!	\file kernel/debug.c
 *	\brief Debug operations.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-04
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#include <errno.h>
#include <stdio.h>

#include <kernel/console.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/debug.h>

//! The default log level.
unsigned int console_loglevel = DEFAULT_CONSOLE_LOGLEVEL;

//! The kernel log ring buffer.
static char log_buf[LOG_BUF_LEN];

//! Queue of task waiting for the log book.
DECLARE_WAITQUEUE( log_wait );

//! A semaphore to preserve the mutual exclusion of the log buffer.
DECLARE_MUTEX( log_buf_mutex );

//! First character to read from the buffer.
static unsigned long log_start=0;
//! Last character written into the buffer + 1.
static unsigned long log_end=0;
//! How many characters have been written into the log buffer.
static unsigned long logged_chars=0;

//! Access to the char #i of the buffer.
#define LOG_BUF( i )	( log_buf[(i) & LOG_BUF_MASK] )

//! \brief System call syslog implementation.
//! \param type
//!	\li 0 -- Close the log.
//!	\li 1 -- Open the log.
//!	\li 2 -- Read from the log.
//!	\li 3 -- Read all messages remaining in the ring buffer.
//!	\li 4 -- Read and clear all messages remaining in the ring
//!		buffer.
//!	\li 5 -- Clear the ring buffer.
//!	\li 6 -- Disable printks to console.
//!	\li 7 -- Enable printks to console.
//!	\li 8 -- Set level of messages printed to console.
//!	\li 9 -- Return number of unread characters in the log buffer.
//! \param buf
//!	The destination buffer.
//! \param len
//!	The length of the buffer.
//! \return
//!	The number of characters actually read.
int do_syslog(int type, char *buf, int len)
{
	int i=0, j=0;
	int count, limit, do_clear=0;
	int error=0;

	switch( type )
	{
		case 0:
		// Close the log.
		break;

		case 1:
		// Open the log.
		break;

		case 2:
			// Read from the log.
			if( !buf || len<0 )
				return( -EINVAL );
			if( !len )
				return( 0 );

			// Wait until the buffer is not empty.
			wait_event( log_wait, logged_chars );

			DOWN( &log_buf_mutex );
			while( (logged_chars) && (i<len) )
			{
				buf[i] = LOG_BUF( log_start );
				log_start++;
				logged_chars--;
				UP( &log_buf_mutex );
				i++;
				DOWN( &log_buf_mutex );
			}
			UP( &log_buf_mutex );
			error = i;
		break;

		case 4:
			// Read/clear last kernel messages.
			do_clear = 1;
			// FALL THRU.
		case 3:
			// Read last kernel messages.
			if( !buf || len<0 )
				return( -EINVAL );
			if( !len )
				return( 0 );
			count = len;
			if( count>LOG_BUF_LEN )
				count = LOG_BUF_LEN;

			DOWN( &log_buf_mutex );

			// Normalize the char counter.
			if( count>logged_chars )
				count = logged_chars;
			// We have also to clear the buffer?
			if( do_clear )
				logged_chars = 0;
			// Begin to read.
			limit = log_end;
			for( i=0; i<count; i++ )
			{
				j = limit-1-i;
				if( j+LOG_BUF_LEN<log_end )
					break;
				buf[count-1-i] = LOG_BUF( j );
			}

			UP( &log_buf_mutex );

			error = i;
			if( i!=count )
			{
				int offset = count-error;
				// Correct the user buffer.
				for( i=0; i<error; i++ )
				{
					buf[i] = buf[i+offset];
				}
			}
		break;

		case 5:
			// Clear the log book.
			DOWN( &log_buf_mutex );
			logged_chars = 0;
			UP( &log_buf_mutex );
		break;

		case 6:
			// Disable logging to console. Only messages
			// of the highest priority are displayed on
			// the console.
			DOWN( &log_buf_mutex );
			console_loglevel = MINIMUM_CONSOLE_LOGLEVEL;
			UP( &log_buf_mutex );
		break;

		case 7:
			// Enable logging to console to the default
			// log level.
			DOWN( &log_buf_mutex );
			console_loglevel = DEFAULT_CONSOLE_LOGLEVEL;
			UP( &log_buf_mutex );
		break;

		case 8:
			// Set level of messages printed to console.
			if( len<1 || len>8 )
				return( -EINVAL );
			if( len<MINIMUM_CONSOLE_LOGLEVEL )
				len = MINIMUM_CONSOLE_LOGLEVEL;
			DOWN( &log_buf_mutex );
			console_loglevel = len;
			UP( &log_buf_mutex );
		break;

		case 9:
			// Number of chars in the log buffer.
			DOWN( &log_buf_mutex );
			error = log_end - log_start;
			UP( &log_buf_mutex );
		break;

		default:
			// Invalid argument!
			error = -EINVAL;
		break;
	}
	return( error );
}

//! \brief The syslog system call.
/**
 *	See do_syslog() for more info.
 */
long sys_syslog( int type, char *buf, int len )
{
	// Check the privilege level.
	if( (type!=3 ) && !suser() )
		return( -EPERM );
	// Perform the syscall.
	return( do_syslog(type, buf, len) );
}

//! Print a character on the log buffer.
static void log_putchar( char c )
{
	LOG_BUF( log_end ) = c;
	log_end++;
	if( (log_end-log_start)>LOG_BUF_LEN )
		log_start = log_end - LOG_BUF_LEN;
	if( logged_chars<LOG_BUF_LEN )
		logged_chars++;
}

//! Debug print routine for the kernel.
int printk( const char *fmt, ... )
{
	va_list args;
	int len;
	char *p;
	unsigned int log_level = DEFAULT_CONSOLE_LOGLEVEL;
	static char printk_buf[1024];

	// Emit the output into the temporary buffer.
	va_start( args, fmt );
	len = vsnprintf( printk_buf, sizeof(printk_buf), fmt, args );
	va_end( args );

	DOWN( &log_buf_mutex );

	// If the caller didn't provide appropriate log level tags,
	// get the default log level.
	p = printk_buf;
	if( (p[0]=='<') && (p[1]>='0') && (p[1]<='7') && p[2]=='>' )
	{
		log_level = p[1] - '0';
	}
	else
	{
		// Print the default tag level on the log buffer.
		log_putchar( '<' );
		log_putchar( '0'+log_level );
		log_putchar( '<' );
	}

	// Print the temporary log buffer into the log book.
	for( ; *p; ++p )
	{
		log_putchar( *p );
	}
	log_putchar( *p );

	UP( &log_buf_mutex );

	// Print also in the current console if necessary.
	if( log_level <= console_loglevel )
	{
		if(	(printk_buf[0]=='<') &&
			(printk_buf[1]>='0') &&
			(printk_buf[1]<='7') &&
			(printk_buf[2]=='>') )
			kprintf( &printk_buf[3] );
		else
			kprintf( printk_buf );
	}

	// Wait every task in the log book queue.
	wakeup_queue( log_wait );

	return( len );
}
