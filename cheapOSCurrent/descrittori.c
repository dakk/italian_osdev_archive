
#include 	"descrittori.h"


void desc_setstnd(union DT_entry* descrittore,unsigned long base,unsigned long limit,unsigned short control)
	{
	descrittore->desc.limit_low	=limit & 0xffff;
	descrittore->desc.base_low	=base & 0xffff;
	descrittore->desc.base_med	=(base>>16) & 0xff;
	descrittore->desc.access		=(control+D_PRESENT)>>8;
	descrittore->desc.limit_high	=limit>>16;
	descrittore->desc.granularity	=(control&0xff)>>4;
	descrittore->desc.base_high	=base>>24;
	}

void	desc_setgate(union DT_entry* descrittore,unsigned char segment,unsigned long offset,unsigned short control)
	{
	descrittore->gate.offset_low	=offset & 0xffff;
	descrittore->gate.selector	=segment;
	descrittore->gate.access		=control;
	descrittore->gate.offset_high	=offset>>16;
	}

unsigned long desc_getstnd_base(union DT_entry* descrittore)
	{
	unsigned int low,med,high,base;

	low=descrittore->desc.base_low & 0xffff;
	med=descrittore->desc.base_med & 0xff;
	high=descrittore->desc.base_high;

	base=low+(med<<16)+(high<<24);

	return base;
	}

unsigned long desc_getstnd_limit(union DT_entry* descrittore)
	{
	unsigned int low,high,limit;

	low=descrittore->desc.limit_low & 0xffff;
	high=descrittore->desc.limit_high;

	limit=low+(high<<16);

	return limit;
	}
