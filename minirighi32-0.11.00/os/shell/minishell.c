/*!	\file shell/minishell.c
 *	\brief MiniShell - a very very simple shell.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-04-23 Andrea Righi:
 *			Added sync, mount, umount commands.\n
 *	\note Copyright (C) 2003-2004 Andrea Righi
 */

#include <const.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <arch/clock.h>
#include <arch/cpu.h>
#include <arch/i386.h>
#include <arch/paging.h>
#include <arch/panic.h>
#include <arch/v86.h>

#include <kernel/console.h>
#include <kernel/dma.h>
#include <kernel/elf32.h>
#include <kernel/ext2.h>
#include <kernel/fat.h>
#include <kernel/floppy.h>
#include <kernel/Ide.h>
#include <kernel/IdeDebug.h>
#include <kernel/IdeTimer.h> // must remove
#include <kernel/IdeLow.h>   // must remove
#include <kernel/keyboard.h>
#include <kernel/kmalloc.h>
#include <kernel/pci.h>
#include <kernel/speaker.h>
#include <kernel/sched.h>
#include <kernel/task.h>
#include <kernel/serial.h>
#include <kernel/time.h>
#include <kernel/net/arp.h>
#include <kernel/net/eth.h>

#include <kernel/shell.h>

/** \ingroup UserInterface
 *  \defgroup UIMiniShell MiniShell
 *  A simple text-based shell.
 *  @{
 */

#define SH_HELP			0
#define SH_UNAME		1
#define SH_CLEAR		2
#define SH_REBOOT		3
#define SH_DUMP			4
#define SH_READ			5
#define SH_WRITE		6
#define SH_PAGES		7
#define SH_MEM			8
#define SH_CHECKMEM		9
#define SH_FRAMES		10
#define SH_BG			11
#define SH_FREE			12
#define SH_SYNC			13
#define SH_MOUNT		14
#define SH_UMOUNT		15
#define SH_CPUID		16
#define SH_LS			17
#define SH_CAT			18
#define SH_CD			19
#define SH_PWD			20
#define SH_PS			21
#define SH_KILL			22
#define SH_RS232		23
#define SH_EXEC			24
#define SH_PCISCAN		25
#define SH_ETH_ARP		26
#define SH_RTL8139_INIT		27
#define SH_HALT			29
#define SH_RM			30
#define SH_ELF			31
#define SH_IDEREAD		32
#define SH_IDEWRITE		33
#define SH_IDEDEVICEINFO	34
#define SH_LS_EXT2		35
#define SH_CAT_EXT2		36
#define SH_CD_EXT2		37

