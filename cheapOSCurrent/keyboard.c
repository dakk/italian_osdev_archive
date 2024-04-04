#include "keyboard.h"

bool num, caps, scroll;	//Indicano se sono premuti o meno (1=premuto)
bool shiftSx, ctrlSx, alt, winSx, altgr, winDx, menu, ctrlDx, shiftDx;  //Indicano se sono premuti o meno (1=premuto)
unsigned char scanset;	//Indica lo scanset attuale
unsigned char layoutAttivo;	//Indica il layout attivo
unsigned char ledsOn;	//Serve per tracciare quali led sono attivi
unsigned long carComposito;
int ultimoCodice;//, bufferTop, bufferBottom;
semaphoro_t readAvail;

extern struct variantiTasto Kbd_layout[MAX_ASCII_CHARS];
extern int consoleOnScreen;

//Buffer dei caratteri.

struct keycoda_t	Qbufferkey;

/*TOGLIERE*/
int i;
unsigned int provaLetturaCarattere;
/**/

ISR_DECLARE(kbd);

inline void Kbd_waitAck()
{
	while(inportb(KBD_64)&2);
	return ;//TRUE;
}

bool Kbd_waitWrite()
{
	//Aspetta fino a quando il byte 1 dello status vale 1
	unsigned char tmp;
	do
	{
		tmp = inportb(KBD_64);
	}
	while(tmp&2);

	return TRUE;
}

bool Kbd_bufferStatus(unsigned long param)
{
	if(keycoda_elements(&Qbufferkey)>0&&param==consoleOnScreen)
		return TRUE;
	else
		return FALSE;
}

