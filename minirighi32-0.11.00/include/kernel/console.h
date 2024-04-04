/*!	\file include/kernel/console.h
 *	\brief Virtual consoles header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-01
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef CONSOLE_H
#define CONSOLE_H

// Console colors							//
#define BLACK 			0x00 //!< Black color.
#define BLUE			0x01 //!< Blue color.
#define GREEN			0x02 //!< Green color.
#define CYAN			0x03 //!< Cyan color.
#define RED	 		0x04 //!< Red color.
#define MAGENTA			0x05 //!< Magenta color.
#define BROWN	 		0x06 //!< Brown color.
#define LIGHT_GREY		0x07 //!< Light grey color.
#define GREY			0x08 //!< Grey color.
#define LIGHT_BLUE		0x09 //!< Light blue color.
#define LIGHT_GREEN		0x0A //!< Light green color.
#define LIGHT_CYAN		0x0B //!< Light cyan color.
#define LIGHT_RED		0x0C //!< Light red color.
#define LIGHT_MAGENTA		0x0D //!< Light magenta color.
#define YELLOW			0x0E //!< Yellow color.
#define WHITE			0x0F //!< White color.

//! The default color for the text of the console.
#define DEFAULT_COLOR 		0x07

//! Size of the console keyboard buffer.
#define KEYB_BUF_DIM		32

//! Number of virtual consoles (must be at least 1).
#define K_TOT_VIR_CONS		10

/** \ingroup Kernel
 *  \defgroup VirConsoles Virtual Consoles
 *  Functions to manage virtual text-based consoles.
 *  @{
 */

//! Virtual console structure.
typedef struct console
{
	// --- VIDEO ---						//
	word 	*vid_buffer;		//!< Video buffer.
	word	cur_pos;		//!< Cursor position.
	byte	cur_color;		//!< Current text color.

	// --- KEYBOARD ---						//
	byte	num_lock,		//!< NUM-LOCK flag.
		caps_lock,		//!< CAPS-LOCK flag.
		scroll_lock;		//!< SCROLL-LOCK flag.

	word	keyb_buf_read,		//!< The first character to read from the buffer.
		keyb_buf_write,		//!< The last character insered into the buffer.
		keyb_buf_count;		//!< How many characters are present into the buffer.

	//! The keyboard circular buffer.
	word	keyb_buffer[KEYB_BUF_DIM];
} console_t;

// Prototypes 							//

console_t *get_console_addr(int c);
int get_curr_console();
bool set_curr_console(int c);
bool switch_to_console(int c);

void clrscr();
byte get_color();
void set_color(byte attrib);
void scroll_up();
void gotoxy(int x, int y);

int kputchar(int c);
int kprintf(const char *fmt, ...);

void init_boot_console();
void init_main_console();
void create_virtual_console();

/** @} */ // end of VirConsoles

#endif