//! The commands supported in MiniShell.
static CMD_t commands[] =
{
{ SH_ETH_ARP, "arp", "arp <destination>", "Send an ARP echo request to ip destination"},
{ SH_BG, "bg", "bg <filename>", "Execute a file in background (add the '&' at the end of the line instead)"},
{ SH_CAT, "cat", "cat <filename>", "Concatenate FILE to standard output"},
{ SH_CAT_EXT2, "cat2", "cat2 <filename>", "Concatenate ext2 FILE to standard output"},
{ SH_CD, "cd", "cd <dir>", "Change the current directory"},
{ SH_CD_EXT2, "cd2", "cd2 <dir>", "Change the current ext2 directory"},
{ SH_RS232, "chat", "chat <2400|4800|9600|19200|38400|56700|115200>", "Connect to /dev/modem on COM1"},
{ SH_CHECKMEM, "checkmem", "checkmem", "Create a background task to check frames stack integrity"},
{ SH_CLEAR, "clear", "clear", "Clear the screen"},
{ SH_CPUID, "cpuid", "cpuid", "Show a lot of informations about the CPU"},
{ SH_DUMP, "dump", "dump", "Dump the CPU registers"},
{ SH_ELF, "elf", "elf <filename>", "Check if filename is a valid ELF32 file"},
{ SH_EXEC, "fg", "fg <filename>", "Execute a program in foreground (you can simply type <filename> without fg)"},
{ SH_FREE, "free", "free", "Print memory informations"},
{ SH_HALT, "halt", "halt", "Power down the system (if possible, otherwise halt)"},
{ SH_HELP, "help", "help [cmd]", "Show the help of a command"},
{ SH_IDEDEVICEINFO, "idedeviceinfo","idedeviceinfo","Display complete info of a specified device"},
{ SH_IDEREAD, "ideread", "ideread", "Read a block from a device on a specified ide channel"},
{ SH_IDEWRITE, "idewrite", "idewrite", "Write a block to a device on specified ide channel"},
{ SH_RTL8139_INIT, "ifconfig", "ifconfig <up|down> [promisc]", "Initialize RTL8139 ethernet card"},
{ SH_KILL, "kill", "kill <pid>", "Kill the user process with the ID <pid>"},
{ SH_MEM, "kmem", "kmem", "Dump kernel memory map"},
{ SH_LS, "ls", "ls", "List informations about FILEs"},
{ SH_LS_EXT2, "ls2", "ls2", "List information about ext2 FILEs"},
{ SH_FRAMES, "lsframe", "lsframe", "Dump free frames"},
{ SH_PAGES, "lspage", "lspage", "Dump (only) the dirty pages"},
{ SH_PCISCAN, "lspci", "lspci", "Scan for PCI devices"},
{ SH_MOUNT, "mount", "mount", "Mount the FAT12 filesystem on the floppy"},
{ SH_UMOUNT, "umount", "umount", "Umount the FAT12 filesystem on the floppy"},
{ SH_PS, "ps", "ps", "Print the state of every processes"},
{ SH_PWD, "pwd", "pwd", "Print the current working directory"},
{ SH_READ, "read", "read <block>", "Read a block from the floppy to the internal buffer"},
{ SH_REBOOT, "reboot", "reboot", "Reboot the system"},
{ SH_RM, "rm", "rm <filename>", "Remove a file"},
{ SH_SYNC, "sync", "sync", "Flush filesystem buffers"},
{ SH_UNAME, "uname", "uname", "Print kernel informations"},
{ SH_WRITE, "write", "write <block>", "Write a block from the internal buffer to the floppy"},
};

/** @} */ // end of UIMiniShell

// Floppy buffer //
uint8_t fd_buffer[ FDC_SECTOR_SIZE ];

// **************** Luca Giovacchini (Ide) ****************
void Sh_IdeRead()
{
	int Dev;
	int Lba;
	int Err=FALSE;
	char *DataBuffer = fd_buffer;

	kprintf("\n\rInsert the channel number (0 or 1): 0\b");
	scanf("%i", &Dev);
	kprintf("\n\rChannel %i Selected",Dev);
	if ( Dev==0 )
		SelectAtaChannel(CC_PRIMARY);
	else if ( Dev==1 )
		SelectAtaChannel(CC_SECONDARY);
	else
	{
		kprintf("\n\rBad Channel Number: Channel 0 will be selected");
		SelectAtaChannel(CC_PRIMARY);
	}

	kprintf("\n\rInsert the device number to Read (0 or 1): 0\b");
	scanf("%i", &Dev);
	kprintf("\n\rDevice %i Selected",Dev);
	if ( Dev==0 || Dev==1 )
	{

		kprintf("\n\rInsert Lba : ");
		scanf("%i", &Lba);
		kprintf("\n\rLba Selected %i",Lba);

		Err = ReadSectorsLba( Dev, (int64)Lba, 1, (word *)DataBuffer, TRUE );
		if (Err)
			ShowIdeErrorMessage(Err,TRUE);
		else
		{
			kprintf("\n\r HEX DATA");
			ShowMatrixData(DataBuffer,1,16,20,512,DBG_SHOWMAT_HEX);
			WaitKeyPress(MSG_PRESSKEY);
			kprintf("\n\r CHAR DATA");
			ShowMatrixData(DataBuffer,1,40,20,512,DBG_SHOWMAT_ASCII);
			WaitKeyPress(MSG_PRESSKEY);
		}
	}
	else
		kprintf("\n\rBad Device Number");
	kprintf("\n\r ");
}

