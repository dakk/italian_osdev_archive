
#include	"cmos.h"


void	cmos_getdati(struct cmos_dati_t* dati)
	{
	unsigned char stato;

	unsigned char ore,min,sec;

	unsigned char	highmem, lowmem;
	mem_t memext,membase;


	//----Clock Info
	sec=cmos_read(CMOS_SEC);
	min=cmos_read(CMOS_MIN);
	ore=cmos_read(CMOS_ORE);

	dati->timeinfo.data.sec=BCD2DEC(sec);
	dati->timeinfo.data.min=BCD2DEC(min);
	dati->timeinfo.data.ore=BCD2DEC(ore);

	stato=cmos_read(CMOS_POST_DIAGNOSTIC) & 0x4;
	dati->timeinfo.invalid=stato;

	//----Mem Info
	lowmem=cmos_read(CMOS_MEM_BASE_LOW);
	highmem=cmos_read(CMOS_MEM_BASE_HIGH);

	membase=highmem;
	membase=membase<<8;
	membase+=lowmem;

	lowmem=cmos_read(CMOS_MEM_ACTEXT_LOW);
	highmem=cmos_read(CMOS_MEM_ACTEXT_HIGH);

	memext=highmem;
	memext=memext<<8;
	memext+=lowmem;

	dati->meminfo.base=membase*1024;
	dati->meminfo.ext=memext*1024;

	stato=cmos_read(CMOS_POST_DIAGNOSTIC) & 0x10;
	dati->meminfo.compare_error=stato;

	return;
	}

unsigned char	cmos_read(unsigned int reg)
	{
	unsigned char val;

	outportb(0x70,reg);
	val=inportb(0x71);

	return val;
	}

/*
void	cmos_write(unsigned int reg,unsigned char val)
	{
	outportb(0x70,reg);
	outportb(0x71,val);
	}
*/
