/*!	\file kernel/main.c
 *	\brief Kernel Main routine
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-12\n
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

// --- MAIN GROUPS DEFINITION (for Doxygen)---------------------------- //

/*!	\mainpage
 *	\section main General Informations
 *		These are the references for the \ref readme,
 *		\ref install and \ref copying files.
 *	\section readme Readme
 *		\verbinclude README
 *	\section install Install
 *		\verbinclude INSTALL
 *	\section copying License
 *		\verbinclude COPYING
 */

/*!	\defgroup Kernel		Kernel
 *	The Minirighi kernel.
 *	@{
 *	@}
 */
	/*!	\ingroup Kernel
 	*	\defgroup Handlers		Handlers
 	*	System interrupt handlers.
 	*	@{
 	*	@}
 	*/
/*!	\defgroup Memory		Memory Manager
 *	The Memory manager.
 *	@{
 *	@}
 */
/*!	\defgroup Multitasking		Multitasking
 *	The Multitasking Manager.
 *	@{
 *	@}
 */
/*!	\defgroup FileSystem		File Systems
 *	The File systems.
 *	@{
 *	@}
 */
/*!	\defgroup Drivers		Device Drivers
 *	The Device drivers.
 *	@{
 *	@}
 */
/*!	\defgroup Network		Networking
 *	Network protocols.
 *	\image html network.jpg "Network layers on Minirighi"
 *	@{
 *	@}
 */
/*!	\defgroup UserInterface		User Interface
 *	Shell, GUI, etc...
 *	@{
 *	@}
 */
/*!	\defgroup Library		Library
 *	The set of libraries for Minirighi.
 *	@{
 *	@}
 */
	/*!	\ingroup Library
 	*	\defgroup LibC			LIBC
 	*	The ANSI-C library.
 	*	@{
 	*	@}
 	*/
	/*!	\ingroup Library
 	*	\defgroup LibMinirighi		LIBMinirighi
 	*	The Minirighi user library.
 	*	@{
 	*	@}
 	*/

// --- END of MAIN GROUPS DEFINITION (for Doxygen)--------------------- //

#include <const.h>
#include <stdarg.h>

#include <arch/clock.h>
#include <arch/cpu.h>
#include <arch/delay.h>
#include <arch/exception.h>
#include <arch/i386.h>
#include <arch/interrupt.h>
#include <arch/mem.h>
#include <arch/paging.h>
#include <arch/v86.h>

#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/dma.h>
#include <kernel/ext2.h>
#include <kernel/fat.h>
#include <kernel/floppy.h>
#include <kernel/Ide.h>
#include <kernel/kernel_map.h>
#include <kernel/keyboard.h>
#include <kernel/kmalloc.h>
#include <kernel/multiboot.h>
#include <kernel/queue.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <kernel/serial.h>
#include <kernel/speaker.h>
#include <kernel/task.h>
#include <kernel/time.h>
#include <kernel/video.h>

#include <kernel/shell.h>

// Extern kernel threads.

extern void klogd( int, char ** );

extern void kpagerd( int, char ** );

/** \ingroup Kernel
 *  \defgroup KernelMain Kernel Initialization
 *  The initialization routine.
 *  @{
 */

//! The multiboot information pointer from GRUB.
extern multiboot_info_t *boot_info;

extern size_t PHYS_MEM_DIM;

extern task_t *kpagd;

extern queue_t *ready_queue;

extern size_t KERNEL_INIT, KERNEL_END_INIT;

//! Free the unused space occupied by the initialization
//! routines.
static __INLINE__ void free_init_space()
{
	size_t addr;

	// Free the unused space occupied by the initialization
	// routines.
	dma_phys_free( (size_t)&KERNEL_INIT,
		(size_t)&KERNEL_END_INIT - (size_t)&KERNEL_INIT );

	// Unmap init pages.
	for( addr = (size_t)&KERNEL_INIT;
		addr < (size_t)&KERNEL_END_INIT; addr += PAGE_SIZE )
	{
		delete_page( addr );
	}
}

//! Initialization completed.
void __NORETURN__ __NOINLINE__ init_done()
{
	// Free the memory occpuied by the init routines.
	free_init_space();

	// Become the IDLE task.
	while( TRUE )
	{
		safe_idle();
		sleep_task( curr_task );
	}
}

//! \brief Print the [ OK ] tag.
void __INIT__ print_ok()
{
	set_color( DEFAULT_COLOR );
	kprintf( "\r\t\t\t\t\t\t[ " );
	set_color( GREEN );
	kprintf( "OK" );
	set_color( DEFAULT_COLOR );
	kprintf( " ]\n" );
}