void Sh_IdeWrite()
{
	int Dev;
	int Lba;
	char *DataBuffer = fd_buffer;
	int Err=FALSE;

	kprintf("\n\rInsert the channel number (0 or 1): 0\b");
	scanf("%i", &Dev);
	kprintf("\n\rChannel %i Selected",Dev);
	if ( Dev==0 )
	{
		SelectAtaChannel(CC_PRIMARY);
	}
	else if ( Dev==1 )
	{
		SelectAtaChannel(CC_SECONDARY);
	}
	else
	{
		kprintf("\n\rBad Channel Number Channel 0 will be selected");
		SelectAtaChannel(CC_PRIMARY);
	}

	kprintf("\n\rInsert the device number to Write (0 or 1): 0\b");
	scanf("%i", &Dev);
	kprintf("\n\rDevice %i Selected",Dev);
	if ( Dev==0 || Dev==1 )
	{

		kprintf("\n\rInsert Lba : ");
		scanf("%i", &Lba);
		kprintf("\n\rLba Selected %i",Lba);

		Err = WriteSectorsLba( Dev,(int64)Lba,1,(word *)DataBuffer,TRUE);
		if (Err)
			ShowIdeErrorMessage(Err,TRUE);
	}
	else
		kprintf("\n\rBad Device Number");
	kprintf("\n\r ");

}

void Sh_IdeDeviceInfo()
{
	int Dev;

	kprintf("\n\rInsert the channel number (0 or 1): 0\b");
	scanf("%i", &Dev);
	kprintf("\n\rChannel %i Selected",Dev);
	if ( Dev==0 )
		SelectAtaChannel(CC_PRIMARY);
	else if ( Dev==1 )
		SelectAtaChannel(CC_SECONDARY);
	else
	{
		kprintf("\n\rBad Channel Number Channel 0 will be selected");
		SelectAtaChannel(CC_PRIMARY);
	}

	kprintf("\n\rInsert the device number (0 or 1): 0\b");
	scanf("%i", &Dev);
	kprintf("\n\rDevice %i Selected",Dev);
	if ( Dev==0 || Dev==1 )
	{
		ShowDeviceData(Dev,30);
	}
}
// ********************************************************

// ******************** Filippo Brogi *********************

//! Change the current ext2 directory.
void sh_cd2(char *argv)
{
	if (argv[0]=='\0') return;

	cd_ext2(argv);
}

//! Concatenate ext2 file to standard output.
void sh_cat2(char *argv)
{
	if (argv[0]=='\0')
	{
		kprintf("\n\rcat2 requires argument\n\r");
		return;
	}
	cat_ext2(argv);
}
// ********************************************************

//! Read 1 sector from the floppy disk to the FD buffer.
void sh_read( char *argv )
{
	int block, i;

	block = atoi( argv );
	kprintf( "Reading block %u...\n", block );

	memset( fd_buffer, 0, sizeof(fd_buffer) );

	if( (i = fdc_read(block, fd_buffer, sizeof(fd_buffer) / FDC_SECTOR_SIZE)) < 0 )
	{
		kprintf( "MiniShell: unable to read the block!\n" );
		return;
	}

	for( i = 0; i < sizeof(fd_buffer); i++ )
	{
		kputchar( fd_buffer[i] );
	}

	kputchar('\n');
}

//! Write 1 sector from the FD buffer to the floppy disk.
void sh_write( char *argv )
{
	int block;

	block = atoi(argv);

	kprintf( "Writing block %u...\n", block );

	if( fdc_write(block, fd_buffer, sizeof(fd_buffer) / FDC_SECTOR_SIZE) < 0 )
	{
		kprintf( "MiniShell: unable to write the block!!!\n" );
		return;
	}
	kprintf( "done.\n" );
}

