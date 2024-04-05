#include "console.h"

/*Prova*/
char* provastringa;
/*Fineprova*/

unsigned int pennelloConsole;
int consoleOnScreen;
bool messaggi = FALSE;
//Utilizzato dal carattere di reverse, stabilisce se si modifica il colore di fg o di bg
//(true = FG, false = BG). Default TRUE
extern bool isBlinking;

extern int VIDEO_XMAX, VIDEO_YMAX;
extern unsigned int AREA_CONSOLE;
extern struct console_session_t* console[5];

bool Console_clrScr(unsigned char cons)
{

	unsigned int i, fgOld, bgOld, cursTmp;

	//if(cons<0||cons>4)
	if(cons>4)
	{
		kprint("Console_clrscr: la console passata non esiste\n");
		return FALSE;
	}

	fgOld = console[cons]->fgColor;
	bgOld = console[cons]->bgColor;

	Console_setColors(cons, BLACK, WHITE);

	//Prendo la posizione corrente del cursore
	cursTmp = console[cons]->cursore;
	//kprint("Cursore era in %d ", cursTmp);

	//Posiziono il cursore all'inizio
	console[cons]->cursore = 0;

	//for(i=0;i<VIDEO_XMAX*VIDEO_YMAX;i++)
	for(i=0;i<AREA_CONSOLE/2;i++)
	{
		Console_printCh(cons,' ');
	}

	//Ripristino il valore precedente del pennello
	console[cons]->bgColor = bgOld;
	console[cons]->fgColor = fgOld;
	//...e riporto il cursore nella posizione precedente
	console[cons]->cursore = cursTmp;
	//kprint("cursore riportato in %d\n", console[cons]->cursore);
	return TRUE;
}

bool Console_gotoxy(unsigned char cons, unsigned char x, unsigned char y)
{

	//Controllo se la console e' valida
	//if(cons<0||cons>4)
	if(cons>4)
	{
		kprint("Console_gotoxy: la console da aggiornare non esiste\n");
		return FALSE;
	}

	//Controllo che le coordinate del cursore non vadano fuori dalla memoria assegnata
	//if(x*y>VIDEO_XMAX*VIDEO_YMAX)
	if(x*y>(AREA_CONSOLE/2))
	{
		kprint("Console_gotoxy: coordinate fuori dal range di memoria\n");
		return FALSE;
	}

	//Prendo, traduco le coordinate (x,y) in un indirizzo lineare e lo applico
	//(se dentro il range) al cursore della console specificata.
	console[cons]->cursore = ((y*VIDEO_XMAX)+x)*2;

	return TRUE;
}

bool Console_scroll(unsigned char cons)
{
	int i;
	//Solito controllo sulla console
	//if(cons<0||cons>4)
	if(cons>4)
	{
		kprint("Console_scroll: la console specificata non esiste\n");
		return FALSE;
	}
	//Deve prendere il contenuto della memoria video e spostarlo in sotto di una linea
	//Copio il contenuto della memoria video dalla seconda riga in poi a partire dalla prima...
	//mem_copy((mem_t)console[cons]->buffer+(VIDEO_XMAX*2),(mem_t)console[cons]->buffer,VIDEO_XMAX*(VIDEO_YMAX-1)*2);
	mem_copy((mem_t)console[cons]->buffer+(VIDEO_XMAX*2),(mem_t)console[cons]->buffer,AREA_CONSOLE-(VIDEO_XMAX*2));

	Console_setColors(cons,BLACK,WHITE);
	//...e cancello l'ultima riga per il nuovo output, posizionando il cursore all'inizio dell'ultima riga
	//for(i=VIDEO_XMAX*(VIDEO_YMAX-1)*2;i<(VIDEO_XMAX*VIDEO_YMAX*2)-1;i+=2)
	for(i=AREA_CONSOLE-(VIDEO_XMAX*2);i<AREA_CONSOLE;i+=2)
	{
  		console[cons]->buffer[i] = ' ';
		console[cons]->buffer[i+1] = pennelloConsole;
	}

	//Sposto il riferimento all'oggetto cursore nel buffer e nella memoria video
	//Console_gotoxy(cons,0,VIDEO_YMAX-1);
	Console_gotoxy(cons,0,((AREA_CONSOLE/2)%(VIDEO_XMAX))-1);
	return TRUE;
}

