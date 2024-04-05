/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: floppy.cpp			|
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

#include <hardware/driver/floppy.h>

//Funzioni per l'IO e per il dma
#include <hardware/driver/dma.h>
#include <hardware/io.h>

//Gestione degli IRQ
#include <hardware/cpu/irq.h>

//Messaggi di output e di debug
#include <kservices/kout.h>
#include <kservices/kdbg.h>

//Define, tipi...
#include <kdef.h>

/*
	**************************
	**	 Define		**
	**************************
*/

//Porte del controller
#define FDC_DOR		0x3F2
#define FDC_MSR 	0x3F4
#define FDC_DRS 	0x3F4
#define FDC_DATA 	0x3F5
#define FDC_DIR		0x3F7
#define FDC_CCR		0X3F7

//Commandi
#define CMD_SPECIFY 	0x03
#define CMD_WRITE	0xC5
#define CMD_READ	0xE6
#define CMD_RECAL	0x07
#define CMD_SENSEI	0x08
#define CMD_FORMAT	0x4D
#define CMD_SEEK	0x0F
#define CMD_VERSION	0x10

//DEBUG: Per abilitare il debug decomentare la seguente riga
//#define FLOPPY_DEBUG

/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace hardware::driver;



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////


bool Floppy::done = true; //L'operazione di IO è stata completata
byte Floppy::status[7] = {0}; //Stato
byte Floppy::sr0 = 0; //Registro sr0
byte Floppy::track; //Traccia corrente

/*
 Floppy Interupt Service Routine (IRQ: 6)
*/
void Floppy::FloppyISR(){

	#ifdef FLOPPY_DEBUG
		kservices::KDbg::WriteLine("floppy.cpp - IRQ 6: Floppy.");
	#endif

	//done indica che si è verificato un interrupt.
	done = true;
}


/*
 Inizializzo il driver del Floppy
*/
void Floppy::Init(){
	//Byte che contiene le informazioni su i 2 drive floppy.
	byte floppyData;

	
	byte floppy0; //Byte che contiene le informazioni sul primo drive floppy (fd0)
	byte floppy1; //Byte che contiene le informazioni sul secondo drive floppy (fd1)

	//Invio il comando di lettura e leggo le informazioni dal CMOS
	outportb(0x70, 0x10);
	floppyData = inportb(0x71);

	//Ottengo le informazioni sui due floppy leggendo i primi e gli ultimi 4 byte
	floppy0 = floppyData >> 4;
	floppy1 = floppyData & 0xF;

	//Controllo se fd0 è presente
	if (floppy0 != 0){
		kservices::KOut::Write("fd0: Floppy Drive A: ");
		PrintInfo(floppy0);
	}

	//Controllo se fd1 è presente
	if (floppy1 != 0){
		kservices::KOut::Write("fd1: Floppy Drive B: ");
		PrintInfo(floppy1);
	}
	
	//Inizializzo i driver del floppy solo se è presente il drive.
	if (floppyData != 0){
		//Aggiungo la Interupt service routine del floppy e abilito l'irq 6.
		hardware::cpu::IRQ::Add(FloppyISR, 6);
		hardware::cpu::IRQ::EnableIRQ(6);

		//Resetto il controllore del floppy
		Reset();
	}
}

/*
 Resetto il controller del floppy
*/
void Floppy::Reset(){
	//Resetta il controller del floppy.
	outportb(FDC_DOR, 0);

	//Setto il data rate a 500 Kbps
	outportb(FDC_DRS, 0);

	//Abilito gli interrupt e attendo un IRQ 6
	outportb(FDC_DOR, 0x0C);
	WaitFDC(true);

	//TODO:Documentarsi
	SendByte(CMD_SPECIFY);
	SendByte(0xDF);
	SendByte(0x02);

	//Vado alla prima traccia
	Seek(1);

	//Ricalibro il drive
	Recalibrate();
}

/*
 Scrivo le informazioni sul drive floppy
*/
void Floppy::PrintInfo(byte floppyInfo){
	switch(floppyInfo){
	case 0:
		kservices::KOut::WriteLine("No floppy drive");
		break;
	case 1:
		kservices::KOut::WriteLine("360 KByte");
		break;
	case 2:
		kservices::KOut::WriteLine("1.2 MByte");
		break;
	case 3:
		kservices::KOut::WriteLine("720 KByte");
		break;
	case 4:
		kservices::KOut::WriteLine("1.4 MByte");
		break;
	case 5:
		kservices::KOut::WriteLine("2.8 MByte");
		break;
	}
}

