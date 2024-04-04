
#ifndef	_CMOS_
#define	_CMOS_

#include	"config.h"

#include	"io.h"
#include	"cpu.h"
#include	"time.h"
#include	"mem.h"
#include	"kmath.h"

#define	CMOS_SEC					0x00
#define	CMOS_ALARM_SEC			0x01
#define	CMOS_MIN					0x02
#define	CMOS_ALARM_MINUTE		0x03
#define	CMOS_ORE					0x04
#define	CMOS_ALARM_ORE			0x05
#define	CMOS_GIORNOSETTIMANA		0x06		//(1=Domenica)
#define	CMOS_GIORNOMESE			0x07
#define	CMOS_MESE					0x08
#define	CMOS_ANNO_LOW			0x09

#define	CMOS_STATUS_A				0x0A
#define	CMOS_STATUS_B				0x0B
#define	CMOS_STATUS_C				0x0C
#define	CMOS_STATUS_D				0x0D

#define	CMOS_POST_DIAGNOSTIC		0x0E
#define	CMOS_SHUTDOWN_STATUS	0x0F


#define	CMOS_DRIVE_FLOPPY			0x10
#define	CMOS_DRIVE_HD				0x12
#define	CMOS_PERIFERICHE			0x14

#define	CMOS_MEM_BASE_LOW		0x15
#define	CMOS_MEM_BASE_HIGH		0x16

#define	CMOS_MEM_EXT_LOW			0x17
#define	CMOS_MEM_EXT_HIGH		0x18

#define	CMOS_HD0_TYPE				0x19
#define	CMOS_HD1_TYPE				0x1A

#define	CMOS_CHECKSUM_LOW		0x2E
#define	CMOS_CHECKSUM_HIGH		0x2F

#define	CMOS_MEM_ACTEXT_LOW		0x30
#define	CMOS_MEM_ACTEXT_HIGH		0x31

#define	CMOS_SECOLO				0x31

#define	CMOS_IBM128k				0x33		//In BCD


struct cmos_dati_t
	{
	struct time_infocmos_t  timeinfo;		//Ora (e Data)
	struct mem_infocmos_t meminfo;		//Memoria
	};

void			cmos_getdati(struct cmos_dati_t* dati);	//Riempie la struttura con i valori della mem CMOS
unsigned char	cmos_read(unsigned int reg);			//Primitiva di accesso alla mem CMOS
//void			cmos_write(unsigned int reg,unsigned char val);

#endif
