/*!	\file drivers/floppy/floppy.c
 *	\brief Floppy disk driver.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-04-23 Andrea Righi:\n
 *			Added the floppy cache for recently used blocks.
 *			Improved low-level routines for read and write
 *			operations.\n
 *		2004-04-17 Andrea Righi:\n
 *			Improved performances adding a wait queue
 *			for tasks that are waiting for the floppy
 *			disk controller operations.
 *		2004-04-11 Andrea Righi:\n
 *			Added mutual exclusion for the floppy
 *			controller.
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#include <const.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <arch/clock.h>
#include <arch/i386.h>
#include <arch/interrupt.h>
#include <arch/mem.h>
#include <arch/paging.h>
#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/dma.h>
#include <kernel/queue.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>

#include <kernel/floppy.h>

/** \ingroup Drivers
 *  \defgroup FloppyDriver Floppy Disk
 *  The floppy disk driver.
 *  @{
 */

//! Motor on flag.
volatile bool fdc_motor = FALSE;

//! FDC timeout.
volatile int fdc_timeout = 0;

//! Floppy motor kill countdown.
volatile int fdc_motor_countdown = 0;

//! FDC operation finish.
volatile bool fdc_done = FALSE;

//! FDC status (result output).
volatile byte fdc_status[ 7 ] = { 0 };

//! Current head position.
volatile byte fdc_track = 0xff;

//! Status register 0.
volatile byte ST0 = 0;

//! Floppy types known from the system.
static floppy_struct_t floppy_type[] = {
// 1,44 MB.
{ 2880, 18, 2, 80, 0x54, 0x1b, 0x00, "H1440" },
// 1.68 MB. (TODO)
{ 3360, 21, 2, 80, 0x0c, 0x1c, 0x00, "H1680" },
};

//! Current floppy geometry.
byte fdc_geometry = 0;

//! Floppy buffer for low level read / write operations.
uint8_t *fdc_buffer = NULL;

//! A semaphore for the mutual exclusion of the floppy disk.
DECLARE_MUTEX( fdc_mutex );

//! Queue for the task that are waiting for the floppy disk controller.
DECLARE_WAITQUEUE( fdc_wait_queue );

//! A queue to store recently used floppy blocks.
DECLARE_QUEUE( floppy_cache );

//! Current number of blocks in the floppy cache.
int floppy_block_count = 0;

// --- Floppy Cache Operators ----------------------------------------- //

//! Get a block into the cache if it is present.
static floppy_cache_block_t *floppy_cache_get_block( int num )
{
	floppy_cache_block_t *buf;
	queue_t *entry;
	int n;

	// Search the block into the cache.
	queue_for_each( entry, n, floppy_cache )
	{
		buf = queue_get_entry( entry );
		if( buf->num == num )
		{
			// Block present: update the timestamp.
			buf->timestamp = sys_get_ticks();
			return( buf );
		}
	}

	// Block not present.
	return( NULL );
}

//! Delete the last recently used block into the cache.
static void floppy_cache_delete_lru()
{
	floppy_cache_block_t *buf, *old_buf = NULL;
	queue_t *entry, *old_entry = NULL;
	int n;
	clock_t min_stamp = sys_get_ticks() + 1;

	// Search the block into the cache.
	queue_for_each( entry, n, floppy_cache )
	{
		buf = queue_get_entry( entry );
		if( buf->timestamp < min_stamp )
		{
			// This is the last recently used block,
			// until now...
			old_entry = entry;
			old_buf = buf;
			min_stamp = buf->timestamp;
		}
	}

	// Free the LRU block.
	if( old_entry != NULL )
	{
#ifdef FLOPPY_DEBUG
		printk( KERN_DEBUG "free lru block %i.\n", old_buf->num );
#endif
		queue_del_entry( &floppy_cache, old_entry );
		kfree( old_buf );
	}
}

//! Add a floppy disk block into the cache.
static void floppy_cache_push_block( int num, uint8_t *block )
{
	floppy_cache_block_t *fb;

	fb = kmalloc( sizeof(floppy_cache_block_t), GFP_KERNEL );
	if( fb == NULL )
	{
		printk( KERN_ERR
			"%s(): out of kernel memory!\n",
			__FUNCTION__ );
		return;
	}

	// Store the sector number.
	fb->num = num;
	// Dirty flag off.
	fb->dirty = 0;
	// Get the timestamp.
	fb->timestamp = sys_get_ticks();
	// Copy the block into the buffer.
	memcpy( fb->data, block, sizeof(fb->data) );

	// Update the block counter.
	if( floppy_block_count < FLOPPY_CACHE_MAX_BLOCKS )
	{
		// Increment the block counter.
		floppy_block_count++;
	}
	else
	{
		// Delete the last recently used block.
		floppy_cache_delete_lru();
	}

	// Insert the block into the queue.
	add_queue( &floppy_cache, fb );
}