bool Console_setColors(unsigned char cons, unsigned char bg, unsigned char fg)
{
	//Controllo sulla console
	//if(cons<0||cons>4)
	if(cons>4)
	{
		kprint("Console_setColors: la console specificata non esiste\n");
		return FALSE;
	}

	console[cons]->bgColor = bg;
	console[cons]->fgColor = fg;

	pennelloConsole = 0;
	pennelloConsole = bg;
	pennelloConsole<<=4;
	pennelloConsole += fg;

	return TRUE;
}

bool Console_incrementaCursore(unsigned char cons)
{
	//Solito controllo sulla console
	//if(cons<0||cons>4)
	if(cons>4)
	{
		kprint("Console_incrementaCursore: la console specificata non esiste\n");
		return FALSE;
	}

	//Incremento il cursore della console specificata
	//if((console[cons]->cursore)+2>((VIDEO_XMAX*VIDEO_YMAX)*2)-1)
	if((console[cons]->cursore)+2>AREA_CONSOLE-1)
	{
		Console_scroll(cons);
		return TRUE;
	}

	console[cons]->cursore = console[cons]->cursore+2;

	return TRUE;
}

bool Console_printCh(unsigned char cons, unsigned char c)
{
	//Console e' un numero da 1 a 4 che indica la console
	//L'offset sara' la posizione attuale del cursore all'interno di una delle console
	//offsetVM e' l'offset nella parte di memoria riservata alla console, che in pratica
	//e' strutturata come la memoria video, quindi i caratteri sono a coppia (carattere, colori)
	int i, riga, cursTmp;
	//unsigned char bgTmp;
	//char* consoleMem;

	//Controllo subito che la console passata sia una di quelle valide
	//if(cons<0||cons>4)
	if(cons>4)
	{
		kprint("Console_printCh: la console su cui si vuole scrivere non esiste\n");
		return FALSE;
	}

	//Stabilita la console uso consoleMem per puntare all'inizio della sua area
	if(console[cons]->cursore>AREA_CONSOLE-1)
		Console_scroll(cons);
	//offsetVM = console[cons]->cursore;
	//kprint("Console_printCh: console[%d]->cursore: %d\n", cons, console[cons]->cursore);

	//Controllo che il carattere non sia un ritorno a capo o una tabulazione
	//kprint("Controllo caratteri speciali\n");
	if(c=='\n')
	{
		//kprint("Console_printCh: ritorno a capo\n");

		//Prendo il numero della riga attuale
		riga = ((console[cons]->cursore)/2) / VIDEO_XMAX;

		//kprint("Siamo alla riga %d. ",riga);

		//Controllo di non essere nell'ultima riga
		//if(riga==VIDEO_YMAX-1)
		if(riga==((AREA_CONSOLE/2)/VIDEO_XMAX)-1)
		{
			Console_scroll(cons);
			//Console_gotoxy(cons,0,VIDEO_YMAX-1);
			Console_gotoxy(cons,0,((AREA_CONSOLE/2)/(VIDEO_XMAX))-1);
			return TRUE;
		}
		//Metto il cursore della console alla posizione 0 nella nuova linea

  		Console_gotoxy(cons,0,riga+1);
		return TRUE;
	}
	//kprint("Controllato ritorno a capo\n");

	if(c=='\t')
	{
		//kprint("Console_printCh: tabulazione\n");
		for(i=0;i<TAB_SIZE;i++)
			Console_incrementaCursore(cons);
		return TRUE;
	}
	//kprint("Controllato tabulazione\n");

	//Controlliamo che non sia il carattere di backspace
	if(c=='\b')
	{
		//sposto il cursore indietro di uno, stampo un carattere vuoto e risposto il cursore indietro
		//Devo spostare 2 volte perche' il printch con lo spazio mi risposta il cursore
		if(console[cons]->cursore<2)
			return FALSE;

		console[cons]->cursore -= 2;
		Console_printCh(cons,' ');
		console[cons]->cursore -= 2;
		return TRUE;
	}

	//Freccetta a sinistra
	if(c==FRECCIA_SX)
	{
		if(console[cons]->cursore<2)
			return FALSE;
		console[cons]->cursore -= 2;
		return TRUE;
	}

	//Freccetta a destra
	if(c==FRECCIA_DX)
	{
		if(console[cons]->cursore>=(AREA_CONSOLE-2))
			return FALSE;
		console[cons]->cursore += 2;
		return TRUE;
	}

	//Freccetta in alto
	if(c==FRECCIA_SU)
	{
		//Controllo di non andare oltre la prima riga dello schermo
		if((console[cons]->cursore)<(VIDEO_XMAX*2)+2)
			return FALSE;

		console[cons]->cursore -= VIDEO_XMAX*2;
		return TRUE;
	}

	//Freccetta in basso
	if(c==FRECCIA_GIU)
	{
		if(console[cons]->cursore>(AREA_CONSOLE-(VIDEO_XMAX*2)))
			return TRUE;
		//Devo ricordarmi in che colonna si trova il cursore
		cursTmp = (console[cons]->cursore)%(VIDEO_XMAX*2);

		//Controllo di non sorpassare l'ultima riga dello schermo, nel tal caso scrollo
		if(console[cons]->cursore + (VIDEO_XMAX*2)>(AREA_CONSOLE))
		{
			Console_scroll(cons);
			//reimposto il cursore nella colonna appropriata
			console[cons]->cursore += cursTmp;
			return TRUE;
		}

		//In casi normali sposto semplicemente il cursore nella riga sotto
		console[cons]->cursore += VIDEO_XMAX*2;
		return TRUE;

	}

	//Cancella lo schermo (Canc)
	if(c==CANC)
	{
		//kprint("Carattere 24 %c trovato",24);
		Console_clrScr(cons);
		return TRUE;
	}

	//Riporta il cursore all'inizio dell'area video
	if(c==HOME)
	{
		console[cons]->cursore = 0;
		return TRUE;
	}

	//Caratteri per modificare i colori
	if(c==CON_REVERSE)
	{
		Console_setColors(consoleOnScreen, console[consoleOnScreen]->fgColor, console[consoleOnScreen]->bgColor);
		return TRUE;
	}

	if(c>=CON_BLACK&&c<=CON_WHITE)
	{
		Console_colorByChar(c);
		return TRUE;
	}

	if(c==CON_BLINK)
	{
		Console_blink(cons);
		return TRUE;
	}

	//Scrivo nella memoria video il carattere passato alla posizione del cursore
	//kprint("Console_printCh: cerco di inserire il carattere %c\n",c);
	console[cons]->buffer[console[cons]->cursore] = c;
	//kprint("Console_printCh: ho inserito %c\n",console[cons]->buffer[console[cons]->cursore]);
	//Imposto come colore quello del pennello attuale
	Console_setColors(cons,console[cons]->bgColor,console[cons]->fgColor);
	console[cons]->buffer[(console[cons]->cursore)+1] = pennelloConsole;
	//Questo serve per avere il colore del cursore dello stesso tipo del testo
	if((console[cons]->cursore)+3<AREA_CONSOLE)
		console[cons]->buffer[(console[cons]->cursore)+3] = pennelloConsole;
	//Faccio avanzare di uno il cursore
	//kprint("Console_printCh: cerco di incrementare\n");
	Console_incrementaCursore(cons);
	//kprint("Console_printCh: cursore a %d\n",console[cons]->cursore);
	//kprint("Cursore incrementato\n");
	//kprint("Contenuto buffer console[%d]= %s\n",cons,console[cons]->buffer);

	return TRUE;
}



