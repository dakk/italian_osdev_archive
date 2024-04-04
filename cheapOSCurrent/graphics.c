#include "graphics.h"

unsigned int VIDEO_XMAX, VIDEO_YMAX, risoluzione;
unsigned long pennello;
bool isBlinking;
char* VideoMem;

//union ddi_buffer_t VideoBuffer;

struct Video_Caret cursore;

bool Video_getRes()
{
	int tmpEstBit, tmpEstBit2, charHeight;
	//Prendo la risoluzione della modalita' testo del video
	//Caratteri in orizzontale
	outportb(VIDEO_CRTC_REG_ID,VIDEO_TOT_COLS);
	VIDEO_XMAX = inportb(VIDEO_CRTC_REG_DATA) +1;

	//I bit 6 e 7 della porta 0x03cc restituiscono un codice che indica la risoluzione
	//verticale in righe come segue:
	// 00 -> Riservato
	// 01 -> 400 righe
	// 10 -> 350 righe
	// 11 -> 480 righe
	tmpEstBit = inportb(0x03cc)&0xc0;
	tmpEstBit>>=6;

	switch(tmpEstBit)
	{
		case 0:
			tmpEstBit2 = 0;
			kprint("Errore nel leggere la risoluzione video.\nIl valore 00 e' riservato.\n");
			return FALSE;
		break;

		case 1:
			tmpEstBit2 = 400;
		break;

		case 2:
			tmpEstBit2 = 350;
		break;

		case 3:
			tmpEstBit2 = 480;
		break;

		default:
			tmpEstBit2 = 1;
			kprint("Questo evento non dovrebbe mai verificarsi!\n");
			return FALSE;
		break;
	}

	//Variabile globale aggiornata per usarla con il Video_getOpt
	risoluzione = tmpEstBit2;

	//kprint("Altezza totale video: %d\n",(unsigned long)tmpEstBit2);

	//Altezza del carattere
	outportb(VIDEO_CRTC_REG_ID,0x09);
	charHeight = (inportb(VIDEO_CRTC_REG_DATA)&0x1f) +1;
	//kprint("Altezza del carattere: %d\n",(unsigned long)charHeight);

	//Risoluzione verticale = numero righe / altezza carattere
	VIDEO_YMAX = tmpEstBit2 / charHeight;

	//kprint("Risoluzione video: %d x %d\n", (unsigned long)VIDEO_XMAX, (unsigned long)VIDEO_YMAX);
	return TRUE;
}

//Stampa a video la risoluzione dello schermo
bool Video_printRes()
{
	kprint("(%d x %d)\n", (unsigned long)VIDEO_XMAX, (unsigned long)VIDEO_YMAX);
	return TRUE;
}

bool Video_enableBlink()
{
	//Abilito il blinking bit. Ho solo 8 colori per lo sfondo ma posso far lampeggiare
	//i caratteri
	unsigned char blinkDisabler, bd2;

	//Azzero la circuiteria leggendo un valore dal registro
	//input status #1 alla porta 0x3da
	bd2 = (unsigned char)inportb(0x3da);
	//kprint("bd2=%d\n",bd2);

	//Seleziono la porta e l'indice voluto (0x10 e' la porta, 0x20 e' il bit che voglio modificare)
	outportb(0x3c0,0x10|0x20);
	//prendo il valore del byte (porta 0x3c1 legge il byte all'indirizzo 0x10)
	blinkDisabler = inportb(0x3c1);
	//kprint("blinkDisabler=%x\n",blinkDisabler);

	//Lo maschero per abilitare il bit 4 (disabilita i colori chiari e abilita il blinking)
	blinkDisabler |= 0x8;
	//Devo ancora una volta azzerare la circuiteria altrimenti non viene scritto un valore
	//corretto
	bd2 = (unsigned char)inportb(0x3da);
	//Seleziono la porta...
	outportb(0x3c0,0x10|0x20);
	//...e scrivo il byte modificato.
	outportb(0x3c0,(unsigned char)blinkDisabler);

	isBlinking = TRUE;

	return TRUE;
}

bool Video_disableBlink()
{
	//Disabilito il blinking bit per avere 16 colori per fg e bg e non fare
	//lampeggiare i caratteri.
	unsigned char blinkDisabler, bd2;

	//Azzero la circuiteria leggendo un valore dal registro
	//input status #1 alla porta 0x3da
	bd2 = (unsigned char)inportb(0x3da);
	//kprint("bd2=%d\n",bd2);

	//Seleziono la porta e l'indice voluto (0x10 e' la porta, 0x20 e' il bit che voglio modificare)
	outportb(0x3c0,0x10|0x20);
	//prendo il valore del byte (porta 0x3c1 legge il byte all'indirizzo 0x10)
	blinkDisabler = inportb(0x3c1);
	//kprint("blinkDisabler=%x\n",blinkDisabler);
	//Lo maschero per disabilitare il bit 4 (disabilita il blinking e abilita i colori chiari)
	blinkDisabler &= 0xf7;
	//Devo ancora una volta azzerare la circuiteria altrimenti non viene scritto un valore
	//corretto
	bd2 = (unsigned char)inportb(0x3da);
	//Seleziono la porta...
	outportb(0x3c0,0x10|0x20);
	//...e scrivo il byte modificato.
	outportb(0x3c0,(unsigned char)blinkDisabler);

	isBlinking = FALSE;

	return TRUE;
}

