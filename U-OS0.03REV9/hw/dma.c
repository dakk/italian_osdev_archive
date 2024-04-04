#include <dma.h>
#include <io.h>

unsigned char AddrPort[8] = { 0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC };
unsigned char PagePort[8] = { 0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A };

void InitDma()
{
}

void clear_bytepointer(int porta)
{

    if(porta==CLEAR_BYTE_P_M || porta==CLEAR_BYTE_P_S)
        outportb(0x00, porta);
}

__inline__ void enable_dma(unsigned int channel)
{

    if (channel >3)
        outportb(SINGLE_MASK_REGISTER_M,  channel & 3);
    else
        outportb(SINGLE_MASK_REGISTER_S,  channel);
}

__inline__ void disable_dma(unsigned int channel)
{

    if (channel >3)
        outportb(SINGLE_MASK_REGISTER_M,  channel & 7);
    else
        outportb(SINGLE_MASK_REGISTER_S,  channel | 4);
}

void setup_transfer(int mode, int direction, int autoinitialize, int modalita, int canale, word base_address, word page_register)
{
asm("cli");
    unsigned char parameters;
    parameters = mode | direction | autoinitialize | modalita | (canale&3);

    if(canale >=4)
        outportb(parameters, DMA_MODE_M);
    else if(canale <4)
        outportb(parameters, DMA_MODE_S);

    outportb(LOW_BYTE(base_address), AddrPort[canale] );
    if(canale<4) clear_bytepointer(CLEAR_BYTE_P_S);
    else if(canale <8) clear_bytepointer(CLEAR_BYTE_P_M);
    outportb(HI_BYTE(base_address), AddrPort[canale] );

    outportb((page_register & 0x0F), PagePort[canale]);


    enable_dma(canale);
asm("sti");
}