//! Delete all blocks into the floppy cache and free the memory space.
static void floppy_cache_clear( void )
{
	queue_t *entry, *dummy;
	floppy_cache_block_t *fb;
	int n;

	// Clear the floppy blocks cache.
	queue_for_each_safe( entry, dummy, n, floppy_cache )
	{
		fb = queue_get_entry( entry );
		queue_del_entry( &floppy_cache, entry );
		kfree( fb );
	}
#ifdef FLOPPY_DEBUG
	printk( KERN_DEBUG "%u blocks freed.\n", floppy_block_count );
#endif
	// Reset the block counter.
	floppy_block_count = 0;
}

// --- Floppy Generic Functions --------------------------------------- //

//! \brief Wait until the floppy controller becomes ready.
static int fdc_wait_until_ready( void )
{
	int counter, status;

	for( counter = 0; counter < 10000; counter++ )
	{
		status = inb_p( FDC_MSR );
		if( status & MSR_READY )
		{
			return( status );
		}
	}
	return( -1 );
}

//! \brief Send a byte to the FDC controller.
//! \param b The byte to send.
static int fdc_sendbyte( uint8_t b )
{
	int msr;

	msr = fdc_wait_until_ready();
	if( msr < 0 )
	{
		return( -1 );
	}
	if( (msr & (MSR_READY | MSR_DIR | MSR_DMA)) == MSR_READY )
	{
		outb_p( FDC_DATA, b );
		return( 0 );
	}
	return( -1 );
}

//! \brief Get a byte from the FDC controller.
//! \return The byte read from the controller.
//! \exception NULL If timeout is elapsed.
static int fdc_getbyte()
{
	int msr;

	msr = fdc_wait_until_ready();
	if( msr < 0 )
	{
		return( -1 );
	}
	msr &= MSR_DIR | MSR_READY | MSR_BUSY | MSR_DMA;
	if( msr == (MSR_DIR | MSR_READY | MSR_BUSY) )
	{
		return( inb_p(FDC_DATA) );
	}
	return( -1 );
}

//! \brief Turn the floppy motor on.
static void fdc_motor_on( void )
{
	if( !fdc_motor )
	{
		outb( FDC_DOR, 0x1C );
		delay( FDC_TIME_MOTOR_SPINUP );
		fdc_motor = TRUE;
	}
	fdc_motor_countdown = -1;
}

//! \brief Starts the FDC motor kill countdown.
static void fdc_motor_off( void )
{
	if( fdc_motor && (fdc_motor_countdown == -1) )
	{
		fdc_motor_countdown = FDC_TIME_MOTOR_OFF / 1000 * HZ;
	}
}

//! \brief Check if the floppy disk was changed.
static bool __fdc_is_changed( void )
{
	if( inb(FDC_DIR) & 0x80 )
	{
		// Invalidate the floppy cache.
		floppy_cache_clear();

		return( TRUE );
	}

	return( FALSE );
}

//! \brief Wait for a FDC operation.
//! \param sensei
//!	\li #TRUE if a "sense interrupt status" command is required;
//!	\li #FALSE for a simple timeout wait.
//! \return
//!	\li #TRUE if the operation has successfully done;
//!	\li #FALSE if the timeout has been elapsed.
/**
 *	This routine must be invoked every time that we have to verify
 *	the correct completion of an operation.
 *	If a "sense interrupt status" command is issued, the status of
 *	the controller is reported into the #ST0 variable and the
 *	current track is stored into the #fdc_track variable.
 */
static bool fdc_wait( bool sensei )
{
	byte i;
	uint32_t flags;
	int ret;

	local_irq_save( flags );

	fdc_timeout = 3 * HZ;

	// Wait for IRQ or timeout.
	for(;;)
	{
		if( (fdc_done) || (fdc_timeout == 0) )
		{
			break;
		}
		wait_event( fdc_wait_queue, fdc_done || (fdc_timeout == 0) );
	}

	// Read command result bytes.
	i = 0;
	while( (i < 7) && (inb(FDC_MSR) & MSR_BUSY) )
	{
		fdc_status[ i++ ] = fdc_getbyte();
	}

	// Send a "sense interrupt status" command.
	if( sensei )
	{
		fdc_sendbyte( CMD_SENSEI );
		ST0 = fdc_getbyte();
		fdc_track = fdc_getbyte();
	}

	// Timed out?
	if( fdc_done == FALSE )
	{
		// Check for disk change.
		ret = FALSE;
	}
	else
	{
		fdc_done = FALSE;
		ret = TRUE;
	}

	local_irq_restore( flags );

	return( ret );
}

