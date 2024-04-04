/*!	\file drivers/rs232/serial.c
 *	\brief Serial/RS232 port driver.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-08
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <const.h>
#include <string.h>

#include <arch/i386.h>
#include <arch/interrupt.h>

#include <kernel/console.h>
#include <kernel/keyboard.h>
#include <kernel/sched.h>

#include <kernel/serial.h>

/** \ingroup Drivers
 *  \defgroup RS232Driver Serial Port (RS232)
 *  The RS232 COM-port driver.
 *  @{
 */

//! RS232 buffer for read operations :: head pointer.
static int rs232_read_head=0;
//! RS232 buffer for read operations :: tail pointer.
static int rs232_read_tail=0;

//! RS232 buffer for read operations.
byte rs232_read_buf[RS232_BUF_DIM];

//! RS232 buffer for write operations :: head pointer.
static int rs232_write_head=0;
//! RS232 buffer for write operations :: tail pointer.
static int rs232_write_tail=0;

//! RS232 buffer for write operations.
byte rs232_write_buf[RS232_BUF_DIM];

//! RS232 current port.
static word curr_port = COM1;

//! \brief RS232 common handler routine.
//! \param port The serial port.
void rs232_handler( word port )
{
	byte flag, ch;
	uint32_t flags;

	local_irq_save( flags );

	while(TRUE)
	{
		// Get a char from the serial port //
		flag = inportb(port+2);

		if (flag & 0x01) break;

		switch(flag & 0x06)
		{
			// Modem status - clear intr by reading modem status reg //
			case 0x00:
			(void)inportb(port+6);
			break;

			// Write char - Send the char to the serial port //
			case 0x02:
			if (rs232_write_head != rs232_write_tail)
			{
				ch = rs232_write_buf[rs232_write_head];
				rs232_write_head = (rs232_write_head+1)%RS232_BUF_DIM;
				outportb(port, ch);
			}
			// Inhibit TX interrupts if write buffer is empty (leave RX enabled) //
			if (rs232_write_head == rs232_write_tail)
				outportb(port+1, 0x01);
			break;

			// Read char - Get the char from the serial port //
			case 0x04:
			while(TRUE)
			{
				// Check for data ready //
				flag = inportb(port+5);
				if (!(flag & 0x01)) break;

				// Get the character //
				ch = inportb(port);

				// Store it into the buffer if it is not full //
				if (((rs232_read_tail+1)%RS232_BUF_DIM) != rs232_read_head)
				{
					rs232_read_buf[rs232_read_tail] = ch;
					rs232_read_tail = (rs232_read_tail+1)%RS232_BUF_DIM;
				}
			}
			break;

			// Line status - clear intr by reading line status reg //
			case 0x06:
			(void)inportb(port+5);
			break;
		}
	}

	local_irq_restore( flags );
}

/** \ingroup Handlers */
//! \brief The COM1 interrupt handler.
void rs232_handler_port1( irq_context_t *c )
{
	rs232_handler( PORT1 );
}

/** \ingroup Handlers */
//! \brief The COM2 interrupt handler.
void rs232_handler_port2( irq_context_t *c )
{
	rs232_handler( PORT2 );
}

//! \brief Close a specific RS232 port.
//! \param port The port to close.
void close_rs232(word port)
{
	// Turn off rs232 interrupt.
	outportb(port+1, 0);

	// Disable the FIFO.
	outportb(port+2, 0);

	// Disconnect UART from the ICU.
	outportb(port+4, 0);

	// Uninstall interrupt handler.
	if ( (port == COM1) || (port == COM3) )
		uninstall_handler( RS232_1_IRQ );
	else if ( (port == COM2) || (port == COM4) )
		uninstall_handler( RS232_2_IRQ );
}

