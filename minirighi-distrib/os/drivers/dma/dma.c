/*!	\file drivers/dma/dma.c
 *	\brief DMA (Direct Memory Access) driver.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-08
 *
 *	This driver is based on the Fabian Nunez DMA driver.
 *	The author can be reached by email at: <fabian@cs.uct.ac.za>.
 */

#include <const.h>
#include <string.h>

#include <arch/i386.h>
#include <arch/mem.h>
#include <arch/paging.h>
#include <kernel/debug.h>
#include <kernel/kernel_map.h>
#include <kernel/kmalloc.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>

#include <kernel/dma.h>

//! The end of the DMA memory area. In a normal system take
//! the lower 16M of memory, but if the system has less than
//! 16M assign a different value to it.
//! \warning Do not assign a value lower than 2M!
size_t dma_memory_end = 0x01000000;

//! Evaluate a reasonable value for the upper bound of the
//! DMA memory area if the system has a little amount of
//! physical memory.
void __INIT__ dma_setup_memory_end( size_t phys_mem )
{
	switch( phys_mem )
	{
		// From 15M to 23M of extended physical memory.
		case (15*1024*1024) ... (23*1024*1024):
			dma_memory_end = 8*1024*1024;
		break;

		// From 7M to 15M of extended physical memory.
		case (7*1024*1024) ... (15*1024*1024 - 1):
			dma_memory_end = 4*1024*1024;
		break;

		// From 3M to 7M of extended physical memory.
		case (3*1024*1024) ... (7*1024*1024 - 1):
			dma_memory_end = 2*1024*1024;
		break;

		// Less than 3M of extended memory is not enough...
		case 0 ... (3*1024*1024 - 1):
			printk( KERN_EMERG
				"%s(): %uK of extended memory are not enough for Minirighi!\n",
				__FUNCTION__,
				phys_mem / 1024 );
			printk( KERN_EMERG
				"System halted!\n" );
			halt();
		break;

		default:
			// Ok leave the default value.
		break;
	}
	printk( KERN_INFO
		"DMA: reserved memory area [%p]->[%p]\n",
			DMA_MEMORY_START, DMA_MEMORY_END );
}

// --- DMA transfers -------------------------------------------------- //

/** \ingroup Drivers
 *  \defgroup DMADriver DMA (Direct Memory Access) driver
 *  The DMA (Direct Memory Access) low level driver
 *  (based on i386-architecture).
 *  @{
 */

//! Definition of DMA channels.
static dma_channel_t dmainfo[] =
{
	{ 0x87, 0x00, 0x01 },
	{ 0x83, 0x02, 0x03 },
	{ 0x81, 0x04, 0x05 },
	{ 0x82, 0x06, 0x07 }
};

//! Semaphore for mutual exclusion use of the DMA controller.
DECLARE_MUTEX(dma_controller_mutex);

//! Lock the DMA controller for mutual exclusion.
//! Use this to begin a DMA operation.
void lock_dma_controller() {
	DOWN(&dma_controller_mutex);
}

//! Unlock the DMA controller for mutual exclusion.
//! Use this at the end of a DMA operation.
void unlock_dma_controller() {
	UP(&dma_controller_mutex);
}

//! \brief Set up a DMA transfer between a device and memory.
//! \param channel The 8-bit channel number (0..3).
//! \param physaddr The physical address of the buffer.
//! \param length The size of the buffer.
//! \param read
//!	If it is #TRUE the transfer will be from memory to device,
//!	otherwise will be from device to memory.
void dma_xfer(unsigned channel, addr_t physaddr, size_t length, bool read)
{
	int page, offset;
	uint32_t flags;

	if( channel > 3 )
		return;

	// Calculate dma page and offset.
	page = physaddr >> 16;
	offset = physaddr & 0xFFFF;
	length -= 1;  // with dma, if you want k bytes, you ask for k-1.

	local_irq_save( flags );

	// Set the mask bit for the channel.
	outb( 0x0A, channel | 0x04 );

	// Clear flipflop.
	outb( 0x0C, 0x00 );

	// Set DMA mode (write+single+r/w).
	outb( 0x0B, (read ? 0x48 : 0x44) + channel );

	// Set DMA page.
	outb( dmainfo[channel].page, page );

	// Set DMA offset.
	outb( dmainfo[channel].offset, offset & 0xFF );  // low byte //
	outb( dmainfo[channel].offset, offset >> 8 );    // high byte //

	// Set DMA length.
	outb( dmainfo[channel].length, length & 0xFF );  // low byte //
	outb( dmainfo[channel].length, length >> 8 );    // high byte //

	// Clear DMA mask bit.
	outb( 0x0A, channel );

	local_irq_restore( flags );
}

/** @} */ // end of DMADriver

// --- DMA memory allocator ------------------------------------------- //

/** \ingroup Memory
 *  \defgroup KDMAAllocator The DMA (Direct Memory Access) Allocator
 *  The allocator for the DMA memory area.
 *  @{
 */

//! DMA free frames vector. It maps every frame into the DMA memory
//! area. On i386 we have to reserve the first 16MB of physical memory
//! to the DMA, because on the bus we have a 24-bit register dedicated
//! to the addressing (2^24=16MB).
/** \todo
 *	Yes I know! A vector is ugly... a bitmap should be much better.
 *	It requests some bit twiddling, but we can gain a lot of memory!
 *	Unfortunately for now I've not much time to spent here! :-(
 */
