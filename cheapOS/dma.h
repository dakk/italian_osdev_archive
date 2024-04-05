
#ifndef	_DMA_
#define	_DMA_

#include	"config.h"

#include	"io.h"
#include	"mem.h"


#define	DMAREAD	0
#define	DMAWRITE	1

//Struttura che identifica i canali DMA
struct dma_channel_t
	{
	unsigned char	pagina;
	unsigned char	offset;
	unsigned char	size;
	};

bool dma_xfer(unsigned char channel,mem_t bufferbase,mem_t buffersize,bool read);	//Trasferimento DMA da a un dispositivo alla memoria


#endif