/*Stampa a video stringhe semplici e mette(ra') su filesistem le stesse*/
bool Console_printStr(unsigned char cons, const char* stringa)
{
	int i;

	//Controllo subito che la console passata sia una di quelle valide
	//if(cons<0||cons>4)
	if(cons>4)
	{
		kprint("Console_printStr: la console su cui si vuole scrivere non esiste\n");
		return FALSE;
	}
	//kprint("Dentro Console_printStr\n");
	//Video_printRes();

	for(i=0;stringa[i]!='\0';i++)
	{
		Console_printCh(cons, stringa[i]);
	}

	//kprint("Console_printStr: stringa stampata\n");
	return TRUE;
}

int Console_getString(unsigned int cons, unsigned char charNum, char* buffer)
{
	int i;
	char tmp;


	i=0;

	//kprint("Console_getString: in attesa di caratteri...\n");

	while(i<charNum)
	{
		//kwait(Kbd_bufferStatus,0);
		tmp = Kbd_getCh(cons);
		//if(tmp==13)	//Se e' invio esco
		//	break;

		//altrimenti lo inserisco nel buffer
		buffer[i] = tmp;
		i++;
	}
	//Alla fine aggiungo uno \0 alla stringa ritornata
	//buffer[i] = '\0';

	//kprint("Console_getString: fine lettura caratteri.\n");

	return i;

}

