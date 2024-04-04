/*!	\file drivers/keyboard/keyboard.c
 *	\brief Low level keyboard driver.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-25\n
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#include <const.h>
#include <stdio.h>

#include <arch/delay.h>
#include <arch/i386.h>
#include <arch/interrupt.h>
#include <arch/panic.h>

#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/queue.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>

#include <kernel/keyboard.h>

/** \ingroup Drivers
 *  \defgroup KeyboardDriver Keyboard
 *  The floppy disk driver.
 *  @{
 */

//! US keyboard keymap :: regular keys.
static
word regular[128] = {
  0x0000,0x011B,0x0231,0x0332,0x0433,0x0534,0x0635,0x0736,0x0837,0x0938,0x0A39,0x0B30,0x0C2D,0x0D3D,0x0E08,0x0F09,
  0x1071,0x1177,0x1265,0x1372,0x1474,0x1579,0x1675,0x1769,0x186F,0x1970,0x1A5B,0x1B5D,0x000D,0x1D00,0x1E61,0x1F73,
  0x2064,0x2166,0x2267,0x2368,0x246A,0x256B,0x266C,0x273B,0x2827,0x2960,0x2A00,0x2B5C,0x2C7A,0x2D78,0x2E63,0x2F76,
  0x3062,0x316E,0x326D,0x332C,0x342E,0x352F,0x3600,0x372A,0x3800,0x3920,0x3A00,0x3B00,0x3C00,0x3D00,0x3E00,0x3F00,
  0x4000,0x4100,0x4200,0x4300,0x4400,0x4500,0x4600,0x4700,0x4800,0x4900,0x4A2D,0x4B00,0x4C00,0x4D00,0x4E2B,0x4F00,
  0x5000,0x5100,0x5200,0x5300,0x5400,0x5500,0x5600,0x8500,0x8600,0x0000,0x0000,0x5B00,0x5C00,0x5D00
};

//! US keyboard keymap :: "with SHIFT" keys.
static
word with_shift[128] = {
  0x0000,0x011B,0x0221,0x0340,0x0423,0x0524,0x0625,0x075E,0x0826,0x092A,0x0A28,0x0B29,0x0C5F,0x0D2B,0x0E08,0x0F00,
  0x1051,0x1157,0x1245,0x1352,0x1454,0x1559,0x1655,0x1749,0x184F,0x1950,0x1A7B,0x1B7D,0x000D,0x1D00,0x1E41,0x1F53,
  0x2044,0x2146,0x2247,0x2348,0x244A,0x254B,0x264C,0x273A,0x2822,0x297E,0x2A00,0x2B7C,0x2C5A,0x2D58,0x2E43,0x2F56,
  0x3042,0x314E,0x324D,0x333C,0x343E,0x353F,0x3600,0x372A,0x3800,0x3920,0x3A00,0x5400,0x5500,0x5600,0x5700,0x5800,
  0x5900,0x5A00,0x5B00,0x5C00,0x5D00,0x4500,0x4600,0x4700,0x4800,0x4900,0x4A2D,0x4B00,0x4C00,0x4D00,0x4E2B,0x4F00,
  0x5000,0x5100,0x5200,0x5300,0x5400,0x5500,0x5600,0x8700,0x8800,0x0000,0x0000,0x5B00,0x5C00,0x5D00
};

//! US keyboard keymap :: "with ALT" keys.
static
word with_alt[128] = {
  0x0000,0x0100,0x7800,0x7900,0x7A00,0x7B00,0x7C00,0x7D00,0x7E00,0x7F00,0x8000,0x8100,0x8200,0x8300,0x0E00,0xA500,
  0x1000,0x1100,0x1200,0x1300,0x1400,0x1500,0x1600,0x1700,0x1800,0x1900,0x1A00,0x1B00,0x1C00,0x1D00,0x1E00,0x1F00,
  0x2000,0x2100,0x2200,0x2300,0x2400,0x2500,0x2600,0x2700,0x2800,0x2900,0x2A00,0x2B00,0x2C00,0x2D00,0x2E00,0x2F00,
  0x3000,0x3100,0x3200,0x3300,0x3400,0x3500,0x3600,0x3700,0x3800,0x3900,0x3A00,0x6800,0x6900,0x6A00,0x6B00,0x6C00,
  0x6D00,0x6E00,0x6F00,0x7000,0x7100,0x4500,0x4600,0x9700,0x9800,0x9900,0x4A00,0x9B00,0x9C00,0x9D00,0x4E00,0x9F00,
  0xA000,0xA100,0xA200,0xA300,0x5400,0x5500,0x5600,0x8B00,0x8C00
};

