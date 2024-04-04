/*!	\file include/kernel/floppy.h
 *	\brief Floppy disk driver header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-04-23\n
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef FLOPPY_H
#define FLOPPY_H

#include <types.h>
#include <arch/interrupt.h>

// --- Drive geometry for 1.44MB floppies ----------------------------- //

#define FDC_SECTORS		18	//!< Sectors per track.
#define FDC_HEADS		2	//!< Heads per track.
#define FDC_TRACKS		80	//!< Tracks per disk.
#define FDC_SECTOR_SIZE		512	//!< Bytes per sector.

//! FDC motor spin-up time (msec).
#define FDC_TIME_MOTOR_SPINUP	500

//! FDC timeout to turn the motor off (msec).
#define FDC_TIME_MOTOR_OFF	3000

// --- Floppy I/O ports ----------------------------------------------- //

#define FDC_DOR		0x3F2 //!< Digital Output Register.
#define FDC_MSR		0x3F4 //!< Main Status Register (read).
#define FDC_DSR		0x3F4 //!< Data Rate Select Register (write).
#define FDC_DATA	0x3F5 //!< Data Register.
#define FDC_DIR		0x3F7 //!< Digital Input Register.
#define FDC_CCR		0x3F7 //!< Configuration Control Register.

// --- Floppy Commands ------------------------------------------------ //

#define CMD_SPECIFY	0x03	//!< Specify.
#define CMD_WRITE	0xC5	//!< Write.
#define CMD_READ	0xE6	//!< Read.
#define CMD_RECAL	0x07	//!< Recalibrate.
#define CMD_SENSEI	0x08	//!< Send a "sense interrupt status".
#define CMD_FORMAT	0x4D	//!< Format a track.
#define CMD_SEEK	0x0F	//!< Seek a track.
#define CMD_VERSION	0x10	//!< Get the controller version.

// --- Register values ------------------------------------------------ //

//! The controller is busy.
#define MSR_BUSY	0x10
//! 0- DMA mode.
#define MSR_DMA		0x20
//! 0- cpu -> fdc.
#define MSR_DIR		0x40
//! Data register ready.
#define MSR_READY	0x80

// --- Upper level informations --------------------------------------- //

//! Size of the floppy buffer for low-level read/write operations.
#define FDC_BUFFER_SIZE \
	( PAGE_ALIGN_UP(floppy_type[ fdc_geometry ].spt * FDC_SECTOR_SIZE) )

//! Maximum number of blocks that can be stored into the floppy cache.
#define FLOPPY_CACHE_MAX_BLOCKS		512

//! Floppy geometry structures.
typedef struct floppy_struct
{
	size_t	size, 	//!< # of sectors total.
		spt,	//!< sectors per track.
		heads,	//!< # of heads.
		tracks;	//!< # of tracks.
	uint8_t	fmtgap,	//!< gap3 while formatting.
		rwgap,	//!< gap3 while reading/writing.
		rate;	//!< data rate.
	char	*name;	//!< format name.
} floppy_struct_t;

//! A floppy block into the cache.
typedef struct floppy_cache_block
{
	//! The sector number.
	int num;
	//! Dirty flag is set if the block need to be written to device.
	int dirty;
	//! A timestamp to implement LRU replacement policy.
	clock_t timestamp;
	//! Block data.
	uint8_t data[ FDC_SECTOR_SIZE ];
} floppy_cache_block_t;

// --- Prototypes ----------------------------------------------------- //

void floppy_thread();
bool init_floppy();
bool fdc_is_changed();

int fdc_seek(int block);
int fdc_read(int block, uint8_t *buffer, size_t count);
int fdc_write(int block, uint8_t *buffer, size_t count);
int fdc_sync( void );

#endif