ISR_CODE_H(kbd)
{
	unsigned int key, caretDim;
 	unsigned char ledstmp;

	key = inportb(KBD_60);
	switch(key)
	{
		case 0xAA:
			if(shiftSx)
			{
				shiftSx = !shiftSx;
				//kprint("Shift sx rilasciato\n");
			}
			else
			{
				kprint("Tastiera: reset mode!\n");
				//Reimposto lo scanset precedente
				Kbd_scanSet(scanset);
				kprint("Reimpostato lo scanset al valore precedente (%d)\n",scanset);
				//Riattivo i led usati in precedenza
				//E' necessaria la variabile temporanea per avere un corretto valore
				//di ledsOn alla fine dell'operazione
				ledstmp = ledsOn;
				Kbd_setLeds(ledstmp);//Spegne tutti i led e...
				Kbd_setLeds(ledstmp); //...riattiva quelli precedenti
				kprint("Reimpostati i led al valore precedente (%d)\n",ledsOn);
			}
		break;

		case 0xFA:
			//kprint("Aknowledge.\n");
		break;

		//Test diagnostico tastiera fallito
		case 0xFC:
			kprint("ATTENZIONE: test diagnostico della tastiera fallito\n");
		break;

		//Gestione dei led
		case 0x01:
			Kbd_setLeds(NUM_LOCK);
		break;

		case 0x76:
			Kbd_setLeds(SCROLL_LOCK);
		break;

		case 0x1D:
			Kbd_setLeds(CAPS_LOCK);
		break;


		case 0x7B:
			//La dimensione del cursore prenderla attraverso l'interfaccia al video!
			caretDim = Video_getCaretDim();
			if(caretDim==4)
				Video_setCaretDim('b');
			if(caretDim==9)
				Video_setCaretDim('s');
		break;

		//Gestione dello shift Sx (per quello rilasciato vedere in alto codice 0xAA)
		case 0x2A:
			shiftSx = !shiftSx;
			//kprint("Shift sx premuto\n");
			ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
		break;

		//Gestione del tasto ctrl sx
		case 0x38:
			ctrlSx = TRUE;
			//kprint("Ctrl sx premuto\n");
			ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
		break;

		case 0xB8:
			ctrlSx = FALSE;
			//kprint("Ctrl sx rilasciato\n");
			ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
		break;

		//Gestione del tasto alt
		case 0x71:
			alt = TRUE;
			//kprint("Alt premuto\n");
			ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
		break;

		case 0xF1:
			alt = FALSE;
			//kprint("Alt rilasciato\n");
			ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
		break;

		//Gestione del tasto altGr (alt destro)
		case 0x72:
			altgr = TRUE;
			//kprint("Altgr premuto\n");
			ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
		break;

		case 0xF2:
			altgr = FALSE;
			//kprint("Altgr rilasciato\n");
			ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
		break;

		//Gestione ctrl destro
		//ATTENZIONE VEDERE DOPO, CAMBIA IN BASE AL LAYOUT (layout americano 2 make)!

  		case 0x3A:
			ctrlDx = TRUE;
			//kprint("Ctrl destro premuto\n");
			ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
		break;

		//----------------------------------------------
		//Ctrl destro e' composto (layout usa) cosi': 0x3A, 0x15, 0xD7, 0x6A
		//I due nel mezzo vengono ignorati
		//Il 15 essendo in make di y, lo tratto nel default!!!
		/*
		case 0x15:
			//Occhio! Conflitta con il make del carattere y
			if(ultimoCodice==0x3A)	//Dovrebbe evitare il problema di non far vedere y
			{
				ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
				kprint("Ctrl in tastiera americana\n")
			}
			kprint("tasto y\n");
			key = 0x15;
		break;
		*/
		case 0xD7:
			ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
		break;
		//-----------------------------------------------

		case 0x6A:	//Codice nel layout ita
			ctrlDx = FALSE;
			//kprint("Ctrl destro rilasciato\n");
			ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
		break;

		case 0xBA:	//Codice nel layout usa
			ctrlDx = FALSE;
			//kprint("Ctrl destro rilasciato\n");
			ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
		break;

		//Gestione shift destro
		case 0x36:
			shiftDx = !shiftDx;
			//kprint("Shift destro premuto\n");
			ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
		break;

		case 0xB6:
			shiftDx = !shiftDx;
			//kprint("Shift destro rilasciato\n");
			ultimoCodice = 0;	//Vedi ISR_CODE_H per maggiori info
		break;

		default:
			//Viene messo nel buffer un carattere cosi' composto:
			//Primo byte modificatori vari, secondo byte codice carattere.
			// xxXXXXXX XXXXXXXX
			//  |-----| |------|
			// Modific.  Codice carattere
			// Modif: altgr, Win, Menu, Alt, Ctrl, Shift

			//Compongo il valore dei modificatori:
			//Occhio! Bisogna seccare i break code, altrimenti vengono stampati!!!

			//Ignoro il secondo make del Ctrldx in layout americano
			//Faccio cosi' perche' altrimenti quando premo ctrl mi scrive
			//anche la y
			if(key==0x15&&ultimoCodice==0x3A)
			{
				kprint("Era il secondo make del ctrl\n");
				break;
			}

			carComposito = 0;

			if(shiftSx||shiftDx)
				carComposito += KBD_MOD_SHIFT;
			if(ctrlSx||ctrlDx)
				carComposito += KBD_MOD_CTRL;
			if(alt)
				carComposito += KBD_MOD_ALT;
			//Se il caps lock e' attivo...
			if(ledsOn&CAPS_LOCK)
			{
				if((key>=0x10&&key<=0x19)||(key>=0x1E&&key<=0x26)||(key>=0x2C&&key<=0x32)||(key>=0x90&&key<=0x99)||(key>=0x9E&&key<=0xA6)||(key>=0xAC&&key<=0xB2))
				{
					//kprint("non e' un numero\n");
					if(shiftSx||shiftDx)
					{
						carComposito -= KBD_MOD_SHIFT;
						//kprint("carcomposito = %x - shift premuto (dx = %d, sx =%d)\n",carComposito,shiftDx,shiftSx);
					}
					if(!(shiftSx||shiftDx))
					{
						carComposito += KBD_MOD_SHIFT;
						//kprint("carcomposito = %x - shift rilasciato (dx = %d, sx =%d)\n",carComposito,shiftDx,shiftSx);
					}
				}
				//kprint("e' un numero\n");
			}

			//Se il num lock e' disattivo...
			if(!(ledsOn&NUM_LOCK))
			{
				if((key>=0x45&&key<=0x54)||(key==0x37))
				{
					if(shiftSx||shiftDx)
					{
						carComposito -= KBD_MOD_SHIFT;
					}
					if(!(shiftSx||shiftDx))
					{
						carComposito += KBD_MOD_SHIFT;
					}

				}
			}

			if(altgr)
				carComposito += KBD_MOD_ALTGR;

			//kprint("Carcomposito = %x (prima shift), ",carComposito);
			//Adesso shifto a sinistra di 8 e aggiungo il codice carattere
			carComposito = carComposito << 8;
			//kprint("%x (dopo shift), ",carComposito);
			carComposito += key;
			//kprint("%x (aggiunta key=%x)\n",carComposito,key);

			//Qui controllo se e' un make o un break code. Se e' un break code
			//non viene inserito nel buffer.
			if(ultimoCodice==0)	//E' un make code
			{
				//kprint("Make code %x\n",ultimoCodice);
				/*Adesso, se il carattere composito e' una combinazione di tasti,
			  	controllo se tale combinazione e' registrata tra gli shortcuts*/
				if(ctrlDx||ctrlSx||alt||altgr||(carComposito>0x57&&carComposito<0x64))
				{
					//kprint("Combo su make code (carcomposito = %x)\n",carComposito);
					if(Kbd_findCombo(carComposito))
					{
						ultimoCodice = carComposito;
						break;
					}
				}

				//kprint("Controllo su make code: carComposito= %x\n",carComposito);

				//Per prima cosa cerco tra i caratteri ascii...
				for(i=0;i<MAX_ASCII_CHARS;i++)
				{
					if(carComposito==Kbd_layout[i].codice)
					{
						//kprint("Trovato codice %x (carattere %c, dec %d)\n",Kbd_layout[i].codice, Kbd_layout[i].asciiChar,Kbd_layout[i].asciiChar);
						//kprint("%c",Kbd_layout[i].asciiChar);

						ultimoCodice = Kbd_layout[i].codice;
						keycoda_ins(&Qbufferkey,Kbd_layout[i].asciiChar);	//<-------------------------------

						//Se e' un carattere esce...
						break;
					}
					//kprint("Codice Kbd_layout[%d]=%x\n");
				}

				//kprint("Corrispondenza non trovata per %x\n",carComposito);

				//Kbd_bufferWrite(carComposito);
				//ultimoCodice = (int)carComposito;
				//kprint("ultimoCodice = %x\n",ultimoCodice);
				break;
			}
			if(ultimoCodice==(carComposito-0x80))	//E' un break code
			{
				//kprint("Questo era un break code (non metto in buffer)\n");
				ultimoCodice = 0;
				break;
			}


			if((ultimoCodice!=0)&&(ultimoCodice==carComposito))
			{
				//kprint("Questo e' uno dei tasti con make/break uguali\n");
				//Per ora mi limito a toglierli dal buffer...
				//Kbd_bufferRead(0, &buffer);<---------------------------------------------------------------------------??????????????

				ultimoCodice = 0;
			}

			//Questo accade se si premono parecchi tasti contemporaneamente
			if((ultimoCodice!=0)&&(ultimoCodice!=(carComposito-0x80)))
			{
				//kprint("ISR_CODE_H: break code non corrispondente allo scan code\n");
				//Per ripristinare una condizione in cui il buffer puo' continuare ad
				//accettare caratteri invece di bloccarsi, quando si trova un break code
				//differente dallo scan code, si azzera tutti i codici incongruenti letti
				//da tastiera!
				ultimoCodice = 0;
			}

		break;

	}

	irq_end();
	}

