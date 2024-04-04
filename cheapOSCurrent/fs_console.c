#include "fs_console.h"

extern struct keycoda_t Qbufferkey;
extern int consoleOnScreen;

//char* console[4];
struct console_session_t* console[5];

semaphoro_t Console_sem = VERDE;

int lungStrRoot;

//Usata per decidere quanta parte di console copiare a schermo
unsigned int AREA_CONSOLE = CONSOLEFS_MAX_IMAGE_LUNG;

char consoleName[5][10]=
{
	"\\",
	"\\Console1",
	"\\Console2",
	"\\Console3",
	"\\Console4"
};

bool Console_init()
{
	int i;
	char rootContent[] = "Console1\nConsole2\nConsole3\nConsole4";

	//All'inizio si mostra la console a tutto schermo.
	//AREA_CONSOLE = CONSOLEFS_MAX_IMAGE_LUNG;


	/*Alloco 4 aree di memoria per le 4 console disponibili + la root*/
	for(i=0;i<6;i++)
	{
		console[i]=(struct console_session_t*)mem_alloc((mem_t)sizeof(struct console_session_t));
		if(console[i]==0)
		{
			kprint("Console_init: errore allocazione memoria\n");
			return FALSE;
		}
		//Imposto i cursori delle strutture alle coordinate (0,0)
		console[i]->cursore = 0;
		console[i]->blink = TRUE;
		//kprint("|c%d=%d |",i,console[i]->cursore);

		//Imposto i colori di primo piano e sfondo a bianco su nero
		console[i]->bgColor = BLACK;
		console[i]->fgColor = WHITE;

		//kprint("Inz.cons.%d ",i);
	}

	for(i=0;i<5;i++)
	{
		//console[i]->cursore = 0;
		if(!Console_clrScr(i))
		{
			kprint("Console_init: impossibile inizializzare la console\n");
			return FALSE;
		}

	}

	//Scrivo nel buffer della root il contenuto
	//kprint("console_init: prima ksprint\n");
	//lungStrRoot = ksprint(console[0]->buffer,"%s",rootContent);
	for(i=0;rootContent[i]!='\0';i++)
	{
		console[0]->buffer[i] = rootContent[i];
	}
	lungStrRoot = i;

	//kprint("Contenuto Console0: %s\n",console[0]->buffer);
	//kprint("Console inizializzate\n");

	//Serve per impostare correttamente i parametri del driver della scheda grafica
	Video_getRes();

	consoleOnScreen = -1;

	//Questo time job copia la console a schermo ogni tick
	time_addTJ(1,0,Console_updateScreen);

	//kprint("Valori cursore: c1=%d, c2=%d, c3=%d, c4=%d\n",console[1]->cursore,console[2]->cursore,console[3]->cursore,console[4]->cursore);

	return TRUE;
}

bool Console_shutdown()
{
	int i;
	//Cerco di liberare le aree di memoria allocate nella init
	for(i=0;i<6;i++)
	{
		console[i]=(void*)mem_free((mem_t)sizeof(struct console_session_t));
		if(console[i]==FALSE)
		{
			kprint("Console_init: errore deallocazione memoria\n");
			return FALSE;
		}
	}

	return TRUE;
}

bool Console_open(void** session,char* nomefile)
{
	/*Controllo che il nome del file della console sia uno di quelli giusti*/
	int i,numConsole;

	numConsole = -1;

	//kprint("Dentro console_open\n");

	for(i=0;i<6;i++)
	{
		if(kstrcmp(consoleName[i],nomefile))
		{
			numConsole = i;
			//kprint("consoleName[%d]=%s, nomefile=%s\n",i,consoleName[i],nomefile);
		}
	}

	if(numConsole==0)
	{
		kprint("Sto aprendo la console 0\n");
		console[0]->cursore = 0;
		kprint("Contenuto:\n%s",console[0]->buffer);

	}

	*session = (void*)numConsole;

	return TRUE;
}

bool Console_close(void** session)
{
	//Libera la console per permettere ad altri processi di accedergli
	return TRUE;
}

int  Console_seek(void** session,int modo,int puntatore)
{
	int cons,newcursore;

	//Controllo che la sessione passata sia un argomento valido.
	//Parto dalla console 1, saltando la directory gia' trattata sopra
	cons = (int)*session;

	if(cons<0||cons>5)
	{
		kprint("Console_seek: non esiste la console specificata\n");
		return -1;
	}

	//kprint("Effettuo il seek sulla console %d\n",cons);

	//Seleziono quale dei differenti tipi di seek posso eseguire
	switch(modo)
		{
		case (MREL) :
			newcursore=console[cons]->cursore+puntatore;
			if(newcursore>=0 && newcursore<AREA_CONSOLE)
				console[cons]->cursore+=puntatore;
		break;

		case (MINIZIO) :
			if(puntatore>=0 && puntatore<AREA_CONSOLE)
				console[cons]->cursore=puntatore;
		break;

		case (MFINE) :
			newcursore=CONSOLEFS_MAX_IMAGE_LUNG-puntatore;
			if(newcursore>=0 && newcursore<AREA_CONSOLE)
				console[cons]->cursore=newcursore;
		break;

		default :
			kprint("Console_seek: Modo Seek Sconosciuto\n");
		}

	return console[cons]->cursore;

}

int  Console_read(void** session,int caratteri,char* buffer)
{
	int i,j,cons,curs;
	i=0;

	//Controllo su quale console si deve leggere
	cons = (int)*session;
	if(cons<0||cons>4)
	{
		kprint("Console_read: la console specificata non esiste\n");
		return -1;
	}

	if(cons==0)
	{
		//kprint("Console_read: console 0, ");
		curs = console[0]->cursore;
		//kprint("cursore a %d (prima), lungStrRoot=%d ",curs, lungStrRoot);

		for(i=curs,j=0;i<lungStrRoot&&j<caratteri;i++,j++)
		{
			buffer[i-curs] = console[0]->buffer[i];
			//kprint("i=%d,j=%d,buffer[i-curs]=%c,consbuffer[i]=%c\n",i,j,buffer[i-curs],console[0]->buffer[i]);
		}

		console[0]->cursore += j;
		//kprint("cursore a %d (dopo)\n",console[0]->cursore);
		//kprint("Contenuto var buffer: %s\n",buffer);
		return j;
	}

	//kprint(" fine read\n");

	//Console_getString(caratteri, buffer);
	return Console_getString(cons, caratteri, buffer);
}

int  Console_write(void** session,int caratteri,char* buffer)
{
	int cons,curs,i;
	char bfr[128];

	cons =-1;

	//Controllo dove si va a scrivere
	(void*)cons = *session;

	if(cons<0||cons>4)
	{
		kprint("Console_read: la console richiesta non esiste\n");
		return -1;
	}


	for(i=0;i<caratteri;i++)
	{
		bfr[i] = buffer[i];
	}
	bfr[i] = '\0';

	//kprint("%s %d\n",buffer,caratteri);

	//Prendo la posizione attuale del cursore nella console specificata
	curs = console[cons]->cursore;
	//Stampo la stringa passata

	Console_printStr(cons, bfr);

	return i;
}

bool Console_touch(char* nomefile)
{
	//non dovrebbe fare touch!!!
	return FALSE;
}

bool Console_remv(char* nomefile)
{
	//non bovrebbe fare remv
	return FALSE;
}