bool Video_setCaretDim(char s_or_b)
{
	switch(s_or_b)
	{
		case 'b':
			outportb(VIDEO_CRTC_REG_ID, VIDEO_CURSOR_START);
			outportb(VIDEO_CRTC_REG_DATA,2);
			outportb(VIDEO_CRTC_REG_ID, VIDEO_CURSOR_END);
			outportb(VIDEO_CRTC_REG_DATA,11);
		break;

		case 's':
			outportb(VIDEO_CRTC_REG_ID, VIDEO_CURSOR_START);
			outportb(VIDEO_CRTC_REG_DATA,7);
			outportb(VIDEO_CRTC_REG_ID, VIDEO_CURSOR_END);
			outportb(VIDEO_CRTC_REG_DATA,11);
		break;

		default:
			kprint("Errore nel parametro di Video_setCaretDim. Valori corretti: 'b' e 's'.\n");
			return FALSE;
		break;
	};

	return TRUE;
}

int Video_getCaretDim()
{
	unsigned int cStart, cEnd;
	outportb(VIDEO_CRTC_REG_ID, VIDEO_CURSOR_START);
	cStart = (unsigned int)(inportb(VIDEO_CRTC_REG_DATA));
	outportb(VIDEO_CRTC_REG_ID, VIDEO_CURSOR_END);
	cEnd = (unsigned int)(inportb(VIDEO_CRTC_REG_DATA));
	return (cEnd-cStart);
}

bool Video_updateCaret(unsigned long x, unsigned long y)
{
	int posVMTmp;
	//Trasformo x e y in coordinate della memoria video e controllo che siano
	//all'interno del range che si puo' mostrare sullo schermo (altrimenti errore!)
	posVMTmp = VIDEO_XMAX * y + x;

	//Mi assicuro di essere nella parte visibile dello schermo!!!
	if(posVMTmp>=VIDEO_XMAX*VIDEO_YMAX)
	{
		kprint("Video_updateCaret: cursore oltre lo spazio disponibile (%d,%d)\n",x,y);
		return FALSE;
	}
	if(posVMTmp<0)
	{
		kprint("Video_updateCaret: cursore prima dello spazio disponibile (%d,%d)\n",x,y);
		return FALSE;
	}

	//Se invece tutto va bene...
	//...modifico il valore...
	cursore.X = x;
	cursore.Y = y;
	//Messo cosi' mi serve per spostare correttamente il cursore fisico...
	cursore.posVM = posVMTmp;

	//...e modifico il cursore fisico
	outportb(VIDEO_CRTC_REG_ID,VIDEO_CURSOR_L);
	outportb(VIDEO_CRTC_REG_DATA,(unsigned char)cursore.posVM); 	//Imposto il byte inferiore (il cast tronca la
									//word al byte inferiore)
	outportb(VIDEO_CRTC_REG_ID,VIDEO_CURSOR_H);
	outportb(VIDEO_CRTC_REG_DATA,cursore.posVM>>8);

	return TRUE;
}

void Video_locateCaret()
{
 	//kprint("Video_locateCaret: inizio localiz ");
	outportb(VIDEO_CRTC_REG_ID, VIDEO_CURSOR_H);
	cursore.posVM = inportb(VIDEO_CRTC_REG_DATA);		//posVM=byte superiore
	cursore.posVM<<=8;					//shifto a sinistra di 8
	outportb(VIDEO_CRTC_REG_ID, VIDEO_CURSOR_L);
	cursore.posVM += inportb(VIDEO_CRTC_REG_DATA);		//sommo il byte basso.
	//kprint("Video_xmax = %d, Video_ymax + %d\n",VIDEO_XMAX,VIDEO_YMAX);

	//Adesso restituisco le coordinate x e y
	cursore.X = cursore.posVM % VIDEO_XMAX;
	cursore.Y = cursore.posVM / VIDEO_XMAX;
	//kprint("Il cursore si trova alla posizione x=%d, y=%d\n",cursore.X,cursore.Y);
	return;
}