bool Kbd_scanSet(int sset)
{
	//Occhio! Questo imposta lo scan set, poi bisogna impostare anche il layout
	//di tastiera!!!

	//Questo serve solo ad assicurarsi che il parametro sia corretto
	if(sset==1||sset==2||sset==3)
	{
		outportb(KBD_60,0xF0);
		//Aspetto che il comando sia stato eseguito
		Kbd_waitAck();
		outportb(KBD_60,sset);
		//Aspetto che il comando sia stato eseguito
		Kbd_waitAck();
	}
	else
	{
		kprint("Kbd_scanSet: parametro errato (ammessi solo 1,2,3)\n");
		return FALSE;
	}

	scanset = sset;

	return TRUE;
}

int Kbd_getScanSet()
{
	//Restituisce lo scanset attuale (serve per decidere quali tipo di segnali processare)
	return scanset;

}

bool Kbd_setLeds(unsigned char leds)
{
	unsigned char ledtmp;

	//Leds e' un valore tra 0 e 7. I bit 0,1,2 sono rispettivamente SCROLL_LOCK
	//NUM_LOCK E CAPS_LOCK. Se sono a 1 sono attivi, altrimenti sono spenti

	if(leds>7)
	{
		kprint("Kbd_setLeds: combinazione di led impossibile\n");
		return FALSE;
	}


	//Prendo il valore precedente di ledsOn e ne faccio lo xor con il valore attuale.
	ledtmp = ledsOn;
	ledtmp ^= leds;

	outportb(KBD_60,0xED);
	//Aspetto che il comando sia stato eseguito
	Kbd_waitAck();
  	outportb(KBD_60,ledtmp);
	//Aspetto che il comando sia stato eseguito
	Kbd_waitAck();

	ledsOn = ledtmp;

	return TRUE;
}

