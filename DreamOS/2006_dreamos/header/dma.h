/*
 * Dreamos
 * dma.h
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

 /*
  * Autore Ivan Gualandri
  * Prima versione : 06/07/2004
  */

#ifndef __DMA_H_
#define __DMA_H_

#include <mydef.h>

#define DMA_MASTER 0
#define DMA_SLAVE 1

//Indirizzi del mode register
#define DMA_MODE_S 0x0B
#define DMA_MODE_M 0xD6

//Indirizzi per accedere al clear byte pointer
#define CLEAR_BYTE_P_S 0x0C
#define CLEAR_BYTE_P_M 0xD8

//Indirizzi per accedere al Mask Register
#define SINGLE_MASK_REGISTER_S 0x0A
#define SINGLE_MASK_REGISTER_M 0xD4

//Definisco le varie opzioni del mode Register
// 1.     Mode Selection
#define MODE_DEMAND 0x00
#define MODE_SINGLE 0x40
#define MODE_BLOCK 0x80
#define MODE_CASCADE 0xC0
// 2.     Direzione trasferimento (incremento o decremento del registro)
#define MODE_INC 0x00
#define MODE_DEC 0x20
// 3.     Autoinizializzazione
#define MODE_SC 0x00
#define MODE_AUTO 0x10
// 4.     Tipo trasferimento
#define MODE_VERIFY 0x00
#define MODE_WRITE 0x04
#define MODE_READ 0x08
// 5.     Selezione Canale
#define MODE_CH0 0x00
#define MODE_CH1 0x01
#define MODE_CH2 0x02
#define MODE_CH3 0x03

//Definisco gli address e i count port dei vari canali
// 1. Address Port
#define DMA_ADD_CH0 0x00
#define DMA_ADD_CH1 0x02
#define DMA_ADD_CH2 0x04
#define DMA_ADD_CH3 0x06
#define DMA_ADD_CH4 0xC0
#define DMA_ADD_CH5 0xC4
#define DMA_ADD_CH6 0xC8
#define DMA_ADD_CH7 0xCC

// 2. Count port
#define DMA_COUNT_CH0 0x01
#define DMA_COUNT_CH1 0x03
#define DMA_COUNT_CH2 0x05
#define DMA_COUNT_CH3 0x07
#define DMA_COUNT_CH4 0xC2
#define DMA_COUNT_CH5 0xC6
#define DMA_COUNT_CH6 0xCA
#define DMA_COUNT_CH7 0xCE



//Ma perche servono? Prendo il primo byte o il secondo byte
#define LOW_BYTE(x)         (x & 0x00FF)
#define HI_BYTE(x)          ((x & 0xFF00) >> 8)

//Questa enum la utilizzero per selezionare i vari canali del dma
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


//Struttura che gestira' il trasferimento dati col dma.
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