//! Kill a task.
void sh_kill(char *argv)
{
	int pid = atoi( argv );
	task_t *p;

	sched_enter_critical_region();

	// Get the task informations.
	p = get_task_info( pid );

	sched_leave_critical_region();

	if( p == NULL )
	{
		kprintf( "MiniShell: task %i unknown.\n", pid );
		return;
	}
	kprintf( "MiniShell: killing task [%s] (%i)...  ", p->name, p->pid );

	if( p->privilege == USER_PRIVILEGE )
	{
		if( kill(atoi(argv)) )
		{
			kprintf( "ok!\n" );
			return;
		}
	}
	kprintf( "cannot kill this task!\n" );
}

// --- Virtual 8086 mode ---------------------------------------------- //

#define V86_START_ADDRESS	0x10000

//! Create a v86 foreground task.
void sh_v86_exec( int dir, char *argv )
{
	byte *file_buffer;
	int file_size;
	task_t *child = NULL;
	int status;

	if( !argv )
	{
		kprintf( "file name?\n" );
		return;
	}

	// Get the file size if exists.
	file_size = fat12_file_size( dir, argv );
	if( file_size > 0 )
	{
		int ans;

		// Create the task.
		file_buffer = kmalloc( file_size, GFP_KERNEL );
		fat12_load_file( file_buffer, file_size, dir, argv );
		// Print a warning message.
		kprintf(
			"--- Warning! ---\n"
			"Attempt to execute plain binary file (not ELF): %s\n"
			"This operation could be very dangerous for the system...\n"
			"Do you really want to execute this file? (y/n) ",
			argv );
		ans = tolower( getchar() );
		kprintf( "%c\n", ans );
		if( ans == 'y' )
		{
			child = create_v86_process( (void *)V86_START_ADDRESS,
				file_buffer, file_size, argv );
		}
		else
		{
			kprintf( "MiniShell: %s execution aborted!\n", argv );
		}
		kfree( file_buffer );

		// Wait until the child is killed.
		if( child != NULL )
			waitpid( child->pid, &status, 0 );
	}
	else
	{
		if( file_size < 0 )
			kprintf( "MiniShell: file %s not found.\n", argv );
	}
}

//! Create a v86 background task.
void sh_v86_bg( int dir, char *argv )
{
	byte *file_buffer;
	int file_size;

	if( !argv )
	{
		kprintf( "file name?\n" );
		return;
	}

	// Get the file size if exists.
	file_size = fat12_file_size( dir, argv );
	if( file_size > 0 )
	{
		int ans;

		// Create the task.
		file_buffer = kmalloc( file_size, GFP_KERNEL );
		fat12_load_file( file_buffer, file_size, dir, argv );
		// Print a warning message.
		kprintf(
			"--- Warning! ---\n"
			"Attempt to execute a binary file: %s\n"
			"This operation could be very dangerous for the system...\n"
			"Do you really want to execute this file? (y/n) ",
			argv );
		ans = tolower( getchar() );
		kprintf( "%c\n", ans );
		if( ans == 'y' )
		{
			(void)create_v86_process( (void *)V86_START_ADDRESS,
				file_buffer, file_size, argv );
		}
		else
		{
			kprintf( "\nMiniShell: %s execution aborted!\n", argv );
		}
		kfree( file_buffer );
	}
	else
	{
		if( file_size < 0 )
			kprintf( "MiniShell: file %s not found.\n", argv );
	}
}
// --- End of Virtual 8086 mode --------------------------------------- //

//! Maximum number of parameters available from the command line.
#define SH_MAX_PARAMS		256