bool Console_toScreen(unsigned char cons)
{
	//int x,y;
	if(cons>4)
	{
		kprint("Console_toScreen: la console specificata non esiste\n");
		return FALSE;
	}

	//mem_copy((mem_t)console[cons]->buffer,(mem_t)0xB8000,(mem_t)CONSOLEFS_MAX_IMAGE_LUNG);
	mem_copy((mem_t)console[cons]->buffer,(mem_t)0xB8000,(mem_t)AREA_CONSOLE);
	//Aggiorno il cursore video!!!
	//x = (console[cons]->cursore/2) % VIDEO_XMAX;

	//y = (console[cons]->cursore/2) / VIDEO_XMAX;

	//kprint("%d = %d,%d\n",console[cons]->cursore,x,y);

	//Video_updateCaret(x,y);
	Video_updateCaret((console[cons]->cursore/2) % VIDEO_XMAX,(console[cons]->cursore/2) / VIDEO_XMAX);

	//Notifico quale console e' sullo schermo
	consoleOnScreen = cons;

	//Video_updateCaret(20,20);
	return TRUE;

}

void Console_1Up()
{
	Console_toScreen(1);
	return;
}

void Console_2Up()
{
	Console_toScreen(2);
	return;
}

void Console_3Up()
{
	Console_toScreen(3);
	return;
}

void Console_4Up()
{
	Console_toScreen(4);
	return;
}

unsigned char Console_getConsoleOnScreen()
{
	return consoleOnScreen;
}

void Console_updateScreen()
{
	if(consoleOnScreen<1||consoleOnScreen>5)
		return;
	Console_toScreen(consoleOnScreen);
	if(console[consoleOnScreen]->blink)
		Video_enableBlink();
	else if(!(console[consoleOnScreen]->blink))
		Video_disableBlink();
	return;

}

void Console_mostraMessaggi()
{
	int i,j;
	int cursTmp[4];
	//Se non sta mostrando i messaggi li faccio mostrare
	if(messaggi==FALSE)
	{
		kprint("Dentro mostraMessaggi FALSE\n");
		AREA_CONSOLE = CONSOLEFS_MAX_IMAGE_LUNG -(VIDEO_XMAX*2)*12;
		if((console[consoleOnScreen]->cursore)>(VIDEO_XMAX*2*12)+1)
		{
			mem_copy(((mem_t)console[consoleOnScreen]->buffer)+(VIDEO_XMAX*2*12),(mem_t)console[consoleOnScreen]->buffer,AREA_CONSOLE);
			console[consoleOnScreen]->cursore -= (VIDEO_XMAX*2*12);
			Video_clrScr();
		}
		Video_clrScr();
		messaggi = TRUE;
		return;
	}
	//Se i messaggi sono visualizzati li faccio scomparire
	if(messaggi==TRUE)
	{
		kprint("Dentro mostraMessaggi FALSE\n");
  		AREA_CONSOLE = CONSOLEFS_MAX_IMAGE_LUNG;
		//Video_clrScr();
		//Svuoto tutto il contenuto della console oltre il cursore, altrimenti ricopia valori a caso
		for(j=1;j<5;j++)
		{
			cursTmp[j-1] = console[j]->cursore;

			for(i=cursTmp[j-1]/2;i<(AREA_CONSOLE/2)-2;i++)
			{
				Console_printCh(j,' ');
			}
			console[j]->cursore = cursTmp[j-1];
		}
		messaggi  = FALSE;
	}
	return;
}