byte *dma_free_frames;

//! A semaphore for the mutual exclusion of dma free frames vector.
DECLARE_MUTEX( dma_mutex );

//! \brief Initialize the DMA memory allocator.
void __INIT__ dma_alloc_init()
{
	extern size_t *K_VIR_END;
	size_t i;

	// Initialize the DMA free frames vector.
	dma_free_frames = kmalloc( DMA_MEMORY_DIM / PAGE_SIZE, GFP_KERNEL );
	for( i = 0; i < DMA_MEMORY_DIM / PAGE_SIZE; i++ )
		dma_free_frames[ i ] = DMA_FREE_FRAME;

	// Set the video buffer memory busy.
	for( i = DMA_FRAME_ADDR(VIDEO_BUF_START) / PAGE_SIZE;
		i < DMA_FRAME_ADDR(VIDEO_BUF_END) / PAGE_SIZE; i++ )
		dma_free_frames[ i ] = DMA_BUSY_FRAME;

	// Set the BIOS ROM memory busy.
	for( i = DMA_FRAME_ADDR(BIOS_ROM_START) / PAGE_SIZE;
		i < (DMA_FRAME_ADDR(BIOS_ROM_END) / PAGE_SIZE); i++ )
		dma_free_frames[ i ] = DMA_BUSY_FRAME;

	// Set the kernel physical space busy.
	for( i = 0; i <= (size_t)K_VIR_END - K_VIR_START; i += PAGE_SIZE )
		dma_free_frames[ (DMA_FRAME_ADDR(K_PHYS_ADDR) / PAGE_SIZE) + i ] = DMA_BUSY_FRAME;
}

//! \brief
//!	Return a pointer of \p len contiguous bytes into the
//!	DMA address space.
//! \param len The size of the buffer we want to allocate.
//! \return
//!	A pointer to the allocated buffer.
//! \note
//!	The pointer is the <b>physical address</b> of the buffer
//!	and not the <b>virtual address</b>!!!
//! \exception
//!	NULL Out of DMA memory area!
void *dma_phys_alloc( size_t len )
{
	size_t i, j, ret;
	int flag = 0;

	// len must be not null!
	if( !len )
		return( NULL );

	// Round up len to frames boundary.
	len = PAGE_ALIGN_UP( len );

	DOWN( &dma_mutex );

	// Search for a free dma page.
	i = 0;
	do
	{
		if( dma_free_frames[ i ] == DMA_FREE_FRAME )
		{
			flag = 0;
			// Found a free frame!
			// OK, now I require (len/PAGE_SIZE-1)
			// others free frames.
			for( j = 1; j < (len / PAGE_SIZE); j++ )
			{
				if( dma_free_frames[ i + j ] == DMA_BUSY_FRAME )
				{
					// Nooo! Found a busy frame!
					flag = 1;
					break;
				}
			}
			if( flag == 1 )
			{
				// Not a valid contiguous free space.
				// Continue after j position...
				i += j;
				continue;
			}
			else
			{
				// Found a valid contiguous free
				// space, so mark it as busy.
				for( j = 0; j < len / PAGE_SIZE; j++ )
				{
					dma_free_frames[ i + j ] = DMA_BUSY_FRAME;
				}
				break;
			}
		}
	} while( ++i < DMA_MEMORY_DIM / PAGE_SIZE );

	UP( &dma_mutex );

	// Check if we have found the free DMA location.
	if( flag == 0 )
	{
		// We found it.
		ret = (i * PAGE_SIZE + DMA_MEMORY_START);
	}
	else
	{
		// Not found!
		ret = NULL;
	}

	return( (void *)ret );
}

//! \brief
//!	Free the DMA memory area from \p dma_start to \p dma_start+len
//!	address.
//! \param dma_start The beginning of the area to free.
//! \param len The size of the buffer to free.
//! \return
//!	\li #TRUE on success;
//!	\li #FALSE if an error occurs.
//! \note
//!	The \p dma_start address is the <b>physical address</b>
//!	of the buffer to free and not the <b>virtual address</b>!!!
void dma_phys_free( size_t dma_start, size_t len )
{
	int i;

	if( !len )
		return;

	// Round up len to frames boundary.
	len = PAGE_ALIGN_UP( len );

	if( (dma_start >= DMA_MEMORY_START) && (dma_start < DMA_MEMORY_END) )
	{
		i = DMA_FRAME_ADDR( dma_start ) / PAGE_SIZE;

		DOWN( &dma_mutex );

		// Mark the DMA region as free.
		while(	i <  DMA_FRAME_ADDR( dma_start + len ) / PAGE_SIZE )
		{
			dma_free_frames[ i++ ] = DMA_FREE_FRAME;
			if( i >= DMA_FRAME_ADDR( DMA_MEMORY_END ) / PAGE_SIZE )
				break;
		}

		UP( &dma_mutex );
	}
}

//! Return the free dma memory in bytes.
size_t dma_get_free_mem()
{
	int ret = 0;
	int i = 0;

	DOWN( &dma_mutex );

	while( i < (DMA_MEMORY_DIM / PAGE_SIZE) )
	{
		if( dma_free_frames[ i++ ] == DMA_FREE_FRAME )
			ret++;
	}

	UP( &dma_mutex );

	return( ret * PAGE_SIZE );
}

/** @} */ // end of KDMAAllocator