//! \brief Parse parameters passed to the task by the command line.
void sh_parse_params( char *cmd_str, char ***argv, int *argc )
{
	char *tmp, *par;
	int i;
	char *str_ptr[ SH_MAX_PARAMS ];

	// Initialize argv and argc.
	*argv = (char **)NULL;
	*argc = 0;

	// Empty command line?!
	if ( cmd_str==NULL )
		return;
	if ( *cmd_str == '\0' )
		return;

	// Convert the string.
	tmp = cmd_str;
	for( i = 0; ; )
	{
		par = strsep( &tmp, " " );
		if( par == NULL )
			break;
		if( par[ 0 ] == '\n' )
			break;
		if( par[ 0 ] == '\0' )
			continue;
		str_ptr[ i++ ] = par;
	}
	*argc = MIN( _countof(str_ptr), i );

	// Create the argv string array.
	*argv = (char **)kmalloc( (*argc) * sizeof(char **), GFP_KERNEL );
	for( i = 0; i < *argc; i++ )
	{
		(*argv)[ i ] = str_ptr[ i ];
	}
}

//! \brief Run a program in foreground.
void sh_exec( int dir, char *cmd_line )
{
	task_t *child;
	int status;
	int u_argc;
	char **u_argv;

	sh_parse_params( cmd_line, &u_argv, &u_argc );
	if( u_argc < 1 )
	{
		kprintf( "file name?\n" );
		return;
	}

	// Execute the file.
	child = exec_file( dir, u_argv[ 0 ], u_argc, u_argv );
	if ( child == NULL )
	{
		// An error occurs. Treat this file as a
		// simple binary 8086 code.
		sh_v86_exec( dir, u_argv[ 0 ] );
		goto out;
	}

	// Wait until the child is killed.
	waitpid( child->pid, &status, 0 );
out:
	// Free the unused memory.
	kfree( u_argv );
}

//! \brief Run a program in background.
void sh_bg( int dir, char *cmd_line )
{
	task_t *child;
	int u_argc;
	char **u_argv;

	sh_parse_params( cmd_line, &u_argv, &u_argc );
	if( !u_argv )
	{
		kprintf( "file name?\n" );
		return;
	}

	// Execute the file.
	child = exec_file( dir, u_argv[0], u_argc, u_argv );
	if ( child == NULL )
	{
		// An error occurs. Treat this file as a
		// simple binary 8086 code.
		sh_v86_bg( dir, u_argv[0] );
		goto out;
	}
out:
	// Free the unused memory.
	kfree( u_argv );
}

//! Print memory informations.
void sh_free( void )
{
	extern size_t PHYS_MEM_DIM;
	size_t free_mem = get_free_frames() * PAGE_SIZE;
	size_t dma_free_mem = dma_get_free_mem();

	// Header.
	set_color( WHITE );
	kprintf( "         total          used          free\n" );
	set_color( DEFAULT_COLOR );

	// System memory.
	kprintf( "sys:  %7uK      %7uK      %7uK\n",
		(PHYS_MEM_DIM - DMA_MEMORY_DIM) / 1024,
		(PHYS_MEM_DIM - DMA_MEMORY_DIM - free_mem) / 1024,
		free_mem / 1024 );

	// DMA memory.
	kprintf( "dma:  %7uK      %7uK      %7uK\n",
		DMA_MEMORY_DIM / 1024,
		(DMA_MEMORY_DIM - dma_free_mem) / 1024,
		dma_free_mem / 1024 );
}

//! Concatenate a FILE to the standard output.
void sh_cat( int dir, char *argv )
{
	uint8_t *filebuffer;
	int filesize, i;

	// Locate the file.
	filesize = fat12_file_size( dir, argv );
	if ( filesize < 0 )
	{
		kprintf( "MiniShell: no such file or directory.\n" );
		return;
	}
	if( filesize == 0 )
	{
		// Empty file or root.
		return;
	}

	// Load the file into the buffer.
	filebuffer = malloc( filesize );
	if( filebuffer == NULL )
	{
		kprintf( "MiniShell: out of heap memory!\n" );
		return;
	}
	if( fat12_load_file( filebuffer, filesize, dir, argv ) < 0 )
	{
		free( filebuffer );
		kprintf( "MiniShell: no such file or directory.\n" );
		return;
	}

	// Print the file to stdout.
	for( i = 0; i < filesize; i++ )
	{
		kputchar( filebuffer[ i ] );
	}
	free( filebuffer );
}