//! \brief Initialize the RS232 interface with a bps rate.
//! \param port The serial port to initialize.
//! \param divisor The divisor to set the bps rate (see serial.h).
//! \param
void init_rs232(word port, byte divisor)
{
	// Install the IRQ handler routine				//
	if ( (port == COM1) || (port == COM3) )
		install_trap_handler( RS232_1_IRQ, (void *)rs232_handler_port1 );
	else if ( (port == COM2) || (port == COM4) )
		install_trap_handler( RS232_2_IRQ, (void *)rs232_handler_port2 );
	else
		return;

	// Turn off rs232 interrupt //
	outportb(port+1, 0);

	// Set DLAB on //
	outportb(port+3, 0x80);

	// Set baud rate - Divisor Latch Low Byte //
	outportb(port, divisor);
	// Set baud rate - Divisor Latch High Byte //
	outportb(port+1, 0x00);

	// 8 bits, no parity, 1 stop-bit //
	outportb(port+3, 0x03);

	// FIFO Control Register //
	outportb(port+2, 0xC7);

	// Turn on DTR, RTS and OUT2 //
	outportb(port+4, 0x0B);

	// Enable interrupts when data is received //
	outportb(port+1, 0x01);

	// Clear receiver //
	(void)inportb(port);
	// Clear line status //
	(void)inportb(port+5);
	// Clear modem status //
	(void)inportb(port+6);
}

//! \brief
//!	Get a character from the current serial port.
//! \return
//!	The ASCII code of the character read.
byte rs232_getchar()
{
	byte temp;
	uint32_t flags;

	local_irq_save( flags );

	// If the buffer is empty enable interrupts and wait... //
	while( rs232_read_head==rs232_read_tail )
	{
		enable();
		schedule();
		disable();
	}

	// Update the RS232 buffer in mutual exclusion & get the char //
	temp = rs232_read_buf[rs232_read_head];
	rs232_read_head = (rs232_read_head+1)%RS232_BUF_DIM;

	local_irq_restore( flags );

	return(temp);
}

//! \brief
//!	Write a character to the current serial port.
//! \param c
//!	The ASCII code of the character to write.
void rs232_putchar(byte c)
{
	uint32_t flags;

	// Check if the write buffer is full //
	if (((rs232_write_tail+1)%RS232_BUF_DIM) == rs232_write_head) return;

	local_irq_save( flags );

	// Write the char to the buffer in mutual exclusion //
	rs232_write_buf[rs232_write_tail] = c;
	rs232_write_tail = (rs232_write_tail+1)%RS232_BUF_DIM;

	local_irq_restore( flags );

	// Enable RX & TX interrupts //
	outportb(curr_port+1, 0x03);
}

//! \brief
//!	Get permanently characters from the serial port and print it
//!	to the screen.
//! \note
//!	This is no-exit function, you have to use it not as a simple
//!	procedure, but as a kernel-thread (see task.c).
void rs232_task( int argc, char **argv )
{
	while(TRUE)
	{
		kputchar(rs232_getchar());
	}
}

//! \brief Chat with a modem or a serial device.
//! \param argv
//!	A string that contains the bps rate to initialize the
//!	serial port connection.
void rs232_chat(char *argv)
{
	static char *default_bps = "38400";
	task_t *child;
	word key;

	kprintf(	"\n\rOpening RS232-1 interface (COM1)... "
		"\n\rPress CTRL-X to quit."	);

	if (strcmp(argv, "2400")==0)	 	init_rs232(COM1, BPS_2400);
	else if (strcmp(argv, "4800")==0)	init_rs232(COM1, BPS_4800);
	else if (strcmp(argv, "9600")==0)	init_rs232(COM1, BPS_9600);
	else if (strcmp(argv, "19200")==0)	init_rs232(COM1, BPS_19200);
	else if (strcmp(argv, "38400")==0)	init_rs232(COM1, BPS_38400);
	else if (strcmp(argv, "56700")==0)	init_rs232(COM1, BPS_56700);
	else if (strcmp(argv, "115200")==0)	init_rs232(COM1, BPS_115200);
	else
	{
		// Default bps //
		argv = default_bps;
		init_rs232(COM1, BPS_38400);
	}

	kprintf("\n\rBitrate: %s bps", argv);
	child = create_thread( &rs232_task, 0, NULL, "rs232_task" );
	set_color(GREEN);
	kprintf("\r						[ OK ]\n\r");
	set_color(DEFAULT_COLOR);

	while(TRUE)
	{
		// Get a char from the keyboard //
		key = kgetchar();
		if (key==CTRL_X) break;
		if ((key&0xFF)==13) rs232_putchar(10);

		// Write the char to the serial port //
		rs232_putchar(key);
	}

	// Local echo of the ESCape char //
	kputchar((byte)CTRL_X);

	// Kill the rs232 task //
	kill(child->pid);

	// Close RS232 connection //
	close_rs232(curr_port);
	kprintf("\n\rBye.\n\r");
}

/** @} */ // end of RS232Driver
