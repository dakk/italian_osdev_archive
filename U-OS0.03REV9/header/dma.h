#ifndef __DMA_H_
#define __DMA_H_

#include <mydef.h>

#define DMA_MASTER 0
#define DMA_SLAVE 1

#define DMA_MODE_S 0x0B
#define DMA_MODE_M 0xD6

#define CLEAR_BYTE_P_S 0x0C
#define CLEAR_BYTE_P_M 0xD8

#define SINGLE_MASK_REGISTER_S 0x0A
#define SINGLE_MASK_REGISTER_M 0xD4

#define MODE_DEMAND 0x00
#define MODE_SINGLE 0x40
#define MODE_BLOCK 0x80
#define MODE_CASCADE 0xC0

#define MODE_INC 0x00
#define MODE_DEC 0x20

#define MODE_SC 0x00
#define MODE_AUTO 0x10

#define MODE_VERIFY 0x00
#define MODE_WRITE 0x04
#define MODE_READ 0x08

#define MODE_CH0 0x00
#define MODE_CH1 0x01
#define MODE_CH2 0x02
#define MODE_CH3 0x03

#define DMA_ADD_CH0 0x00
#define DMA_ADD_CH1 0x02
#define DMA_ADD_CH2 0x04
#define DMA_ADD_CH3 0x06
#define DMA_ADD_CH4 0xC0
#define DMA_ADD_CH5 0xC4
#define DMA_ADD_CH6 0xC8
#define DMA_ADD_CH7 0xCC

#define DMA_COUNT_CH0 0x01
#define DMA_COUNT_CH1 0x03
#define DMA_COUNT_CH2 0x05
#define DMA_COUNT_CH3 0x07
#define DMA_COUNT_CH4 0xC2
#define DMA_COUNT_CH5 0xC6
#define DMA_COUNT_CH6 0xCA
#define DMA_COUNT_CH7 0xCE

#define LOW_BYTE(x)         (x & 0x00FF)
#define HI_BYTE(x)          ((x & 0xFF00) >> 8)


enum channels {
    MEMORY_REFRESH,
    FREE0,
    FLOPPY,
    HD,
    NOT_USE,
    FREE1,
    FREE2,
    FREE3
};



struct dma_t {
    word base_address;
    word lunghezza;
    char page;
};

typedef struct dma_t dma_t;

void clear_bytepointer(int);
void InitDma();
void setu_transfer(int, int, int, int, int, int);
 __inline__ void enable_dma(unsigned int);
 __inline__ void disable_dma(unsigned int);
#endif