// Remove a file.
void sh_rm( int dir, char *argv )
{
	int c;

	if( *argv == '\0' )
	{
		kprintf( "file name?\n" );
		return;
	}

	kprintf( "MiniShell: remove file %s (y/n)? ", argv );
	c = tolower( getchar() );
	kprintf( "%c\n", c );
	if( c == 'y' )
	{
		if ( fat12_rm(dir, argv) < 0 )
		{
			kprintf( "MiniShell: no such file or directory.\n" );
		}
		else
		{
			kprintf( "MiniShell: file %s deleted.\n", argv );
		}
	}
}

//! Print the help.
void sh_help(char *argv)
{
	int i, row;

	kprintf("\n%s\n", SH_VERSION);
	if( *argv != NULL )
	{
		// Print only the selected commands.
		for( i = 0; i < SH_COMMANDS; i++ )
		if( strcmp(argv, commands[i].name) == 0 )
		{
			// Print usage and command info.
			kprintf("\n-> ");
			set_color(WHITE);
			kprintf("%s", commands[i].usage);
			set_color(DEFAULT_COLOR);
			kprintf("\n   %s;\n\n", commands[i].help);
			return;
		}
		// Command not found!
		kprintf("\n   Command not found!\n\n");
		return;
	}

	// Dump the standard help.
	kprintf("\n"
		"ALT+Fn -> switch between virtual consoles.\n\n"
		"To execute a file type its name and press RETURN.\n"
		"To execute a file in background add the '&' at the end of the command line.\n"
		"Press a key to see the detailed help list... (CTRL+C to break)\n" );
	if( kgetchar() == CTRL_C )
		return;

	// Print all commands.
	for( i = 0, row = 0; i < SH_COMMANDS; i++ )
	{
		// Print usage and command info.
		kprintf("\n-> ");
		set_color(WHITE);
		kprintf("%s", commands[i].usage);
		set_color(DEFAULT_COLOR);
		kprintf("\n   %s;", commands[i].help);

		// Wait for keypressed to avoid screen scrolling.
		row += 2;
		if( row >= 24 )
		{
			if( kgetchar() == CTRL_C )
				break;
			row = 0;
		}
	}
	kprintf("\n\n");
}

//! This task prints the process list.
void ps_task( int argc, char **argv )
{
	ps();
	_exit(0);
}

//! Print the process list.
void sh_ps()
{
	pid_t pid;
	int status;

	pid = create_thread( &ps_task, 0, NULL, "ps" )->pid;
	waitpid( pid, &status, 0 );
}

//! Halt the system.
void sh_halt()
{
	poweroff();
}

//! Check if the file is a valid ELF32.
void sh_elf( int dir, char *argv )
{
	byte *file_buffer;
	int file_size;

	if( !argv )
	{
		kprintf( "file name?\n" );
		return;
	}

	// Get the file size if exists
	file_size = fat12_file_size( dir, argv );
	if( file_size < 0 )
	{
		kprintf( "MiniShell: file %s not found.\n", argv );
		return;
	}
	// Load the ELF32 file.
	file_buffer = malloc( file_size );
	if( elf32_load_file(file_buffer, file_size, dir, argv) == NULL )
	{
		kprintf( "MiniShell: %s is not a valid elf32 file.\n", argv );
	}
	else
	{
		kprintf( "MiniShell: %s elf32 file-format ok.\n", argv );
	}
	free(file_buffer);
}

void sh_ls( int dir, char *args )
{
	int num = fat12_ls( dir, args );

	if( num < 0 )
	{
		kprintf( "MiniShell: no such file or directory.\n" );
	}
	else
	{
		kprintf( "%u file%c\n", num, (num == 1) ? ' ' : 's' );
	}
}