/*
 Attendo un IRQ 6 (ovvero che sia completata un'operazione)
*/
bool Floppy::WaitFDC(bool sense_interrupt){
	//Attendo un interrupt
	while(!done);

	int status_i = 0;
	
	while( (status_i < 7) && (inportb(FDC_MSR) & (1 << 4))){
		status[status_i++] = GetByte();
	}
	
	if (sense_interrupt == true){
		SendByte(CMD_SENSEI);
		sr0 = GetByte();
		track = GetByte();
	}

	done = false;

	return true;
}
/*
 Invio un byte al controller
*/
//TODO; False nel caso che l'operazione non va a buon fine
int Floppy::SendByte(byte Byte){
	/* Ogni ciclo viene incrementato di 1, se dopo 255 cicli il bit MRQ 
	 continua essere = a 0 restituisco un errore di timeout */
	int timeoutcounter;
	//Valore contenuto nel registro MSR
	volatile int MSR;

	for (timeoutcounter = 0; timeoutcounter < 128; timeoutcounter++){
		MSR = inportb(FDC_MSR);
		//Verifico se il bit MRQ è = 1. Se è = a uno significa che il controller è pronto.
		if ((MSR & 0xC0) == 0x80){
			outportb(FDC_DATA,Byte);
			return 0;
		}
		inportb(0x80); //Piccolo delay.
	}
	//Restituisco un errore di timeout
	return -1;
}

/*
 Leggo un byte dal controller
*/
int Floppy::GetByte(){
	/* Ogni ciclo viene incrementato di 1, se dopo 255 cicli il bit MRQ 
	 continua essere = a 0 restituisco un errore di timeout */
	int timeoutcounter;
	//Valore contenuto nel registro MSR
	volatile int MSR;

	for (timeoutcounter = 0; timeoutcounter < 128; timeoutcounter++){
		MSR = inportb(FDC_MSR);
		//Verifico se il bit MRQ è = 1. Se è = a uno significa che il controller è pronto.
		if ((MSR & 0xD0) == 0xD0){
			return inportb(FDC_DATA);
		}
		inportb(0x80); //Piccolo delay.
	}
	//Restituisco un errore di timeout
	return -1;
}

/*
 Accendo il motore
*/
void Floppy::MotorON(){
	/*Per accendere il motore deve essere selezionato il bit MOTA per il drive A (fd0)
	  o il bit MOTB per il drive B (fd1) del registro DOR -> A: 0x1C B: 0x2C*/
	outportb(0x3F2,0x1C);
}

/*
 Spengo il motore
*/
void Floppy::MotorOFF(){
	outportb(0x3F2,0x0C);
}

/*
 Vado a una certa traccia.
*/
//TODO: Da finire
bool Floppy::Seek(int seektrack){
	MotorON();

	//Controllo se sono già alla traccia richiesta
	if (seektrack == track) return true;
	
	//Invio il comando seek al FDC
	SendByte(CMD_SEEK);
	SendByte(0);
	SendByte((byte) seektrack); //ATTENZIONE: Pericolo BUG

	//Attendo un iterrupt.
	if (WaitFDC(true) == false) return false;

	//Piccolo delay
	for (int i = 0; i < (~(0)); i++) inportb(0x80);

	//Verifico se la funzione seek ha funzionato correttamente
	if ((sr0 == 0x20) && (seektrack == track)){
		return true;
	}else{
		#ifdef FLOPPY_DEBUG
			kservices::KDbg::WriteLine("floppy.cpp - Seek Error:");
			kservices::KDbg::Write("floppy.cpp - Track: ");
			kservices::KDbg::WriteLine((int) track);
			kservices::KDbg::Write("floppy.cpp - sr0: ");
			kservices::KDbg::WriteLine((int) sr0);
		#endif
		return false;
	}

	//MotorOFF();
}

/*
 Ricalibro il floppy
*/
void Floppy::Recalibrate(){
	MotorON();

	SendByte(CMD_RECAL);
	SendByte(0);

	WaitFDC(true);

	MotorOFF();
}

/*
  Converte il blocco nella tripletta head/track/sector
*/
void Floppy::block2hts(int block, int *head, int *track, int *sector) {
	*head = (block / 18) % 2;
	*track = block / 36;
	*sector = block % 18 + 1;
}

