
#ifndef	_FLOPPY_
#define	_FLOPPY_


#include	"config.h"

#include	"isr.h"
#include	"time.h"
#include	"io.h"
#include	"dma.h"
#include	"scheduler.h"
#include	"syscall_task.h"
#include	"mem.h"
#include	"semaphoro.h"
#include	"cache.h"

#define	FLOPPY_CACHESECTOR_NUM	128		//64K

#define	FLOPPY_GEOMETRY	0	//1,44MB

#define	FLOPPY_SECTOR_SIZE	512

// FDC motor spin up (ticks)
#define	FLOPPY_TIME_MOTOR_SPINUP	 50

// FDC timeout to turn the motor off (ticks)
#define	FLOPPY_TIME_MOTOR_OFF	300

//porte	I/O
#define	FLOPPY_DOR	0x3F2	// Digital Output Register
#define	FLOPPY_MSR	0x3F4	// Main Status Register (read)
#define	FLOPPY_DSR	0x3F4	// Data Rate Select Register (write)
#define	FLOPPY_DATA	0x3F5	// Data Register
#define	FLOPPY_DIR	0x3F7	// Digital Input Register
#define	FLOPPY_CCR	0x3F7	// Configuration Control Register

// Commandi
#define	FLOPPY_CMD_SPECIFY		0x03
#define	FLOPPY_CMD_WRITE		0xC5
#define	FLOPPY_CMD_READ		0xE6
#define	FLOPPY_CMD_RECAL		0x07
#define	FLOPPY_CMD_SENSEI		0x08
#define	FLOPPY_CMD_FORMAT		0x4D
#define	FLOPPY_CMD_SEEK		0x0F
#define	FLOPPY_CMD_VERSION	0x10

//Flags
#define	MSR_BUSY	0x10

//Geometria
struct floppy_type_t
	{
	unsigned long	size; 	// settori totali
	unsigned long 	spt;		// settori per traccia
	unsigned long 	heads;	// testine
	unsigned long 	tracks;	// tracce
	unsigned char	fmtgap;	// gap3 Format
	unsigned char	rwgap;	// gap3 ReadWrite
	unsigned char	rate;		// data rate
	};


bool floppy_init();												//Inizzializza il floppy

bool floppy_read(int block,mem_t buffer,unsigned long count);				//Legge 'count' blocchi a partire da 'block' e li copia in buffer
bool floppy_write(int block,mem_t buffer,unsigned long count);			//Scrive 'count' blocchi a partire da 'block' prendendoli da  buffer

bool floppy_changed();											//Il disco e' cambiato (quando non doveva)

void floppy_lba2chs(int lba,unsigned char *track,int *head,int *sector);		//Da Lba a Chs
bool floppy_rw(int block,mem_t buffer,bool read);						//Primitiva di lettura/scrittura

void floppy_sendb(unsigned char b);								//Manda byte al controller (polling)
unsigned char floppy_getb();										//Prende byte dal controller(polling)

void floppy_reset();												//Reset del disco
void floppy_recalibrate();											//Ricalibra il disco

bool floppy_seek(unsigned char track);								//Seeka

void floppy_motoron();											//Motore Acceso
void floppy_motoroff();											//Motore spento (tra TOT ticks)

void floppy_thread();											//TimeJob del floppy
bool fdc_wait(bool sensei);										//Wait (usa kwait!!!)

#endif
