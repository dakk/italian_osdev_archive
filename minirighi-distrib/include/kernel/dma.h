/*!	\file include/kernel/dma.h
 *	\brief DMA (Direct Memory Access) driver header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-08
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef DMA_H
#define DMA_H

//! The size of the DMA memory area.
#define DMA_MEMORY_DIM (DMA_MEMORY_END - DMA_MEMORY_START)

//! The DMA-memory frame is marked as free.
#define DMA_FREE_FRAME	0

//! The DMA-memory frame is marked as busy.
#define DMA_BUSY_FRAME	1

//! Normalize the physical address to the dma base address.
#define DMA_FRAME_ADDR( i ) \
	(((i) < DMA_MEMORY_START) ? 0 : ((size_t)(i) - DMA_MEMORY_START))

//! A structure used to store hardware definition of DMA channels.
typedef struct dma_channel
{
	uint8_t page;     //!< Page register.
	uint8_t offset;   //!< Offset register.
	uint8_t length;   //!< Length register.
} dma_channel_t;

extern size_t dma_memory_end;

// --- Prototypes ----------------------------------------------------- //

__INLINE__ void lock_dma_controller();
__INLINE__ void unlock_dma_controller();

void dma_xfer(unsigned channel, addr_t physaddr, size_t length, bool read);
void *dma_phys_alloc(size_t len);
void dma_phys_free(size_t dma_start, size_t len);
size_t dma_get_free_mem();

void dma_setup_memory_end( size_t phys_mem );
void dma_alloc_init();

#endif

