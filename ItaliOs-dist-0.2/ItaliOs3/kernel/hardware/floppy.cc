#include <irq.h>
#include <task.h>
#include <io.h>
#include <video.h>
#include <cstddef>
#include <dma.h>
#include <cstring>

	extern volatile tick_t tick;

namespace {
	volatile bool motor;
	volatile bool done;
	byte status[7] = { 0 };
	byte statsz = 0;
	byte sr0 = 0;
	byte fdc_track = 0xff;
	dword tbaddr = 0x80000L;	// indirizzo di dove si trova la traccia sotto 1Mega
	static bool dchange = false;
	
#define FDC_DOR  (0x3f2)	/* Digital Output Register */
#define FDC_MSR  (0x3f4)	/* Main Status Register (input) */
#define FDC_DRS  (0x3f4)	/* Data Rate Select Register (output) */
#define FDC_DATA (0x3f5)	/* Data Register */
#define FDC_DIR  (0x3f7)	/* Digital Input Register (input) */
#define FDC_CCR  (0x3f7)	/* Configuration Control Register (output) */


#define CMD_SPECIFY (0x03)	/* imposta i tempi del floppy */
#define CMD_WRITE   (0xc5)	/* scrive i dati */
#define CMD_READ    (0xe6)	/* legge i dati */
#define CMD_RECAL   (0x07)	/* ricalibra */
#define CMD_SENSEI  (0x08)	/* stato dell'interrupt */
#define CMD_FORMAT  (0x4d)	/* formatta una traccia */
#define CMD_SEEK    (0x0f)	/* cerca una traccia */
#define CMD_VERSION (0x10)	/* versione del floppy */

//handler dell'irq del floppy
	void fdc_handler() {
		done = true;
	}

//spedisce un byte al controller del floppy
	void sendbyte(int byte) {
		volatile int msr;
		int tmo;

		for (tmo = 0; tmo < 128; tmo++) {
			msr = inportb(FDC_MSR);
			if ((msr & 0xc0) == 0x80) {
				outportb(FDC_DATA, byte);
				return;
			}
			inportb(0x80);	/* piccolo delay */
		}
	}

	//prende un byte al controller del floppy
	int getbyte() {
		volatile int msr;
		int tmo;

		for (tmo = 0; tmo < 128; tmo++) {
			msr = inportb(FDC_MSR);
			if ((msr & 0xd0) == 0xd0) {
				return inportb(FDC_DATA);
			}
			inportb(0x80);
		}
		return -1;	// timeout di lettura
	}



	// aspetta che un comando de floppy è completo
	bool waitfdc(bool sensei) {
		dword valore = tick + 100;
		asm("sti");
		while ((!done) &&  (valore > tick));
		if(!done)
			return false;
		//leggo i risultati di un comando
		statsz = 0;
		while ((statsz < 7) && (inportb(FDC_MSR) & (1 << 4))) {
			status[statsz++] = getbyte();
		}

// 		//vedo i risultati dell'ultimo interrupt
		if (sensei) {
			sendbyte(CMD_SENSEI);
			sr0 = getbyte();
			fdc_track = getbyte();
		}

		done = false;

		return true;
	}


//accende il motore
	void motoron(void) {
		if (!motor) {
			outportb(FDC_DOR, 0x1c);
			for (int i = 0; i < (~(0)); i++)
				inportb(0x80);	/* piccolo delay */
			motor = true;
		}
	}

	//spegne il motore
	void motoroff(void) {
		if (motor) {
			outportb(FDC_DOR, 0x0c);
			motor = false;
		}
	}


// ricalibra il driver
	void recalibrate(void) {
		motoron();

		//mando il comando per il ricalibramento
		sendbyte(CMD_RECAL);
		sendbyte(0);

		//aspetto finchè ha finitp
		waitfdc(true);

		//spengo il motore
		motoroff();
	}

	// si posizione alla traccia 
	bool seek(int track) {
		if (fdc_track == track)	// ci sono già?
			return true;

		// spedisce il comando di posizionamento
		sendbyte(CMD_SEEK);
		sendbyte(0);
		sendbyte(track);

		// aspetto che la ricdrca della traccia è finita
		if (!waitfdc(true))
			return false;	//time out

		for (int i = 0; i < (~(0)); i++)
			inportb(0x80);	/* piccolo delay */

		//controllo se la seek ha lavorato
		if ((sr0 != 0x20) || (fdc_track != track))
			return false;
		else
			return true;
	}

