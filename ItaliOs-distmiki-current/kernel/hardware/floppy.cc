#include <cstdlib>
#include <irq.h>
#include <task.h>
#include <io.h>
#include <video.h>
#include <cstddef>
#include <dma.h>
#include <debug.h>


unsigned char *Bufrx = NULL;
#define MAX_BUF_ 8
struct tbufrx {
  unsigned char flag;
  unsigned char token;
  int clust;
  unsigned char *buf;
} bufrx[MAX_BUF_];

byte fdc_track = 0xff;



namespace {
	volatile bool motor;
	volatile bool done;
	byte status[7] = { 0 };
	byte statsz = 0;
	byte sr0 = 0;
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


void init_bufrx() {
int n;
     Bufrx = (unsigned char *) malloc(8192);
     for (n=0;n<MAX_BUF_;n++) {
        bufrx[n].flag = 0;
        bufrx[n].token = 0;
        bufrx[n].clust = 10000;
        bufrx[n].buf = Bufrx + n*512;
     }
}

void free_bufrx() {
    free(Bufrx); 
}



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
           	byte tmmp;
		while (!done);
		//leggo i risultati di un comando
		statsz = 0;
		while ((statsz < 7) && (inportb(FDC_MSR) & (1 << 4))) {
			status[statsz++] = getbyte();
		}