//! \brief Recalibrate the floppy drive.
static void fdc_recalibrate()
{
	// Turn the motor on.
	fdc_motor_on();

	// Send recalibrate command.
	fdc_sendbyte( CMD_RECAL );
	fdc_sendbyte( 0 );

	// Wait until recalibrate command is finished.
	fdc_wait( TRUE );

	// Turn the motor off.
	fdc_motor_off();
}

//! \brief Seek a track.
//! \param track The track to seek.
//! \return
//!	\li 0 success;
//!	\li -ENODEV the floppy device is not installed;
//!	\li -EINVAL the track is not valid;
//!	\li -ETIME timeout;
//!	\li -ESPIPE invalid seek.
static int __fdc_seek( int track )
{
	if( fdc_buffer == NULL )
	{
		// Floppy is not installed.
		return( -ENODEV );
	}

	// If already there return.
	if( fdc_track == track )
	{
		return( 0 );
	}

	// Turn the motor on.
	fdc_motor_on();

	// Send seek command.
	fdc_sendbyte( CMD_SEEK );
	fdc_sendbyte( 0 );
	fdc_sendbyte( track );

	// Wait until seek is finished.
	if( fdc_wait(TRUE) == FALSE )
	{
		// Timeout!
		fdc_motor_off();
		return( -ETIME );
	}

	// Let the head settle for 15msec.
	delay( 15 );

	// Turn off the motor.
	fdc_motor_off();

	// Check if seek worked.
	if( (ST0 == 0x20) && (fdc_track == track) )
	{
		return( 0 );
	}
	return( -ESPIPE );
}

//! \brief Seek a sector (mutual exclusion guaranteed).
int fdc_seek( int block )
{
	int ret;

	// Evaluate the track.
	block = ( block /
		floppy_type[ fdc_geometry ].spt *
		floppy_type[ fdc_geometry ].heads ) + 1;

	DOWN( &fdc_mutex );

	// Run the seek operation in mutual exclusion.
	ret = __fdc_seek( block );

	UP( &fdc_mutex );
	return( ret );
}

//! \brief Reset the floppy disk controller.
static void fdc_reset()
{
	// Stop the motor and disable IRQ.
	outb( FDC_DOR, 0x00 );

	// Program data rate (500K/s).
	outb( FDC_DSR, 0x00 );

	// Re-enable floppy IRQ.
	outb( FDC_DOR, 0x0c );

	// Wait for the controller.
	fdc_wait( TRUE );

	// Specify drive timings.
	fdc_sendbyte( CMD_SPECIFY );
	// SRT = 3ms, HUT = 240ms
	fdc_sendbyte( 0xdf );
	// HLT = 4ms, ND = 0 (DMA mode selected)
	fdc_sendbyte( 0x02 );

	// Clear the floppy cache.
	floppy_cache_clear();

	// Seek the first track.
	__fdc_seek( 1 );
	fdc_recalibrate();
}

//! \brief
//!	Convert from a LBA (Linear Block Address) address to a
//!	CHS (Cylinder, Head, Sector) coordinates.
//! \param lba The LBA address.
//! \param track A pointer to the track coordinate.
//! \param head A pointer to the head coordinate.
//! \param sector A pointer to the sector coordinate.
static void lba2chs(int lba, int *track, int *head, int *sector)
{
	*track = lba / (floppy_type[fdc_geometry].spt * floppy_type[fdc_geometry].heads);
	*head = (lba / floppy_type[fdc_geometry].spt) % floppy_type[fdc_geometry].heads;
	*sector = (lba % floppy_type[fdc_geometry].spt) + 1;
}

