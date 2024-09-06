
/*
 * Dreamos
 * dma.c
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

#include <dma.h>
#include <io.h>

//dma_t * dmadata = (dma_t*) 0x85000;
unsigned char AddrPort[8] = { 0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC };
unsigned char PagePort[8] = { 0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A };

void InitDma()
{
//Questa funzione prepara la struttura dati attia al trasferimento con il dma
//    dmadata->base_address = 0x00;
//    dmadata->page = 0x00;
//    dmadata->lunghezza = 0x00;
}

void clear_bytepointer(int porta)
{
//L'input utile sara'  CLEAR_BYTE_P_M, CLEAR_BYTE_P_S
    if(porta==CLEAR_BYTE_P_M || porta==CLEAR_BYTE_P_S)
        outportb(0x00, porta);
}

__inline__ void enable_dma(unsigned int channel)
{
//Questa funzione serve per abilitare un canale dma
    if (channel >3)
        outportb(SINGLE_MASK_REGISTER_M,  channel & 3);  //Abilito un canale  del processore MASTER
    else
        outportb(SINGLE_MASK_REGISTER_S,  channel);         //Abilito un canale del processore slave
}

__inline__ void disable_dma(unsigned int channel)
{
//Questa funzione serve per disabilitare un canale dma
    if (channel >3)
        outportb(SINGLE_MASK_REGISTER_M,  channel & 7);   //Disabilito un canale del processore MASTER 7 = 111b
    else
        outportb(SINGLE_MASK_REGISTER_S,  channel | 4);     //Disabilito un canale del Processore SLAVE
}

void setup_transfer(int mode, int direction, int autoinitialize, int modalita, int canale, word base_address, word page_register)
{
asm("cli");
    unsigned char parameters;
    parameters = mode | direction | autoinitialize | modalita | (canale&3);
//devo sendare al mode register i parametri di settaggio del trasferimento.
    if(canale >=4)
        outportb(parameters, DMA_MODE_M);
    else if(canale <4)
        outportb(parameters, DMA_MODE_S);
//devo settare base_register, page_register
    outportb(LOW_BYTE(base_address), AddrPort[canale] );
    if(canale<4) clear_bytepointer(CLEAR_BYTE_P_S);
    else if(canale <8) clear_bytepointer(CLEAR_BYTE_P_M);
    outportb(HI_BYTE(base_address), AddrPort[canale] );
    //send_addresses(base_address, page);
    outportb((page_register & 0x0F), PagePort[canale]);

//devo mandare l'abilitazione dei canali
    enable_dma(canale);
asm("sti");
}
