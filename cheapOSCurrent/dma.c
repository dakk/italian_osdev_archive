
#include	"dma.h"

struct dma_channel_t dma_channel[]=
	{
	{0x87,0x00,0x01},
	{0x83,0x02,0x03},
	{0x81,0x04,0x05},
	{0x82,0x06,0x07},
	};

bool dma_xfer(unsigned char channel,mem_t bufferbase,mem_t buffersize,bool read)
	{
	unsigned int pagina,offset;

	if(channel>3) return	FALSE;

	//kprint("dma bufferbase  0x%8x\n",bufferbase);

	offset  =(bufferbase & 0xFFFF);
	pagina=bufferbase >> 16;
	buffersize-=1;

	//kprint("dma canale %3d\n",channel);

	outportb(0x0A,channel | 0x04);					//Seleziono il canale
	outportb(0x0C,0x00);							//FlipFlop a Zero

	//kprint("dma %s\n",read?"READ":"WRITE");
	outportb(0x0B,(read?0x48:0x44)+channel);			//Lettura o Scrittura??

	//kprint("dma pagina 0x%8x\n",pagina);
	outportb(dma_channel[channel].pagina,pagina);		//Pagina

	//kprint("dma offset 0x%8x\n",offset);
	outportb(dma_channel[channel].offset,offset & 0xFF);	//OffSet
	outportb(dma_channel[channel].offset,offset	>>8	 );

	//kprint("dma size 0x%8x\n",buffersize);
	outportb(dma_channel[channel].size,buffersize	& 0xFF);		//size
	outportb(dma_channel[channel].size,buffersize	>>	8 );

	outportb(0x0A,channel);							//Deseleziono il canale

	return TRUE;
	}