//! \brief
//!	Perform a read/write operation with the floppy drive using
//!	the DMA (Direct Memory Access).
//! \param block The LBA address of the block on the floppy.
//! \param buffer
//!	The address of the buffer in memory for the transfer
//!	operation. It must have a size of at least #FDC_SECTOR_SIZE
//!	bytes.
//! \param do_read
//!	\li #TRUE to perform a read operation (from disk to memory);
//!	\li #FALSE to perform a write operation (from memory to disk).
//! \return
//!	\li 0 success;
//!	\li -ENOMEDIUM the disk is changed;
//!	\li -ESPIPE the seek operation fails;
//!	\li -ETIME timeout error;
//!	\li -EIO i/o error.
static int fdc_rw( int block, uint8_t *buffer, int nsec, bool do_read )
{
	int track, head, sector, tries;

	// Translate the logical address into CHS address.
	lba2chs( block, &track, &head, &sector );

	if( !do_read )
	{
		// Write operation => copy data from buffer into fdc_buffer.
		memcpy( fdc_buffer, buffer, nsec * FDC_SECTOR_SIZE );
	}

	for( tries = 0; tries < 3; tries++ )
	{
		// Check for disk changed.
		if( __fdc_is_changed() )
		{
			floppy_cache_clear();
			__fdc_seek( 1 );
			fdc_recalibrate();
			return( -ENOMEDIUM );
		}

		// Spin up the motor.
		fdc_motor_on();

		// Move the head to the right track.
		if( __fdc_seek(track) < 0 )
		{
			fdc_motor_off();
			return( -ESPIPE );
		}
		// Program data rate.
		outb( FDC_CCR, floppy_type[fdc_geometry].rate );

		lock_dma_controller();

		// Send command.
		if( do_read )
		{
			// Read operation //
			dma_xfer( 2, VIRTUAL((size_t)fdc_buffer), nsec * FDC_SECTOR_SIZE, FALSE );
			fdc_sendbyte( CMD_READ );
		}
		else
		{
			// Write operation //
			dma_xfer( 2, VIRTUAL((size_t)fdc_buffer), nsec * FDC_SECTOR_SIZE, TRUE );
			fdc_sendbyte( CMD_WRITE );
		}
		fdc_sendbyte( head << 2 );
		fdc_sendbyte( track );
		fdc_sendbyte( head );
		fdc_sendbyte( sector );
		// 512 bytes/sector.
		fdc_sendbyte( 2 );
		fdc_sendbyte( floppy_type[fdc_geometry].spt );
		fdc_sendbyte( floppy_type[fdc_geometry].rwgap );
		// DTL = unused.
		fdc_sendbyte( 0xff );

		// Wait for command completion.
		if( fdc_wait(FALSE) == FALSE )
		{
			unlock_dma_controller();

			// Timed out!
			fdc_motor_off();
			return( -ETIME );
		}

		unlock_dma_controller();

		if( (fdc_status[0] & 0xc0) == 0 )
		{
			// Worked.
			break;
		}

		// Try again...
		fdc_recalibrate();
	}

	// Stop the motor
	fdc_motor_off();

	if( do_read )
	{
		// Read operation => copy data from fdc_buffer
		// into the buffer.
		memcpy( buffer, fdc_buffer, nsec * FDC_SECTOR_SIZE );
	}

	// Return if the R/W operation terminates successfully.
	return( (tries == 3) ? -EIO : 0 );
}

//! \brief Physical read some contiguous blocks from the floppy disk.
//! \param block The starting block to read.
//! \param buffer
//!	The destination buffer in memory where the
//!	blocks will be copied.
//! \param count
//!	How many blocks to read.
//! \return
//!	\li 0 success;
//!	\li -ENODEV device not installed;
//!	\li -ENOMEDIUM disk changed;
//!	\li -ENXIO seek error;
//!	\li -EIO i/o error.
int __fdc_phys_read( int block, uint8_t *buf, size_t count )
{
	int track, track2, head, head2, sector, sector2;
	int upper, i;
	floppy_cache_block_t *tmp = NULL;
	uint8_t *ptr = buf;

	int first = block;
	int last = block + count - 1;

	int ret = 0;

	lba2chs( first, &track, &head, &sector );
	lba2chs( last, &track2, &head2, &sector2 );

	while( first <= last )
	{
		// Read more than one block for each time.
		if( (track == track2) && (head == head2) )
		{
			upper = sector2;
		}
		else
		{
			upper = floppy_type[ fdc_geometry ].spt;
		}

		// Perform the read operation.
		ret = fdc_rw( first, ptr, (upper - sector + 1), TRUE );
		if( ret < 0 )
		{
			// I/O error.
			goto out;
		}

		// Push the blocks into the cache.
		for( i = 0; i < (upper - sector + 1); i++, ptr += FDC_SECTOR_SIZE )
		{
			tmp = floppy_cache_get_block( first + i );
			if( tmp == NULL )
			{
				// Add the block into the cache.
				floppy_cache_push_block( first + i, ptr );
			}
			else
			{
				// Update the timestamp only.
				tmp->timestamp = sys_get_ticks();
			}
		}
		// Update the first sector position.
		first = first + i;
		lba2chs( first, &track, &head, &sector );
	}
out:
	return( ret );
}