void Console_colorByChar(unsigned char c)
{
	//Non faccio controlli sul carattere perche' ci ha gia' pensato
	//Console_printCh() quando ha richiamato questa funzione!

	unsigned char coloreTmp;

	//'Traduco' il codice nel colore corrispondente
	switch(c)
	{
		case CON_BLACK:
			coloreTmp = BLACK;
		break;

		case CON_DKBLUE:
			coloreTmp = DKBLUE;
		break;

		case CON_DKGREEN:
			coloreTmp = DKGREEN;
		break;

		case CON_DKCYAN:
			coloreTmp = DKCYAN;
		break;

		case CON_DKRED:
			coloreTmp = DKRED;
		break;

		case CON_DKPURPLE:
			coloreTmp = DKPURPLE;
		break;

		case CON_DKBROWN:
			coloreTmp = DKBROWN;
		break;

		case CON_WHITE_PLAIN:
			coloreTmp = LTGRAY;
		break;

		case CON_BLACK_BLINK:
			coloreTmp = DKGRAY;
		break;

		case CON_BLUE_BLINK:
			coloreTmp = LTBLUE;
		break;

		case CON_GREEN_BLINK:
			coloreTmp = LTGREEN;
		break;

		case CON_CYAN_BLINK:
			coloreTmp = LTCYAN;
		break;

		case CON_RED_BLINK:
			coloreTmp = LTRED;
		break;

		case CON_PURPLE_BLINK:
			coloreTmp = LTPURPLE;
		break;

		case CON_BROWN_BLINK:
			coloreTmp = LTBROWN;
		break;

		case CON_WHITE_BLINK:
			coloreTmp = WHITE;
		break;

		default:
		break;

	};

	//Questo switch serve per le macro dei colori lampeggianti, dato che se messo insieme a quello
	//sopra genera errori in quanto ci sarebbero piu' case con lo stesso valore
	switch(c)
	{
		case CON_LTGRAY:
		coloreTmp = LTGRAY;
		break;

		case CON_DKGRAY:
			coloreTmp = DKGRAY;
		break;

		case CON_LTBLUE:
			coloreTmp = LTBLUE;
		break;

		case CON_LTGREEN:
			coloreTmp = LTGREEN;
		break;

		case CON_LTCYAN:
			coloreTmp = LTCYAN;
		break;

		case CON_LTRED:
			coloreTmp = LTRED;
		break;

		case CON_LTPURPLE:
			coloreTmp = LTPURPLE;
		break;

		case CON_LTBROWN:
			coloreTmp = LTBROWN;
		break;

		case CON_WHITE:
			coloreTmp = WHITE;
		break;
	};

	//Modifico il colore di foreground
	Console_setColors(consoleOnScreen, console[consoleOnScreen]->bgColor, coloreTmp);
}

void Console_blink(unsigned char cons)
{

	//Controllo che la console sia giusta
	if(cons>4)
	{
		kprint("Console_blink: la console specificata non esiste\n");
		return;
	}

	//Se disattivo, lo attivo
	if(!(console[cons]->blink))
	{
		kprint("Console_blink: attivo blink\n");
		if(((console[cons]->bgColor)+0x8>0x07)&&((console[cons]->bgColor)+0x8<0x10))
		{
			console[cons]->bgColor += 8;
		}
		else
		{
			kprint("Impossibile incrementare di 8 lo sfondo, continuo senza cambiare\n");
		}
		Console_setColors(cons, console[cons]->bgColor, console[cons]->fgColor);
		Video_enableBlink();
		console[cons]->blink = TRUE;
		return;
	}

	//Se attivo, lo disattivo
	if(console[cons]->blink)
	{
		kprint("Console_blink: disattivo blink\n");
		if(((console[cons]->bgColor)-0x8>=0x00)&&((console[cons]->bgColor)-0x8<0x08))
		{
			console[cons]->bgColor -=0x8;
		}
		else
		{
			kprint("Impossibile decrementare di 8 lo sfondo, continuo senza cambiare\n");
		}
		Console_setColors(cons, console[cons]->bgColor, console[cons]->fgColor);
		Video_disableBlink();
		console[cons]->blink = FALSE;
		return;
	}

}




/*
//SPOSTATE IN FS_CONSOLE

bool Console_init()
{

	if(!Console_clrscr())
	{
		kprint("Console_init: impossibile inizializzare la console\n");
		return FALSE;
	}
	kprint("Console inizializzata\n");

	Video_printRes();

	return TRUE;
}

bool Console_shutdown()
{

	return TRUE;
}
*/