/*
  Fa un'operazione di input o di output di n blocchi
*/
bool Floppy::BlockIO(int block, int blockn, byte *blockbuffer, bool read){
	int head, track, sector; //Tripletta head, track, sector
	int tbaddr = 0x80000L; //Indirizzo per il DMA

	//Copio i dati da blockbuffer all'area di memoria per il DMA
	if (read == false && blockbuffer != 0){
		dword *tbmem = (dword *) tbaddr;
		for (int i = 0; i < (blockn * 512); i++){
			tbmem[i] = blockbuffer[i];
		}
	}

	//Converto il blocco nella tripletta head, track, sector
	block2hts(block,&head,&track,&sector);

	//Tre tentativi	
	for (int tries = 0; tries < 3; tries++){

		MotorON();
		for (int i = 0; i < (~(0)); i++) inportb(0x80);	// piccolo delay

		//Controllo se è stato cambiato il disco
		if (inportb(FDC_DIR) & 0x80) {
			#ifdef FLOPPY_DEBUG
				kservices::KDbg::WriteLine("floppy.cpp - BlockIO - Disk changed");
			#endif
			//Mi riposiziono sulla prima traccia
			//Seek(1);
			//Ricalibro
			//Recalibrate();;
			//TODO:Aggiungere delay o sleep
			//sleep(50);

			//TODO: Rieseguire??
			//return fdc_rw(block, blockbuff, read,
					      //nosectors);
		}

		//Vado alla traccia in cui si trova il settore che devo leggere
		if (!Seek(track)) {
			#ifdef FLOPPY_DEBUG
				kservices::KDbg::WriteLine("floppy.cpp - BlockIO - Seek Error");
			#endif
		}

		//TODO: Eseguire un recalibrate???
		//Recalibrate();
		//MotorON(); //Dopo il recalibrate è necessario ri-accendere il motore...

		for (int i = 0; i < (~(0)); i++) inportb(0x80);	 // piccolo delay

		outportb(FDC_CCR, 0);

		for (int i = 0; i < (~(0)); i++) inportb(0x80);	//piccolo delay

		if(read){
			DMA::StartDMA(2, tbaddr, blockn * 512, false);
			SendByte(CMD_READ);
		}else{
			DMA::StartDMA(2, tbaddr, blockn * 512, true);
			SendByte(CMD_WRITE);
		}

		SendByte(head << 2);
		SendByte(track);
		SendByte(head);
		SendByte(sector);
		SendByte(2);	/* 512 bytes/sector */
		SendByte(18);
		SendByte(0x1b);	/* gap 3 size for 1.44M read/write */
		SendByte(0xff);	/* DTL = unused */

		for (int i = 0; i < (~(0)); i++) inportb(0x80); //Piccolo delay

		if (!WaitFDC(true)) {
			#ifdef FLOPPY_DEBUG
				kservices::KDbg::Write("floppy.cpp - ");

				if (read == true){
					kservices::KDbg::WriteLine("READ TIMEOUT");
				}else{
					kservices::KDbg::WriteLine("WRITE TIMEOT");
				}
			#endif
		}

		for (int i = 0; i < (~(0)); i++) inportb(0x80);	//piccolo delay

		if ((status[0] & 0xc0) == 0){
			break;
		}else{
			kservices::KOut::WriteLine("ERROR");
		}

		for(int z = 0; z < 512 ; z++){
			for (int i = 0; i < (~(0)); i++) inportb(0x80);	// piccolo delay
		}
		Recalibrate();
		for(int z = 0; z < 512 ; z++){
			for (int i = 0; i < (~(0)); i++) inportb(0x80);	// piccolo delay
		}
	}

	//Copio i dati dal'area del DMA al blockbuffer	
	if (read == true && blockbuffer != 0){
		dword *tbmem = (dword *) tbaddr;
		for (int i = 0; i < (blockn * 512); i++) blockbuffer[i] = tbmem[i];
	}

	for (int i = 0; i < (~(0)); i++) inportb(0x80);	// piccolo delay

	MotorOFF();

	for (int i = 0; i < (~(0)); i++) inportb(0x80);	// piccolo delay

	return true;
}

/*
  Leggo un blocco
*/
bool Floppy::ReadBlock(int block, int blockn, byte *blockbuffer){
	return BlockIO(block, blockn, blockbuffer, true);
}

/*
  Scrivo un blocco
*/
bool Floppy::WriteBlock(int block, int blockn, byte *blockbuffer){
	return BlockIO(block, blockn, blockbuffer, false);
}