//! US keyboard keymap :: "with CTRL" keys.
static
word with_control[128] = {
  0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x9400,
  0x1011,0x1117,0x1205,0x1312,0x1414,0x1519,0x1615,0x1709,0x180F,0x1910,0x0000,0x0000,0x1C0A,0x1D00,0x1E01,0x1F13,
  0x2004,0x2106,0x2207,0x2308,0x240A,0x250B,0x260C,0x0000,0x0000,0x0000,0x2A00,0x0000,0x2C1A,0x2D18,0x2E03,0x2F16,
  0x3002,0x310E,0x320D,0x0000,0x0000,0x9500,0x3600,0x9600,0x3800,0x3920,0x3A00,0x5E00,0x5F00,0x6000,0x6100,0x6200,
  0x6300,0x6400,0x6500,0x6600,0x6700,0x4500,0x4600,0x7700,0x8D00,0x8400,0x8E00,0x7300,0x8F00,0x7400,0x9000,0x7500,
  0x9100,0x7600,0x9200,0x9300,0x5400,0x5500,0x5600,0x8900,0x8A00
};

//! The keypad map.
static
byte keypad_char[] = {'7','8','9','-','4','5','6','+','1','2','3','0','.'};

//! SHIFT flag.
static byte shift=0;
//! CTRL flag.
static byte ctrl=0;
//! ALT flag.
static byte alt=0;

//! Queue for the task that are waiting for the keyboard.
DECLARE_WAITQUEUE( keyb_wait );

//! \brief Wait after a keyboard operation.
/**
 *	\todo Put here a timeout!
 */
void keyb_wait_controller()
{
	int retries = 1000;

	while( ((inb(KEYB_STATUS) & KEYB_BUSY) == KEYB_BUSY) )
	{
		udelay( 50 );
		if( --retries == 0 )
			break;
	}
}

//! \brief Update keyboard leds.
void update_leds()
{
	int leds;
	console_t *curr_cons = get_console_addr( get_curr_console() );
	uint32_t flags;

	leds =	(curr_cons->scroll_lock) |
		(curr_cons->num_lock << 1) |
		(curr_cons->caps_lock << 2);

	local_irq_save( flags );

	keyb_wait_controller();
	outportb(KEYB_PORT, KEYB_LED_CODE);
	keyb_wait_controller();
	outportb(KEYB_PORT, leds);

	local_irq_restore( flags );
}

//! \brief Get the hitten key keyboard code.
//! \return The hitten key scan code.
word scan_key()
{
	static int code, val;
	uint32_t flags;

	local_irq_save( flags );

	code = inportb(KEYB_PORT);	// Get scan code
	val =  inportb(KEYB_ACK);	// Get keyboard acknowledge
	outportb(KEYB_ACK, val | 0x80); // Disable bit 7
	outportb(KEYB_ACK, val);	// Send that back

	local_irq_restore( flags );
	return( code );
}