//! \brief Read some contiguous blocks from the floppy disk.
//! \param block The starting block to read.
//! \param buffer
//!	The destination buffer in memory where the
//!	blocks will be copied.
//! \param count
//!	How many blocks to read.
//! \return
//!	\li 0 success;
//!	\li -ENODEV device not installed;
//!	\li -ENOMEDIUM disk changed;
//!	\li -ENXIO seek error;
//!	\li -EIO i/o error.
int __fdc_read( int block, uint8_t *buffer, size_t count )
{
	floppy_cache_block_t *tmp = NULL;
	uint8_t *ptr;
	int i;
	int ret = 0;

	if( fdc_buffer == NULL )
	{
		// Floppy is not installed.
		return( -ENODEV );
	}

	for( i = 0, ptr = buffer; i < count; i++, ptr += FDC_SECTOR_SIZE )
	{
		// Try to get the block from the cache.
		tmp = floppy_cache_get_block( block + i );
		if( tmp == NULL )
		{
			ret = __fdc_phys_read( block + i, ptr, count - i );
			goto out;
		}
		else
		{
			// Block already present into the cache.
			memcpy( ptr, tmp->data, FDC_SECTOR_SIZE );
			// Update the timestamp.
			tmp->timestamp = sys_get_ticks();
		}
	}
out:
	// Read operation done.
	return( ret );
}

//! \brief Global interface for the floppy read operation.
int fdc_read( int block, uint8_t *buffer, size_t count )
{
	int ret;

	DOWN( &fdc_mutex );

	// Perform the write operation.
	ret = __fdc_read( block, buffer, count );

	UP( &fdc_mutex );

	return( ret );
}

//! \brief Write some contiguous blocks to the floppy disk.
//! \param block The starting block to write.
//! \param buffer
//!	The address in memory from which the data will be copied.
//! \param count
//!	How many blocks to write.
//! \return
//!	\li 0 success;
//!	\li -ENODEV device not installed;
//!	\li -ENOMEDIUM disk changed;
//!	\li -ENXIO seek error;
//!	\li -EIO i/o error.
static int __fdc_write( int block, uint8_t *buffer, size_t count )
{
	floppy_cache_block_t *tmp = NULL;
	uint8_t *ptr;
	int i;
	int ret = 0;

	if( fdc_buffer == NULL )
	{
		// Floppy is not installed.
		return( -ENODEV );
	}

	for( i = 0; i < count; i++ )
	{
		// Update the pointer of the buffer.
		ptr = buffer + ( FDC_SECTOR_SIZE * i );

		// Try to get the block from the cache.
		tmp = floppy_cache_get_block( block + i );
		if( tmp == NULL )
		{
			// Write the block to the device.
			ret = fdc_rw( block + i, ptr, 1, FALSE );
			if( ret < 0 )
			{
				// I/O error.
				goto out;
			}
		}
		else
		{
			// Block already present: write it into the
			// cache.
			memcpy( tmp->data, ptr, FDC_SECTOR_SIZE );
			// Set the dirty flag for this block.
			tmp->dirty = 1;
			// Update the timestamp.
			tmp->timestamp = sys_get_ticks();
		}
	}
out:
	// Write operation done.
	return( ret );
}
//! \brief Global interface for the floppy write operation.
int fdc_write( int block, uint8_t *buffer, size_t count )
{
	int ret;

	DOWN( &fdc_mutex );

	// Perform the write operation.
	ret = __fdc_write( block, buffer, count );

	UP( &fdc_mutex );

	return( ret );
}

//! Syncronize the floppy cache with the disk.
static int __fdc_sync( void )
{
	queue_t *entry;
	floppy_cache_block_t *fb;
	int n;
	int ret;

	queue_for_each( entry, n, floppy_cache )
	{
		fb = queue_get_entry( entry );
		if( fb->dirty == 1 )
		{
			// Write the block to the device.
			ret = fdc_rw( fb->num, fb->data, 1, FALSE );
			if( ret < 0 )
			{
				// I/O error.
				goto out;
			}
		}
	}
	ret = 0;
out:
	// Clear the floppy cache.
	floppy_cache_clear();

	// Seek the first track.
	__fdc_seek( 1 );

	// Recalibrate the floppy.
	fdc_recalibrate();

	return( ret );
}