		//vedo i risultati dell'ultimo interrupt
		if (sensei) {
			sendbyte(CMD_SENSEI);
                        if ((tmmp = getbyte()) != 255) sr0 = tmmp;
                        if ((tmmp = getbyte()) != 255) fdc_track = tmmp;
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
//                debug << "AA1 fdc " << fdc_track << " track " << track << endl;

		// spedisce il comando di posizionamento
//                kout << "DIRET " << fdc_track << " " << track << endl;
		sendbyte(CMD_SEEK);
		sendbyte(0);
		sendbyte(track);

		// aspetto che la ricdrca della traccia è finita
		if (!waitfdc(true))
			return false;	//time out
//                debug << "AA2 fdc " << fdc_track << " track " << track << endl;

/*                fdc_track = track; */
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
//                        debug << "LEGGI " << head << " " << track << " " << sector << endl;
			sendbyte(head << 2);
			sendbyte(track);
			sendbyte(head);
			sendbyte(sector);
			sendbyte(2);	/* 512 bytes/sector */
			sendbyte(18);
			sendbyte(0x1b);	/* gap 3 size for 1.44M read/write */
			sendbyte(0xff);	/* DTL = unused */

			//aspetto il completamento del comando
//                        kout << " fdc " << fdc_track << " track " << track << endl;
			if (!waitfdc(true)) {
				kout << "Timed out, riprovare con un reset()\n";
				reset();
				return fdc_rw(block, blockbuff, read,
					      nosectors);
			}

//                      kout << "ZZ2 fdc " << fdc_track << " track " << track << endl;
			if ((status[0] & 0xc0) == 0)
				break;	//tutto è andato liscio

			recalibrate();	//qualcosa è andato storto riproviamo
//                      kout << "ZZ3 fdc " << fdc_track << " track " << track << endl;
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

}

void init_floppy()
{
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
        init_bufrx();
}


//legge un blocco grosso 512 yte
bool _read_block(int block, byte * blockbuff, size_t nosectors)
{
	int track = 0, sector = 0, head = 0, track2 = 0, result = 0;
	size_t loop = 0;
//il floppy può leggere da entrambi le testine, ma non su tracce multiple contemporaneamnte

	block2hts(block, &head, &track, &sector);
	block2hts(block + nosectors, &head, &track2, &sector);
//        kout << "H" << head << " T" << track2 << " S" << sector << endl;
	if (track != track2) {
		for (loop = 0; loop < nosectors; loop++)
			result = fdc_rw(block + loop,
					blockbuff + (loop * 512), true, 1);
		return result;
	}
        return fdc_rw(block, blockbuff, true, nosectors);
}

void de_init_floppy(void)
{
	outportb(FDC_DOR, 0x0c);
        free_bufrx();
}

bool _write_block(int block, byte * blockbuff, unsigned long nosectors)
{
	return fdc_rw(block, blockbuff, false, nosectors);
}


















bool read_block(int block, byte * blockbuff, size_t nosectors) {
  int n1,n2=0,n3,n4=0,n5;
  if (Bufrx == NULL) init_bufrx();   
  n3 = 255;
  for (n1 = 0;n1<MAX_BUF_;n1++) {
     if (bufrx[n1].clust == block) {
        bufrx[n1].token = 100;
        for (n5=0;n5<512;n5++) blockbuff[n5] = bufrx[n1].buf[n5];
/*        memcpy(blockbuff,bufrx[n1].buf,512); */
        
/*        printf("trovato %u token=%u cluster=%u %u\n",n1,
            bufrx[n1].token,bufrx[n1].clust,bufrx[n1].buf[0]);*/
          debug << "Trovato " << n1 << " cluster " << bufrx[n1].clust << "\n"; 
        for (n5=0;n5<MAX_BUF_;n5++) bufrx[n5].token -= (bufrx[n5].token > 0);
        return 1;
     } else
     if (bufrx[n1].clust == 10000) n2 = n1; else 
     if (bufrx[n1].token < n3) {
         n4 = n1; n3 = bufrx[n1].token;
     }
  }
  if (!n2) n2 = n4;

  _read_block(block,blockbuff,1);

  for (n5=0;n5<512;n5++) bufrx[n2].buf[n5] = blockbuff[n5];
/*  memcpy(bufrx[n2].buf,blockbuff,512); */
  for (n5=0;n5<MAX_BUF_;n5++) bufrx[n5].token -= (bufrx[n5].token > 0);
  bufrx[n2].clust = block;
  bufrx[n2].token = 100;
  debug << "Impegnato " << n2 << " cluster " << bufrx[n2].clust << "\n";
/*  printf("impegnato %u token=%u cluster=%u %u\n",n2,
            bufrx[n2].token,bufrx[n2].clust,bufrx[n2].buf[0]);
  printf("rx %u\n",n); */
  return 1;
}



bool write_block(int block, byte * blockbuff, unsigned long nosectors) {
  int n1,n5;
  if (Bufrx == NULL) init_bufrx();   
  for (n1 = 0;n1<MAX_BUF_;n1++) {
     if (bufrx[n1].clust == block) {
        for (n5=0;n5<512;n5++) bufrx[n1].buf[n5] = blockbuff[n5];
/*        memcpy(bufrx[n1].buf,blockbuff,512); */
        break;
     }
  }

/*  printf("tx %u %02x%02x%02x\n",n,buf[0],buf[1],buf[2]);  */
  _write_block(block,blockbuff,1);
//  debug << "Scritto blocco " << block << "\n";
/*  printf("tx1 %u %02x%02x%02x\n",n,buf[0],buf[1],buf[2]);  */
  return 1;
}

/*
   block = settore
   blockbuff = buffer
   nosectors = numero settori   
*/

int floppy_r_buff(int puntat,byte *buff,int size) {
  int n4=0,n;
   int clust,punt1;
   byte buf[512];
   clust = puntat >> 9; /* cluster di 512 bytes */
   punt1 = puntat & 511;/* posizione all'interno del cluster */
//   debug << "floppy_r_buff clust=" << clust << " punt1=" << punt1 << endl;
   if ((size + punt1 ) > 512) return -4;
   for (n=0; n<MAX_BUF_;n++)
     if (bufrx[n].clust == clust) break;
   if (n == MAX_BUF_) { /* se non e' stato precaricato in un buffer */
     _read_block(clust,buf,1);/* lo riempio */
     for (n4=0;n4 < size; n4++)
        buff[n4] = buf[n4+punt1];
   } else {
     for (n4=0;n4 < size; n4++)
        buff[n4] = bufrx[n].buf[n4+punt1];
   }  
   return 0;   


}



//  rimasugli della floppy_r_buff probabilmente errati
//   if (n == MAX_BUF_) { /* se non e' stato precaricato in un buffer */
//     n3 = 255;
//     for (n = 0; n < MAX_BUF_; n++ )/* cerco quello + vecchio */
//       if (bufrx[n].clust == 10000) n2 = n; else 
//       if (bufrx[n].token < n3) {
//           n4 = n; n3 = bufrx[n].token;
//       }
//     n = n4; /* e lo occupo */
//     bufrx[n]
//     debug << "floppy_r_buff carico in " << n << endl;
//     _read_block(clust,bufrx[n].buf,1);/* e lo riempio */
//   }
/* adesso manovriamo */
//   debug << "floppy_r_buff buffer=" << n << " size=" << size << " buf=" << (char *)(bufrx[n].buf+punt1) << endl;












int floppy_w_buff(int puntat,byte *buff,int size) {
   static int n4,n;
   static int clust,punt1;
   static int puntat1;
   byte buf1[512];
   puntat1 = puntat;
   clust = puntat >> 9; /* cluster di 512 bytes */
   punt1 = puntat & 511;/* posizione all'interno del cluster */
/*  if (floppy_r_buff(puntat1,buff,size) < 0) return -4;*/
   for (n=0; n<MAX_BUF_;n++) {
     if (bufrx[n].clust == clust) break;
   }
   if (n == MAX_BUF_) {/* se non e' nei buffer */
      read_block(clust,buf1,1);
      for (n4=0;n4 < size; n4++)
         buf1[n4+punt1] = buff[n4];
      write_block(clust,buf1,1);/* e lo riempio */
      return 0;
   }
/* if (n == MAX_BUF_) return -4; errore inatteso??? */
   for (n4=0;n4 < size; n4++)
      bufrx[n].buf[n4+punt1] = buff[n4];
   write_block(clust,bufrx[n].buf,1);/* e lo riempio */
   return 0;   
   
}