/** \ingroup Handlers */
//! \brief
//!	This is the keyboard interrupt handler routine. It is
//!	invoked every time a key is pressed or released on the
//!	keyboard.
void keyboard_handler( irq_context_t *c )
{
	int console;
	console_t *curr_cons;
	word code;
	word keypressed = scan_key();

	// Get the current console structure.
	curr_cons = get_console_addr( get_curr_console() );

	// If CTRL+ALT+Canc => Reboot the system.
	if (ctrl==1 && alt==1 && keypressed==DEL_SCAN)
		reboot();

	if (alt==1) code = with_alt[keypressed];
	else if (ctrl==1) code = with_control[keypressed];
	else
	{
		if (curr_cons->caps_lock==0 && shift==0) code = regular[keypressed];
		else if (curr_cons->caps_lock==0 && shift==1) code = with_shift[keypressed];
		else if (curr_cons->caps_lock==1 && shift==0)
		{
			code = regular[keypressed];
			if (((code&0xFF)>='a') && ((code&0xFF)<='z')) code-='a'-'A';
		}
		else
		{
			code = with_shift[keypressed];
			if (((code&0xFF)>='A') && ((code&0xFF)<='Z')) code+='a'-'A';
		}
		if ((curr_cons->num_lock!=shift) && (keypressed>=71 && keypressed<=83))
			code = keypad_char[keypressed-71];
	}

 	// Print the char only if it's not released (bit 8 set).
	switch (keypressed)
	{
		case 42:	// LShift
		shift=1;
		break;

		case (42+128):
		shift=0;
		break;

		case 54:	// RShift
		shift=1;
		break;

		case (54+128):
		shift=0;
		break;

		case 56:	// ALT
		alt=1;
		break;

		case (56+128):
		alt=0;
		break;

		case 29:	// CTRL
		ctrl=1;
		break;

		case (29+128):
		ctrl=0;
		break;

		case 58:	// CAPS LOCK
		curr_cons->caps_lock ^= 1;
		update_leds();
		break;

		case 69:	// NUM LOCK
		curr_cons->num_lock ^= 1;
		update_leds();
		break;

		case 70:	// SCROLL LOCK
		curr_cons->scroll_lock ^= 1;
		update_leds();
		break;

		default:

		// Update keyboard buffer. This is an interrupt
		// handler so we are already in mutual exclusion!!!

		if (!(keypressed & 0x80))
		{
#if 0
			if( code == CTRL_C )
			{
				// TODO: User kill foreground task.
				return;
			}
#endif
			if ( (code>=ALT_F1) && (code<=ALT_F10) )
			{
				// Change console.
				console = ((code-ALT_F1) >> 8) + 1;
				switch_to_console( console );
				return;
			}
			// A key has been released => update the buffer
			if ( (curr_cons->keyb_buf_count) < KEYB_BUF_DIM )
			{
				(curr_cons->keyb_buf_count)++;
				if ( ++(curr_cons->keyb_buf_write) >= KEYB_BUF_DIM )
					curr_cons->keyb_buf_write = 0;
				curr_cons->keyb_buffer[curr_cons->keyb_buf_write] = code;

				wakeup_queue( keyb_wait );
			}
			else
			{
				printk( KERN_INFO
					"%s: keyboard buffer for console tty%i full!\n",
					__FUNCTION__, get_curr_console() );
			}
		}
		break;
	}
}

//! \brief Initialize the low-level keyboard driver.
void __INIT__ init_keyboard()
{
	// Set typematic delay as short as possible.
	outportb(KEYB_PORT, KEYB_SET_TYPEMATIC);
	keyb_wait_controller();
	// typematic 0 means "MODE CON RATE=30 DELAY=1".
	outportb(KEYB_PORT, 0);

	// Initialize leds.
	update_leds();

	// Install the keyboard handler.
	install_trap_handler( KEYBOARD_IRQ, (void *)&keyboard_handler );
}

//! \brief
//!	Wait for keypressed and return the hitten character ASCII code.
//! \return The hitten character ASCII code.
int kgetchar()
{
	task_t *current = get_curr_task();
	console_t *cons = get_console_addr( current->console );
	register int __key;
	uint32_t flags;

	local_irq_save( flags );

	for( ; ; )
	{
		if( cons->keyb_buf_count )
			break;
		wait_event( keyb_wait, cons->keyb_buf_count );
	}

	// Update keyboard buffer in mutual exclusion.
	(cons->keyb_buf_count)--;
	if ( ++(cons->keyb_buf_read) >= KEYB_BUF_DIM )
		cons->keyb_buf_read = 0;
	__key = (int)( cons->keyb_buffer[ cons->keyb_buf_read ] );

	local_irq_restore( flags );

	if( __key == CTRL_D )
	{
		return( EOF );
	}

	return( __key );
}

//! \brief
//!	Return the first key in the buffer, without waiting if
//!	the buffer is empty.
//! \return
//!	The hitten character ASCII code if a character has
//!	been hitten or -1 if error.
int keyb_read()
{
	console_t *cons = get_console_addr( get_curr_task()->console );
	int key;
	uint32_t flags;

	local_irq_save( flags );

	// If the task's console is not the current console return.
	if( get_curr_task()->console != get_curr_console() )
	{
		local_irq_restore( flags );
		return( -1 );
	}

	// If the keyboard buffer is empty return NULL.
	if ( cons->keyb_buf_count==0 )
	{
		local_irq_restore( flags );
		return( NULL );
	}

	// Update keyboard buffer in mutual exclusion.
	(cons->keyb_buf_count)--;
	if ( ++(cons->keyb_buf_read) >= KEYB_BUF_DIM )
		cons->keyb_buf_read = 0;
	key = (int)( cons->keyb_buffer[ cons->keyb_buf_read ] );

	local_irq_restore( flags );

	if( key == CTRL_C )
	{
		return( EOF );
	}

	return( key );
}

/** @} */ // end of KeyboardDriver