//! Interface for the syncronize operation.
int fdc_sync( void )
{
	DOWN( &fdc_mutex );

	// Perform the sync operation in mutual exclusion.
	(void)__fdc_sync();

	UP( &fdc_mutex );

	return( 0 );
}

//! \brief Check if the floppy disk was changed.
//! \return
//!	\li #TRUE if the disk was changed;
//!	\li #FALSE otherwise.
bool fdc_is_changed()
{
	register bool ret;

	DOWN( &fdc_mutex );

	ret = __fdc_is_changed();

	UP( &fdc_mutex );

	return( ret );
}

// --- Interrupt routines --------------------------------------------- //

//! \brief
//!	This is a routine called from clock_handler() at every clock
//!	tick to perform the floppy motor kill countdown and to control
//!	the floppy timeouts.
void floppy_thread()
{
	if( fdc_timeout > 0 )
	{
		if( (--fdc_timeout) == 0 )
		{
			wakeup_queue( fdc_wait_queue );
		}
	}

	if( fdc_motor_countdown > 0 )
	{
		fdc_motor_countdown--;
	}
	else if( fdc_motor && !fdc_motor_countdown )
	{
		outb( FDC_DOR, 0x0C );
		fdc_motor = FALSE;
	}
}

/** \ingroup Handlers */
//! \brief
//!	This is the floppy interrupt handler routine. It is invoked
//!	every time that a floppy operation successfully completes.
void floppy_handler( irq_context_t *c )
{
	fdc_done = TRUE;
	wakeup_queue( fdc_wait_queue );
}

//! \brief Uninstall the floppy driver.
void fdc_remove()
{
	DOWN( &fdc_mutex );

	// Delete the floppy cache.
	floppy_cache_clear();

	// Uninstall the interrupt handler.
	uninstall_handler( FLOPPY_IRQ );

	// Free the floppy buffer.
	dma_phys_free( VIRTUAL((size_t)fdc_buffer), FDC_BUFFER_SIZE );
	fdc_buffer = NULL;

	UP( &fdc_mutex );
}

// --- Initialization --------------------------------------------------//

//! \brief Initialize the floppy driver.
//! \return
//!	\li #TRUE on success;
//!	\li #FALSE if an error occurs.
bool init_floppy()
{
	int v, ret;

	DOWN( &fdc_mutex );

	// Install the interrupt handler routine.
	install_trap_handler( FLOPPY_IRQ, (void *)floppy_handler );

	// Reset the controller.
	fdc_reset();

	// Get floppy controller version.
	fdc_sendbyte( CMD_VERSION );
	v = fdc_getbyte();

	UP( &fdc_mutex );

	switch ( v )
	{
		case 0xFF:
			printk( KERN_ERR
				"%s(): floppy controller not found!\n",
				__FUNCTION__ );
			ret = FALSE;
		break;

		case 0x80:
			printk( KERN_INFO
				"%s(): NEC controller.\n",
				__FUNCTION__ );
			ret = TRUE;
		break;

		case 0x81:
			printk( KERN_INFO
				"%s(): VMware controller.\n",
				__FUNCTION__ );
			ret = TRUE;
		break;

		case 0x90:
			printk( KERN_INFO
				"%s(): enhanced controller.\n",
				__FUNCTION__ );
			ret = TRUE;
		break;

		default:
			printk( KERN_INFO
				"%s(): unknown controller [%#04x].\n",
				__FUNCTION__, v );
			ret = TRUE;
		break;
	}

	if( ret == TRUE )
	{
		DOWN( &fdc_mutex );

		// Create the floppy buffer for low-level read/write
		// operations.
		fdc_buffer = PHYSICAL( dma_phys_alloc(FDC_BUFFER_SIZE) );
		if( fdc_buffer == NULL )
		{
			UP( &fdc_mutex );

			printk( KERN_ERR
				"%s(): out of dma memory!\n",
				__FUNCTION__ );

			return( FALSE );
		}

		UP( &fdc_mutex );

		printk( KERN_INFO
			"%s(): floppy buffer at [%p]->[%p]\n",
			__FUNCTION__,
			fdc_buffer, (size_t)fdc_buffer + PAGE_SIZE );
	}

	return( ret );
}

/** @} */ // end of FloppyDriver