bool Video_setColor(unsigned char bg, unsigned char fg)
{
	unsigned int colore;
	colore = 0;
	colore = bg;
	colore<<=4;
	colore += fg;
	pennello = colore;
	return TRUE;
}

void Video_scroll()
{
	int i;
	//Copio il contenuto della memoria video dalla seconda riga in poi a partire dalla prima...
	mem_copy(0xB80A0,0xB8000,VIDEO_XMAX*(VIDEO_YMAX-1)*2);
	//...e cancello l'ultima riga per il nuovo output, posizionando il cursore all'inizio dell'ultima riga
	for(i=VIDEO_XMAX*(VIDEO_YMAX-1)*2;i<VIDEO_XMAX*VIDEO_YMAX*2;i+=2)
	{
  		VideoMem[i] = ' ';
		VideoMem[i+1] = 0x07;
	}
	//Sposto il riferimento del cursore
	Video_updateCaret(0,24);
	return;
}

void Video_incrementCaret()
{
	//Se sono nell'ultima colonna...
	if(cursore.X==VIDEO_XMAX-1)
	{	//Se sono nell'ultima riga scrollo
		if(cursore.Y==VIDEO_YMAX-1)
			Video_scroll();
		//altrimenti sposto il cursore all'inizio della riga successiva
		else
			Video_updateCaret(0,cursore.Y+1);
	}
	//Se sono in una colonna qualsiasi...
	else
	{
		Video_updateCaret(cursore.X+1,cursore.Y);
	}
	return;

}

void Video_clrScr()
{
	unsigned int i, pennelloOld;
	//Cancello lo schermo...

	//Salvo il colore del pennello
	pennelloOld = pennello;
	Video_setColor(BLACK,WHITE);
	//Posiziono il cursore all'inizio
	Video_updateCaret(0,0);

	for(i=0;i<VIDEO_XMAX*VIDEO_YMAX;i++)
	{
		Video_printCh(' ');
	}

	//Ripristino il valore precedente del pennello
	pennello = pennelloOld;
	//...e riporto il cursore nella posizione (0,0)
	Video_updateCaret(0,0);

	return;
}

void Video_printCh(char ch)
{
	int indexVM,i;
	//Prendo la posizione attuale del cursore
	Video_locateCaret();
	//kprint("Presa posizione del cursore (%d,%d)\n",cursore.X, cursore.Y);
	indexVM = (((VIDEO_XMAX)*cursore.Y)+cursore.X)*2;

	//Controllo che il carattere non sia un ritorno a capo o una tabulazione
	if(ch=='\n')
	{
		//Controllo di non essere nell'ultima riga
		if(cursore.Y==VIDEO_YMAX-1)
		{
			Video_scroll();
			Video_updateCaret(0,VIDEO_YMAX-1);
			return;
		}
		//Metto il cursore alla posizione 0 nella nuova linea
		Video_updateCaret(0,cursore.Y+1);
	}

	if(ch=='\t')
	{
		for(i=0;i<TAB_SIZE;i++)
			Video_incrementCaret();
	}

	//Scrivo nella memoria video il carattere passato alla posizione del cursore
	VideoMem[indexVM] = ch;
	//Imposto come colore quello del pennello attuale
	VideoMem[indexVM+1] = pennello;

	//Faccio avanzare di uno il cursore
	Video_incrementCaret();

	return;
}


bool Video_init()
{
	//Dichiaro dove inizia la memoria video
	VideoMem  = (unsigned char*)0xB8000;

	kprint("Video:");
	//Leggo la risoluzione del video
	if(!Video_getRes())
		kprint("Errore nel leggere la modalita' video\n");

	//Stampo la risoluzione del video

	if(!Video_printRes())
		kprint("Errore nello scrivere la modalita' video\n");

	//Disablito il lampeggiamento per avere 16 colori anche per lo sfondo
	if(!Video_enableBlink())
		kprint("Errore nella abilitazione blinking.\n");
	else
		kprint("\tBlinking abilitato\n");

	//Setto la dimensione del cursore a piccolo (4 pixel) e la rileggo
	if(!Video_setCaretDim('s'))
		kprint("Impossibile impostare altezza cursore.\n");
	else
		kprint("\t\tAltezza cursore impostata a %d\n", Video_getCaretDim());

	//Posiziono il cursore all'inizio dello schermo
	if(!Video_updateCaret(0,0))
		kprint("Errore nel posizionamento del cursore.\n");

	//Rileggo la posizione del cursore per testare la funzione
	//Video_locateCaret();

	//Imposto il pennello
	Video_setColor(BLACK,WHITE);
	//Stampo il codice esadecimale per sicurezza
	kprint("\t\tValore pennello: %x\n",pennello);

	return TRUE;
}

//Non fa nulla
bool Video_shutdown()
{
	return TRUE;
}
