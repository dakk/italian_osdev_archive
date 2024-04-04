/*!	\file kernel/console.c
 *	\brief Virtual consoles.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-01
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <const.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <arch/mem.h>

#include <kernel/keyboard.h>
#include <kernel/kmalloc.h>
#include <kernel/sched.h>
#include <kernel/video.h>

#include <kernel/console.h>

//! The address of the video buffer in memory.
#define VIDEO_BUFFER	VIDEO_MEM_ADDRESS
//! The size of the video buffer (in standard text-mode).
#define VIDEO_BUF_DIM	(k_get_crt_width()*k_get_crt_height()*sizeof(word))

//! Virual consoles buffer (console #0 is the special console).
static console_t vir_cons[K_TOT_VIR_CONS+1];

//! Index of the current console.
static int curr_cons = 1;

// --- Consoles managemet operators ---					//

//! \brief Get the address of a console structure.
//! \param c The console id.
//! \return The address of the console structure.
//! \exception NULL Console \p c does not exists.
console_t *get_console_addr(int c)
{
	if ( (c>=0) && (c<=K_TOT_VIR_CONS) )
		return( (console_t *)(&vir_cons[c]) );

	return((console_t *)NULL);
}

//! \brief Get the current console id.
//! \return The current console id.
int get_curr_console()
{
	return(curr_cons);
}

//! \brief Set the current console.
//! \param c The console id to set as current console.
//! \return
//!	\li #TRUE if \p c is a valid console id.
//!	\li #FALSE if  \p c is not a valid console id.
bool set_curr_console(int c)
{
	if ( (c > 0) && (c <= K_TOT_VIR_CONS) )
		curr_cons = c;
	else
		return(FALSE);

	return(TRUE);
}

//! \brief Change the current console and refresh the screen.
//! \param c The console to switch to.
//! \return
//!	\li #TRUE if \p c is a valid console id.
//!	\li #FALSE if  \p c is not a valid console id.
bool switch_to_console(int c)
{
	if ( c == curr_cons)
		// Attempt to switch to the current console		//
		return( TRUE );

	if( c > K_TOT_VIR_CONS )
		return( FALSE );

	// Copy old console parameters from video memory		//
	memcpy(vir_cons[curr_cons].vid_buffer, vir_cons[0].vid_buffer, VIDEO_BUF_DIM);
	vir_cons[curr_cons].cur_pos = vir_cons[0].cur_pos;
	vir_cons[curr_cons].cur_color = vir_cons[0].cur_color;

	if ( set_curr_console(c) )
	{
		// Set new console parameters to the video memory	//
		memcpy(vir_cons[0].vid_buffer, vir_cons[c].vid_buffer, VIDEO_BUF_DIM);
		k_set_cur_pos( (console_t *)(&vir_cons[0]), vir_cons[c].cur_pos );
		k_set_color( (console_t *)(&vir_cons[0]), vir_cons[c].cur_color );

		// Set keyboard parameters of the new selected console	//
		update_leds();

		return( TRUE );
	}
	return( FALSE );
}

// --- Video operators --- 						//

//! \brief Clear the screen.
void clrscr()
{
	task_t *p;

	if ( !(p=get_curr_task()) )
	{
		// If there is no task clear the main console		//
		video_clrscr( (console_t *)(&vir_cons[0]) );
		return;
	}

	if ( p->console==curr_cons )
		// The task is running in the current console.		//
		// So clear the main console				//
		video_clrscr( (console_t *)(&vir_cons[0]) );
	else
		// The task is not running in the current console.	//
		// Clear the task console				//
		video_clrscr( (console_t *)(&vir_cons[p->console]) );
}

//! \brief Get the current text color of the console.
//! \return The value of the color.
byte get_color()
{
	task_t *p;

	if ( !(p=get_curr_task()) )
	{
		// If there is no task return the main console color	//
		return( k_get_color((console_t *)(&vir_cons[0])) );
	}

	if ( p->console==curr_cons )
		// The task is running in the current console.		//
		// So return the main console color			//
		return( k_get_color((console_t *)(&vir_cons[0])) );
	else
		// The task is not running in the current console.	//
		// Return the task console color			//
		return( k_get_color((console_t *)(&vir_cons[p->console])) );
}

//! \brief Set the text color for the current console.
//! \param attrib The color value.
void set_color(byte attrib)
{
	task_t *p;

	if ( !(p=get_curr_task()) )
	{
		// If there is no task set the main console color	//
		k_set_color( (console_t *)(&vir_cons[0]), attrib );
		return;
	}

	if ( p->console==curr_cons )
		// The task is running in the current console.		//
		// So set the main console color			//
		k_set_color( (console_t *)(&vir_cons[0]), attrib );
	else
		// The task is not running in the current console.	//
		// Set the task console color				//
		k_set_color( (console_t *)(&vir_cons[p->console]), attrib );
}

//! \brief Scroll the console up.
void scroll_up()
{
	task_t *p;

	if ( !(p=get_curr_task()) )
	{
		// If there is no task execute in the video buffer	//
		k_scroll_up( (console_t *)(&vir_cons[0]) );
		return;
	}

	if ( p->console==curr_cons )
		// The task is running in the current console.		//
		// So execute directly to the video memory		//
		k_scroll_up( (console_t *)(&vir_cons[0]) );
	else
		// The task is not running in the current console.	//
		// Execute into its video buffer			//
		k_scroll_up( (console_t *)(&vir_cons[p->console]) );
}

//! \brief Place the cursor at the (\p x, \p y) coordinates.
//! \note
//!	If \p x or \p y get a value of -1 they does not change
//!	their value.
void gotoxy(int x, int y)
{
	task_t *p;

	if ( !(p=get_curr_task()) )
	{
		// If there is no task print to the video buffer	//
		k_gotoxy((console_t *)(&vir_cons[0]), x, y);
		return;
	}

	if ( p->console==curr_cons )
		// The task is running in the current console.		//
		// So execute directly to the video memory		//
		k_gotoxy((console_t *)(&vir_cons[0]), x, y);
	else
		// The task is not running in the current console.	//
		// Execute into its video buffer			//
		k_gotoxy((console_t *)(&vir_cons[p->console]), x, y);
}

//! \brief Put a character on the current console.
//! \param c The ASCII value of the character.
int kputchar( int c )
{
	task_t *p = get_curr_task();
	uint32_t flags;

	local_irq_save( flags );

	if ( !p )
	{
		// If there is no task print to the video buffer.
		video_putchar((console_t *)(&vir_cons[0]), c);

		local_irq_restore( flags );
		return( 0 );
	}

	if ( p->console==curr_cons )
	{
		// The task is running in the current console.
		// So print directly to the video memory.
		video_putchar((console_t *)(&vir_cons[0]), c);
	}
	else
	{
		// The task is not running in the current console.
		// Print into its video buffer.
		video_putchar((console_t *)(&vir_cons[p->console]), c);
	}

	local_irq_restore( flags );

	return( 0 );
}

//! \brief Print a string to the current console.
//! \param message The format of the message.
//! \param ... The variables to print.
int kprintf(const char *fmt, ...)
{
	char buf[1024];
	va_list args;
	int i;

	va_start( args, fmt );

	vsnprintf( buf, sizeof(buf), fmt, args );
	va_end( args );

	for( i=0; i<1024; i++ )
	{
		if( !buf[i] ) break;
		kputchar( buf[i] );
	}

	return( i );
}

// --- Initialization routines ---------------------------------------- //

//! \brief Initialize the special console #0 as the video memory buffer.
void __INIT__ init_boot_console()
{
	vir_cons[0].vid_buffer = (word *)0xb8000;
	vir_cons[0].cur_pos = 0;
	vir_cons[0].cur_color = DEFAULT_COLOR;

	// Clear the screen.
	video_clrscr( (console_t *)(&vir_cons[0]) );
}

//! \brief Initialize the special console #0 as the video memory buffer.
void __INIT__ init_main_console()
{
	vir_cons[0].vid_buffer = (word *)VIDEO_BUFFER;
	vir_cons[0].cur_pos = 0;
	vir_cons[0].cur_color = DEFAULT_COLOR;

	// Clear the screen.
	video_clrscr( (console_t *)(&vir_cons[0]) );
}

//! \brief Allocate and initialize memory space for virtual consoles.
//! \note
//!	Before you can run this procedure you have to initialize the
//!	virtual memory manager.
void __INIT__ create_virtual_console()
{
	int i;

	// Initialize virtual consoles.
	for ( i = 1; i < (K_TOT_VIR_CONS+1); i++ )
	{
		vir_cons[i].vid_buffer = kmalloc( VIDEO_BUF_DIM, GFP_KERNEL );
		bzero( vir_cons[i].vid_buffer, VIDEO_BUF_DIM );
		vir_cons[i].cur_pos = 0;
		vir_cons[i].cur_color = DEFAULT_COLOR;
	}
	// Copy the current video parameters into the first
	// virtual console.
	memcpy(vir_cons[1].vid_buffer, vir_cons[0].vid_buffer, VIDEO_BUF_DIM);
	vir_cons[1].cur_pos = vir_cons[0].cur_pos;
	vir_cons[1].cur_color = vir_cons[0].cur_color;

	// Automatic switch to the first virtual console.
	curr_cons = 1;
}
