/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: dma.cpp			|
	| Date: 25/04/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 


/*
	**************************
	**	 Include	**
	**************************
*/

#include <hardware/driver/dma.h>
#include <kservices/kout.h>
#include <hardware/io.h>

#include <kdef.h>

#define WRITE_TRANSFER 0X45
#define READ_TRANSFER 0x49

//Queste due macro servono per accedere alla parte bassa o alta di un integer.
#define LOW_BYTE(x)         (x & 0x00FF)
#define HI_BYTE(x)          ((x & 0xFF00) >> 8)


/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace hardware::driver;




	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////

//Dati registri
byte DMA::MaskReg[8] = { 0x0A, 0x0A, 0x0A, 0x0A, 0xD4, 0xD4, 0xD4, 0xD4 };
byte DMA::ModeReg[8] = { 0x0B, 0x0B, 0x0B, 0x0B, 0xD6, 0xD6, 0xD6, 0xD6 };
byte DMA::ClearReg[8] = { 0x0C, 0x0C, 0x0C, 0x0C, 0xD8, 0xD8, 0xD8, 0xD8 };

//Dati porte
byte DMA::PagePort[8] = { 0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A };
byte DMA::AddrPort[8] = { 0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC };
byte DMA::CountPort[8] = { 0x01, 0x03, 0x05, 0x07, 0xC2, 0xC6, 0xCA, 0xCE };

/*
  Inizializzo il driver DMA
*/
void DMA::Init(){
	
}

/*
  Inizio un'operazione del DMA
*/
void DMA::StartDMA(byte DMAchannel, byte page, unsigned int offset, unsigned int lenght, byte mode){
	//Disabilito gli interupt
	asm("cli");

	//Setto il MASK REGISTER. Il 3 bit dice al DMA che sto settando i registri del DMA.
	outportb(MaskReg[DMAchannel], (DMAchannel | 0x04));

	//Cancello ogni trasferimento che è in corso su questo canale
	outportb(ClearReg[DMAchannel], 0x00);

	//Setto la modalità.
	outportb(ModeReg[DMAchannel],mode);
	
	//Invio l'indirizzo del offset. Il primo byte è l'offset base. Il secondo byte è il byte alto del offset.
	outportb(AddrPort[DMAchannel], LOW_BYTE(offset));
	outportb(AddrPort[DMAchannel], HI_BYTE(offset));

	//Invio la pagina fisica
	outportb(PagePort[DMAchannel], page);
	
	//Invio la lunghezza dei dati. Il primo byte è il byte basso.
	outportb(CountPort[DMAchannel], LOW_BYTE(lenght));
	outportb(CountPort[DMAchannel], HI_BYTE(lenght));

	//Ho finito di settare i registri. Ora pulisco la mask e avvio il trasferimento.
	outportb(MaskReg[DMAchannel], DMAchannel);

	//Abilito gli interupt
	asm("sti");
}

/*
  Inizio un'operazione del dma
*/
void DMA::StartDMA(byte channel, unsigned long address, unsigned int length, bool read){
	byte page = 0;
	byte mode = 0;
	int offset = 0;

	if(read){
		mode = 0x48 + channel;
	}else{
		mode = 0x44 + channel;
	}

	page = address >> 16;
	offset = address & 0xFFFF;
	length--;

	StartDMA(channel, page, offset, length, mode);
}