//! Go up from the current directory.
char *up_dir_path( char *path )
{
	int i = strlen( path );

	if( i < 2 )
		return( path );

	if( path[ --i ] == '/' )
		i--;
	while( path[ i ] != '/' )
		i--;

	if( i )
		path[ i ] = '\0';
	else
		strcpy( path, "/" );

	return( path );
}

//! Add a path to the current position.
char *add_dir_path( char *path, char *new_path, int size )
{
	char *tmp = new_path;
	char *ret = tmp;
	int i;

	if( *tmp == '\0' )
		return( ret );

	// Generate the new path string.
	if( strcmp(tmp, ".") == 0 )
	{
		// Do nothing.
		return( ret );
	}
	else if( strcmp(tmp, "..") == 0 )
	{
		// Level up.
		ret = up_dir_path( path );
		return( ret );
	}
	else if( *tmp == '/' )
	{
		// Absolute path.
		// Skip every initial '/'s.
		while( *tmp == '/' )
			tmp++;
		ret = strncpy( path, --tmp, size );
	}
	else
	{
		ret = strncat( path, tmp, size );
		ret = strncat( path, "/", size );
	}

	// Skip the '/'s in the trailer.
	i = strnlen( path, size );
	if( i-- > 1 )
	{
		while( (path[ i ] == '/') && i )
			path[ i-- ] = '\0';
		path[ i + 1 ] = '/';
	}

	return( ret );
}

//! Change directory.
void sh_cd( int *dir, char *path, int size, char *cmd_line )
{
	int u_argc;
	char **u_argv;
	int ret;

	// If all goes well the u_argv array will be allocated in the
	// kernel space.
	sh_parse_params( cmd_line, &u_argv, &u_argc );
	if( !u_argc )
	{
		return;
	}

	ret = fat12_cd( *dir, u_argv[ 0 ] );
	if( ret < 0 )
	{
		kprintf( "MiniShell: no such file or directory.\n" );
	}
	else
	{
		*dir = ret;
		add_dir_path( path, u_argv[ 0 ], size );
	}

	// Free the unused memory.
	kfree( u_argv );
}

//! Syncronize filesystem buffers.
int sh_sync( void )
{
	fat12_sync();
	fdc_sync();

	return( 0 );
}

//! Mount the filesystem on the floppy disk and initialize
//! the root directory and the path.
int sh_mount( char *path, int *dir )
{
	int ret;

	// Initialize the directory.
	ret = fat12_mount();
	if( ret == -EPERM )
	{
		// Filesystem already mounted!
		kprintf( "MiniShell: filesystem already mounted.\n" );
		return( ret );
	}
	// Initialize the path to the root directory.
	strcpy( path, "/" );
	// Update the current directory.
	*dir = ret;

	return( ret );
}

//! Umount the fat12 filesystem on the floppy disk.
void sh_umount( char *path, int *dir )
{
	if( fat12_umount() < 0 )
	{
		kprintf( "MiniShell: cannot umount the filesystem.\n" );
	}
	strcpy( path, "/" );
	*dir = -ENOENT;
}

//! The shell prompt.
#define SH_PROMPT	"[minirighi:(fd0)%s]# "