//! \brief Print the [ ERROR ] tag.
void __INIT__ print_error()
{
	set_color( DEFAULT_COLOR );
	kprintf( "\r\t\t\t\t\t\t[ " );
	set_color( LIGHT_RED );
	kprintf( "ERROR" );
	set_color( DEFAULT_COLOR );
	kprintf( " ]\n" );
}

//! \brief
//! 	This is the beginning of the kernel!!!
//!	You can figure it as the \e init task...
//! \warning
//!	To obviate memory faults, \b first \b of \b all, we MUST initialize
//!	paging with the init_paging() routine.
void __INIT__ __NORETURN__ k_main()
{
	int i;

	// Initialize the standard console to allow the
	// print of eventual error messages before paging
	// is enabled.
	init_boot_console();

	// Initialize paging.
	init_paging();

	// Initialize video.
	init_video();

	// Initialize the main console to print messages.
	init_main_console();

	set_color( LIGHT_BLUE );
	kprintf( "\n< Starting system initialization >\n\n" );
	set_color( DEFAULT_COLOR );

	printk( KERN_INFO
		"Kernel is running at virtual address: %#010x\n",
		(size_t)&k_main );

	// Get some multiboot informations.
	boot_info = (multiboot_info_t *)PHYSICAL((dword)boot_info);
	if (boot_info->flags & 0x02)
	{
		printk( KERN_INFO
			"Total amount of conventional memory: %uKB\n",
			((size_t)boot_info->mem_lower) );
		printk( KERN_INFO
			"Total amount of extended memory: %uMB\n",
			PHYS_MEM_DIM/(1024*1024) );
	}
	else
	{
		printk( KERN_EMERG
			"\nError reading multiboot informations... halting system!\n" );
	}

	// Get CPU informations.
	kprintf("Detecting CPU...");
	init_cpu();
	print_ok();

	// Reprogram the PIC 8259 (interrupt controller).
	kprintf("Reprogramming PIC 8259...");
	reprogram_PIC();
	print_ok();

	// Installing GDT.
	kprintf("Installing kernel GDT...");
	install_GDT();
	print_ok();

	// Installing IDT.
	kprintf("Installing kernel IDT...");
	install_IDT();
	print_ok();

	// Initialize memory manager.
	kprintf("Initializing memory manager...");
	kmalloc_init();
	print_ok();

	// Initialize DMA allocator.
	kprintf("Initializing DMA allocator...");
	dma_alloc_init();
	print_ok();

	// Initialize multitasking.
	kprintf("Initializing multitasking...");
	init_multitasking();
	print_ok();

	// Initialize system clock.
	kprintf("Initializing system clock...");
	init_clock();
	print_ok();

	// Calibrating CPU loops per tick.
	kprintf("Calibrating loops per tick...");
	calibrate_loops_per_tick();
	print_ok();

	// Initialize the keyboard.
	kprintf("Initializing keyboard...");
	init_keyboard();
	print_ok();

	// Initialize the floppy.
	kprintf("Initializing floppy driver...");
	if ( init_floppy() )
		print_ok();
	else
		print_error();

	// Auto-mount FAT12 on the floppy.
	kprintf("Auto-mounting FAT12 file-system on floppy...");
	if ( fat12_mount() < 0 )
		print_error();
	else
		print_ok();

	// Initialize the ide driver.
	kprintf("Initializing ide driver...");
	InitIde();
	print_ok();

	// Initialize the ext2 file-system.
	kprintf("Auto-mounting ext2 file-system on /dev/hda1...");
	if ( init_ext2() )
		print_ok();
	else
		print_error();

	// Initialize virtual consoles.
	kprintf("Creating virtual consoles...");
	create_virtual_console();
	set_color(GREEN);
	print_ok();

	// Initialization complete.
	set_color( LIGHT_BLUE );
	kprintf( "\n< System initialization completed >\n" );
	set_color(WHITE);
	kprintf( "\n*** Welcome to %s - Kernel [v%s] ***\n",
		KERNEL_NAME, KERNEL_VERSION );
	set_color(DEFAULT_COLOR);

	disable();

	// Create the kpager daemon.
	kpagd = create_thread( kpagerd, 0, NULL, "kpagerd" );
	kpagd->console = 0;

	// Create the klogd daemon in the last console.
	curr_task->console = K_TOT_VIR_CONS;
	create_thread( klogd, 0, NULL, "klogd" );

	// Create a shell task for each console.
	for( i = 1; i < K_TOT_VIR_CONS; ++i )
	{
		curr_task->console = i;
		if ( i > 1 )
			clrscr();
		create_thread( shell, 0, NULL, "MiniShell" );
	}

	// Set the main console.
	set_curr_console( 1 );

	// Well done. Become the IDLE task...
	curr_task->console = 0;
	init_done();
}
/** @} */ // end of KernelMain
