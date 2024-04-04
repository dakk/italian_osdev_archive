#ifndef _DMA_H_
#define  _DMA_H_


	/* Just helps in making things look cleaner.  :) */
typedef unsigned char uchar;
typedef unsigned int uint;


void dma_xfer(uchar channel, unsigned long address, unsigned int length,
	      unsigned char read);
		  
#endif