// The MINIShell main program.
void shell( int argc, char **argv )
{
	// Shell command buffer.
	char cmd[ STR_MAX_LENGTH ];
	char path[ 1024 ];
	char *args;
	int dir = FAT12_ROOT_DIR;
	int i;

	// Reset the command buffer and the path.
	memset( cmd, '\0', sizeof(cmd) );

	// Initialize the path.
	strncpy( path, "/", sizeof(path) );

	// Welcome message.
	kprintf( "\nWelcome to the MiniShell (tty%u)",
		get_curr_task()->console );
	kprintf( "\nType 'help' for more informations.\n"
		 "Have a lot of fun...\n" );

	// Command line.
	while(TRUE)
	{
		// Print the prompt.
		kprintf( SH_PROMPT, path );

		// Wait for a command.
		getline( cmd, sizeof(cmd) );

		// Skip initial spaces
		args = cmd;
		while( *args == ' ' )
			args++;

		// Empty command.
		if( *args == '\0' )
			continue;

		// Parse the command.
		for( i = 0; i < SH_COMMANDS; i++ )
		{
			if( strncmp(args, commands[i].name, strlen(commands[i].name)) == 0 )
			{
				char *par = ( args + strlen(commands[ i ].name) );
				if( (*par == '\0') || (*par == ' ') )
					break;
			}
		}
		if( i == SH_COMMANDS )
		{
			// Look for the '&' character.
			char *amp = args + strlen( args );

			while( (*(--amp) == ' ') )
				if( amp == args)
					break;
			if( *amp == '&' )
			{
				*amp = ' ';
				sh_bg( dir, args );
				*amp = '&';
			}
			else
			{
				sh_exec( dir, args );
			}
			continue;
		}

		// Get the parameter string.
		args += strlen(commands[i].name);
		while( *args == ' ' )
			args++;

		// Execute the command.
		switch( commands[i].index )
		{
			case SH_HELP:
				sh_help( args );
			break;

			case SH_UNAME:
				kprintf("%s - Kernel [v%s]\n",
					KERNEL_NAME, KERNEL_VERSION );
			break;

			case SH_CLEAR:
				set_color( DEFAULT_COLOR );
				clrscr();
			break;

			case SH_REBOOT:
				reboot();
			break;

			case SH_DUMP:
				breakpoint();
			break;

			case SH_READ:
				sh_read( args );
			break;

			case SH_WRITE:
				sh_write( args );
			break;

			case SH_PAGES:
				dump_dirty_pages();
			break;

			case SH_MEM:
				dump_memory_map();
			break;

			case SH_FRAMES:
				dump_free_frames();
			break;

			case SH_CHECKMEM:
				create_thread(
					&check_free_frames_integrity,
					0, NULL,
					"frames_check" );
			break;

			case SH_EXEC:
				sh_exec( dir, args );
			break;

			case SH_BG:
				sh_bg( dir, args );
			break;

			case SH_FREE:
				sh_free();
			break;

			case SH_CPUID:
				show_cpuinfo();
			break;

			case SH_SYNC:
				sh_sync();
			break;

			case SH_MOUNT:
				sh_mount( path, &dir );
			break;

			case SH_UMOUNT:
				sh_umount( path, &dir );
			break;

			case SH_CD:
				sh_cd( &dir, path, sizeof(path), args );
			break;

			case SH_PWD:
				kprintf( "%s\n", path );;
			break;

			case SH_LS:
				sh_ls( dir, args );
			break;

			case SH_CAT:
				sh_cat( dir, args );
			break;

			case SH_RM:
				sh_rm( dir, args );
			break;

			case SH_PS:
				sh_ps();
			break;

			case SH_KILL:
				sh_kill( args );
			break;

			case SH_RS232:
				rs232_chat( args );
			break;

			case SH_PCISCAN:
				pci_scan();
			break;

			case SH_RTL8139_INIT:
				ifconfig( args );
			break;

			case SH_ETH_ARP:
				arp_request( args );
			break;

			case SH_HALT:
				sh_halt();
			break;

			case SH_ELF:
				sh_elf( dir, args );
			break;

			case SH_IDEREAD:
				Sh_IdeRead();
			break;

			case SH_IDEWRITE:
				Sh_IdeWrite();
			break;

			case SH_IDEDEVICEINFO:
				Sh_IdeDeviceInfo();
			break;

			case SH_LS_EXT2:
				ls_ext2();
			break;

			case SH_CD_EXT2:
				sh_cd2( args );
			break;

			case SH_CAT_EXT2:
				sh_cat2( args );
			break;
		}
	}
}
