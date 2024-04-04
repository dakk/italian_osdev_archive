/*!	\file drivers/video/video.c
 *	\brief Low level video controller 6845 driver.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-08
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <const.h>
#include <stdarg.h>
#include <string.h>

#include <arch/i386.h>
#include <arch/mem.h>
#include <arch/paging.h>

#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/semaphore.h>
#include <kernel/speaker.h>
#include <kernel/task.h>

#include <kernel/video.h>

/** \ingroup Drivers
 *  \defgroup VideoDriver Video Controller 6845
 *  The low level video controller 6845 driver.
 *  @{
 */

//! Video width (columns).
int	crt_width = 80;
//! Video height (rows).
int	crt_height = 25;

//! \brief Set the cursor size.
void video_set_cur_size( int u, int l )
{
	uint32_t flags;

        local_irq_save( flags );

	outb_p( VIDEO_CRT_PORT, 0x0a );
	outb_p( VIDEO_CRT_PORT + 1, u );
	outb_p( VIDEO_CRT_PORT, 0x0b );
	outb_p( VIDEO_CRT_PORT + 1, l );

	local_irq_restore( flags );
}

//! \brief Refresh cursor position for a selected console.
void move_cur( int pos )
{
	// Refresh video cursor position with cur_pos value.
	// Registers 14-15 tell the 6845 where to put the
	// cursor.
	outb_p( VIDEO_CRT_PORT + 0, 14 );
	outb_p( VIDEO_CRT_PORT + 1, pos >> 8 );
	outb_p( VIDEO_CRT_PORT + 0, 15);
	outb_p( VIDEO_CRT_PORT + 1, pos & 0xff );
}

//! \brief Clear the screen of the selected console.
//! \param console The selected console address.
void video_clrscr( console_t *console )
{
	uint32_t flags;

	local_irq_save( flags );

	memsetw( (word *)(console->vid_buffer),
		BLANK,
		crt_width*crt_height );
	console->cur_pos = 0;

	// Update the cursor position only for current console.
	if( console==get_console_addr(0) )
		move_cur( console->cur_pos );

	local_irq_restore( flags );
}

//! \brief Get the cursor position of the selected console.
//! \param console The selected console address.
//! \return
//!	The cursor position (in characters from the beginning
//!	of the video buffer).
int k_get_cur_pos( console_t *console )
{
	return( console->cur_pos );
}

//! \brief Set the cursor position of the selected console.
//! \param console The selected console address.
//! \param pos
//!	The cursor position (in characters from the beginning
//!	of the video buffer).
//! \return
//!	\li #TRUE on success;
//!	\li #FALSE if the position is over the buffer.
bool k_set_cur_pos( console_t *console, word pos )
{
	uint32_t flags;

	if ( pos < (crt_width*crt_height) )
	{
		local_irq_save( flags );

		console->cur_pos = pos;
		// Update the cursor position only for current console.
		if( console==get_console_addr(0) )
			move_cur( console->cur_pos );

		local_irq_restore( flags );

		return( TRUE );
	}
	return( FALSE );
}

//! \brief Get the current color of the selected console.
//! \param console The selected console address.
//! \return The current color code (see console.h).
int k_get_color( console_t *console )
{
	return( console->cur_color );
}

//! \brief Set the current color of the selected console.
//! \param console The selected console address.
//! \param attrib The current color code (see console.h).
void k_set_color( console_t *console, byte attrib )
{
	console->cur_color = attrib;
}

//! \brief Scroll up the screen of the selected console.
//! \param console The selected console address.
void k_scroll_up( console_t *console )
{
	uint32_t flags;

	local_irq_save( flags );

	// Scroll up.
	memcpy( (void *)(console->vid_buffer),
		((void *)(console->vid_buffer))+crt_width*2,
		crt_width*(crt_height-1)*2 );

	// Blank the bottom line of the screen.
	memsetw((void *)( ((void *)(console->vid_buffer)) + crt_width*(crt_height-1)*2),
		 BLANK, crt_width);

	(console->cur_pos)-=crt_width;

	// Update the cursor position only for current console.
	if( console==get_console_addr(0) )
		move_cur( console->cur_pos );

	local_irq_restore( flags );
}

//! \brief
//!	Set the (x, y) coordinates as the current position in the
//!	selected console.
//! \param console The selected console address.
//! \param x The column position.
//! \param y The row position.
void k_gotoxy(console_t *console, int x, int y)
{
	uint32_t flags;

	// Round x, y to CRT bounds 					//
	if (x != -1)
		x = x % crt_width;
	else
		x = console->cur_pos % crt_width;

	if (y != -1)
		y = y % crt_height;
	else
		y = console->cur_pos / crt_width;

	local_irq_save( flags );

	console->cur_pos = y*crt_width + x;
	// Update the cursor position only for current console.
	if( console==get_console_addr(0) )
		move_cur( console->cur_pos );

	local_irq_restore( flags );
}

//! \brief Put a character in the selected console.
//! \param console The selected console address.
//! \param c The ASCII code of the character to print.
void video_putchar( console_t *console, uint8_t c )
{
	uint32_t flags;

	local_irq_save( flags );

	switch (c)
	{
		case '\n':
			(console->cur_pos)+=crt_width;
			(console->cur_pos)-=(console->cur_pos)%crt_width;
			break;
		case '\r':
			(console->cur_pos)-=(console->cur_pos)%crt_width;
			break;
		case '\b':
			if ((console->cur_pos) > 0)
			{
				(console->vid_buffer)[--(console->cur_pos)] =
					((console->cur_color) << 8 ) | ' ';
			}
			break;
		case '\t':
			(console->cur_pos)+=8;
			break;
		case '\a': // CTRL+G => system beep! //
			beep();
			local_irq_restore( flags );
			return;
			break;

		default:
		{
			(console->vid_buffer)[console->cur_pos] = ((console->cur_color) << 8) | c;
			(console->cur_pos)++;
		}
	}

	// Check if cursor is at the bottom of the screen.
	if ( console->cur_pos >= (crt_width * crt_height) )
		k_scroll_up(console);

	// Update the cursor position only for current console.
	if( console==get_console_addr(0) )
		move_cur( console->cur_pos );

	local_irq_restore( flags );
}

//! \brief Get the CRT height (in characters).
//! \return The CRT height.
int k_get_crt_height()
{
	return( crt_height );
}

//! \brief Get the CRT width (in characters).
//! \return The CRT width.
int k_get_crt_width()
{
	return( crt_width );
}

//! \brief Initialize the video parameters.
void __INIT__ init_video()
{
	// Set the size of the screen.
	crt_width = 80;
	crt_height = 25;

	// Clear the screen.
	memsetw( (word *)VIDEO_MEM_ADDRESS,
		BLANK,
		crt_width*crt_height );

	// Set the cursor size & position.
	video_set_cur_size( 15, 31 );
	move_cur( 0 );

	printk( KERN_INFO "Console: [default] colour 80x25.\n" );
}

/** @} */ // end of VideoDriver