	// rimette il floppy in uno stato conosciuto
	void reset(void) {
		//ferma il motore e disabilita il DMA
		outportb(FDC_DOR, 0);

		motor = false;

		// 500K/s 
		outportb(FDC_DRS, 0);

		// riabilita gli interrupt
		outportb(FDC_DOR, 0x0c);

		/* risetto il floppy */
		done = true;
		waitfdc(true);

		sendbyte(CMD_SPECIFY);
		sendbyte(0xdf);	/* SRT = 3ms, HUT = 240ms */
		sendbyte(0x02);	/* HLT = 16ms, ND = 0 */

		seek(1);
		recalibrate();
	}

	// Converte il cluster nella tripletta head/track/sector
	void block2hts(int block, int *head, int *track, int *sector) {
		*head = (block % (18 * 2)) / (18);
		*track = block / (18 * 2);
		*sector = block % 18 + 1;
	}



	inline byte reard_rtc_address(byte address) {
		outportb(0x70, address);
		return inportb(0x71);
	}

	bool classifica_rtc_floppy(byte valore) {
		bool presente = true;
		switch (valore & 0xf) {
		case 0:
			presente = false;
			kout << "Non presente ";
			break;
		case 1:
			kout << "5 1/4\" 360Kb";
			break;
		case 2:
			kout << "5 1/4\" 1.2MB";
			break;
		case 3:
			kout << "3 1/2\" 720KB ";
			break;
		case 4:
			kout << "3 1/2\" 1.44MB ";
			break;
		default:
			kout << "ATTENZIONE: Valore non valido!!!!!!!!!!!!";
			presente = false;
		}
		return presente;
	}
	void leggi_versione() {
		sendbyte(CMD_VERSION);
		byte ver = getbyte();
		switch (ver) {
		case 0x90:
			kout << "Modello uPD765B";
			break;
		case 0x80:
			kout << "Modello uPD765A oppure uPD765A-2";
			break;
		default:
			kout << "Modello sconosciuto";
		}
	}


/*Questa funzione si occupa di leggere e scrivere dei blocchi sul floppy */
	bool fdc_rw(int block, byte * blockbuff, bool read, size_t nosectors) {
		int head, track, sector, tries;
		size_t copycount = 0;
		byte *p_tbaddr = (byte *) 0x80000;
		unsigned char *p_blockbuff = blockbuff;

		// converto l'indirizzo logico nell'indirizzo fisico
		block2hts(block, &head, &track, &sector);

		//accendo il motore
		motoron();

		if (!read && blockbuff) {
			//copio i dati dal buffer dei dati al buffer della traccia
			for (copycount = 0; copycount < (nosectors * 512);
			     copycount++) {
				*p_tbaddr = *p_blockbuff;
				p_blockbuff++;
				p_tbaddr++;
			}
		}

		for (tries = 0; tries < 3; tries++) {
			//vedo se l'utente a cambiato la traccia
			if (inportb(FDC_DIR) & 0x80) {
				dchange = true;
				seek(1);	//se si mi posiziona sulla prima traccia
				recalibrate();
				motoroff();
				kout << "FDC: Hai cambiato il disko, rimetti quello buono, e riprova\n";
				sleep(50);

				return fdc_rw(block, blockbuff, read,
					      nosectors);
			}
			//muove la testina sulla traccia giusta
			if (!seek(track)) {
				motoroff();
				kout << "FDC: errore nel trovare la traccia track \n";
				return false;
			}
			//velocità: 500k/s
			outportb(FDC_CCR, 0);

			//spedisco il comando e programmo il dma
			if (read) {
				dma_xfer(2, tbaddr, nosectors * 512, false);
				sendbyte(CMD_READ);
			} else {
				dma_xfer(2, tbaddr, nosectors * 512, true);
				sendbyte(CMD_WRITE);
			}

			sendbyte(head << 2);
			sendbyte(track);
			sendbyte(head);
			sendbyte(sector);
			sendbyte(2);	/* 512 bytes/sector */
			sendbyte(18);
			sendbyte(0x1b);	/* gap 3 size for 1.44M read/write */
			sendbyte(0xff);	/* DTL = unused */

			//aspetto il completamento del comando
			if (!waitfdc(true)) {
				//kout << "Timed out, riprovare con un reset()\n";
				reset();
				return fdc_rw(block, blockbuff, read,
					      nosectors);
			}

			if ((status[0] & 0xc0) == 0)
				break;	//tutto è andato liscio

			recalibrate();	//qualcosa è andato storto riproviamo
		}

		//spengo il motore
		motoroff();

		if (read && blockbuff) {
			//copio i dati dal buffer della traccia al buffer dei dati
			p_blockbuff = blockbuff;
			p_tbaddr = (byte *) 0x80000;
			for (copycount = 0; copycount < (nosectors * 512);
			     copycount++) {
				*p_blockbuff = *p_tbaddr;
				p_blockbuff++;
				p_tbaddr++;
			}
		}

		return (tries != 3);
	}