bool Kbd_setLayout(int layout)
{
	if(layout>2)
	{
		kprint("Kbd_setLayout: layout inesistente\n");
		return FALSE;
	}
	//Fare la funzione che copia in memoria il layout specificato

	layoutAttivo = layout;

	return TRUE;
}

int Kbd_getLayout()
{
	return layoutAttivo;
}

bool Kbd_init()
{
	//int modo,stato,i;
	shiftSx = FALSE;
	shiftDx = FALSE;
	ctrlSx = FALSE;
	ctrlDx = FALSE;
	alt = FALSE;
	//winSx = FALSE;
	altgr = FALSE;
	//Questi non dovrebbero servire piu' dato che per motivi di tempo non vengono usati!!!
	//winDx = FALSE;
	//menu = FALSE;

	ultimoCodice = 0;
	//Inizializzo il semaforo
	readAvail = VERDE;

	kprint("Keyboard:");

	keycoda_init(&Qbufferkey);
	//Forza tutti i tasti a restituire sia il make che il break code
	//(altrimenti Ctrl dx e altgr rendono soltanto il make!!!)
	outportb(KBD_60,0xF8);
	Kbd_waitAck();

	//SI PASSA SUBITO AL MODO 3 CHE HA UN SOLO MAKE CODE E
	//UN SOLO BREAK CODE PER TASTO!!!

	if(!Kbd_scanSet(3))
		kprint("Kbd_init: impossibile inizializzare scan set.\nAttivato scan set di default (modo 2)\n");
		//Non mi sembra il caso di uscire qui, tanto uno scan set di default esiste!!!
		//Provare per credere: staccare la tastiera e riattaccarla passa al modo 2 automaticamente!!!

	//Impostazione dei led (NUM_LOCK abilitato per default).
	ledsOn=0;
	if(!Kbd_setLeds(NUM_LOCK))
		kprint("Kbd_init: impossibile impostare i led\n");


	isr_setISR(0x21,ISR_ADDRESS(kbd));
	irq_enable(1);

	//Inizializza la tabella degli shortcut.
	Kbd_shortcuts_init();

	//Prova-------------------------------------------------------------------------INIZIO
	//time_addTJ(100,0,kbd_svuota);

	//Aggiunge i combo per passare tra una console e l'altra (sono del tipo Ctrl-Alt-X)
	Kbd_add_shortcut(0x0658,Console_1Up);
	Kbd_add_shortcut(0x0659,Console_2Up);
	Kbd_add_shortcut(0x065A,Console_3Up);
	Kbd_add_shortcut(0x065B,Console_4Up);

	//Aggiungo i combo per vedere o meno i messaggi di errore sotto la console
	Kbd_add_shortcut(0x0063,Console_mostraMessaggi);	//F12 attiva/disattiva messaggi


	//Prova-------------------------------------------------------------------------FINE
	kprint("OK\n");
	return TRUE;
}


bool Kbd_shutdown()
{
	//Non fa nulla ma serve per l'interfaccia
	return TRUE;
}

//Prende un carattere dal buffer
char Kbd_getCh(int console)
{
	char tmp;
	/*Mettere la parte sull'esclusione*/

	//semaphoro_down(&readAvail);

	kwait(Kbd_bufferStatus,(unsigned int)console);
	tmp = keycoda_pop(&Qbufferkey);

	//semaphoro_up(&readAvail);

	return tmp;
}