	struct cluster{
		byte valore[512];
		size_t count;
		size_t num;
	};

	#define NUM_CLUSTER 15
	#define MAX_COUNT 500
	cluster buff_floppy[NUM_CLUSTER];
}

#define _BUFFERING_

void init_floppy()
{
	for(size_t i = 0; i< NUM_CLUSTER; i++)
		buff_floppy[i].count = 0;
	add_irq(fdc_handler, 6);
	enable_irq(6);
	reset();
	byte present = reard_rtc_address(0x10);
	kout << "A: ";
	if (classifica_rtc_floppy(present >> 4)) {
		leggi_versione();
		kout << endl;
	} else {
		kout << endl;
	}
	kout << "B: ";
	if (classifica_rtc_floppy(present)) {
	} else {
		kout << endl;
	}
}


//legge un blocco grosso 512 yte
bool read_block(int block, byte * blockbuff, size_t nosectors)
{
	int track = 0, sector = 0, head = 0, track2 = 0, result = 0;
	size_t loop = 0;
#ifdef _BUFFERING_
	size_t libero = (~0);
	for(size_t i = 0; i< NUM_CLUSTER; i++){
		if(buff_floppy[i].count){
			if(buff_floppy[i].num == block){
			//	kout << "ho trovato il blocco " << block << " in " << i << endl;
				memcpy(blockbuff, buff_floppy[i].valore, 512);
				if(buff_floppy[i].count < MAX_COUNT) buff_floppy[i].count++;
				return true;
			}
		}else{
			libero = i;
		}
	}
	if(libero == (~0)){
		libero = 0;
		for(size_t i = 1; i< NUM_CLUSTER; i++){
			if(buff_floppy[libero].count > buff_floppy[i].count) libero = i;
		}
		//kout << "ho scelto da occupare " << libero << endl;
	}
#endif
        //kout << "devo leggere il blocco " << block << endl;

//il floppy può leggere da entrambi le testine, ma non su tracce multiple contemporaneamnte

	block2hts(block, &head, &track, &sector);
	block2hts(block + nosectors, &head, &track2, &sector);

	if (track != track2) {
		//kout << "devo fare una lettura multipla" << endl;
		for (loop = 0; loop < nosectors; loop++)
			result = fdc_rw(block + loop,
					blockbuff + (loop * 512), true, 1);
		return result;
	}
#ifdef _BUFFERING_
	//kout << "scritto " << block << " in " << libero;
#endif
	bool fdc_ret = fdc_rw(block, blockbuff, true, nosectors);
#ifdef _BUFFERING_
	//kout << "   OK\n";
	memcpy(buff_floppy[libero].valore, blockbuff, 512);
	buff_floppy[libero].count = 1;
	buff_floppy[libero].num = block;
#endif
	//kout << "scritto " << block << " in " << libero<<endl;
	return fdc_ret;
}

void de_init_floppy(void)
{
	outportb(FDC_DOR, 0x0c);
}

bool write_block(int block, byte * blockbuff, unsigned long nosectors)
{
	return fdc_rw(block, blockbuff, false, nosectors);
}
