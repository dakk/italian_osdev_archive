#include<interrupt.h>
#include<io.h>
#include<keyboard.h> //La includiamo per la mappa dei caratteri e altre dichiarazioni
#include<shell.h>

#define LSHIFT 0x2a 		 //scancode del tasto shift sinistro
#define RSHIFT 0x36 		 //scancode del tasto shift destro
#define ENTER 0x1c 		 //scancode del tasto Invio
#define SPACE 0x39  		 //scancode del tasto spazio
#define BACKSPACE 0xE   	 //scancode del tasto backspace
#define KEY_RELEASE 0X80  	 /*scancode generato al rilascio di un tasto. 0x80 in decimale è 128. Ricordo che se uno scancode è
			           maggiore o uguale a 128 significa che un tasto è stato rilasciato, altrimenti premuto */




unsigned char scancode; //Ci servirà per contenere lo scancode del tasto digitato
unsigned char key;      //Ci servirà per la conversione dallo scancode al carattere

static unsigned char *current_map = key_map; //current map contiene l' indirizzo della mappa dei caratteri

/********************************************************************************
********************************************************************************/
void initKBD(void)
{
Init_IDT_entry(handler_asm,0x21); //0x21 è la porta che identifica la tastiera ed associamo ad assa l' hander
abilita_irq(1,0x21);  		  //Abilitiamo l' irq della tastiera
pos=0; 				  //Inizializzo l'indice del buffer
}
/********************************************************************************
********************************************************************************/
void isr_kbd(void)
{
int i;

//Questo if mi serve per controllare se qualche altra funzione ha settato la variabile block_clear_status... Se è cosi vuol dire che
//la memoria video occupata fino a quel punto non può essere cancellata dallo backspace
if(block_clear_status)
{
block_video_mem = videomem;
block_clear_status = 0;
}


//se pos che utilizziamo come indice all' interno del buffer supera le dimensione del buffer stesso azzeriamo tale indice
if(pos>=BUF_SIZE)
pos=0;



scancode = inportb(0x60); //Assegno alla variabile 'scancode' lo scancode relativo al tasto premuto prelevabile dalla porta 0x60
disabilita_kbd(); //Ogni volta che si legge uno scancode bisogna disabilitare ed attivare la tastiera affinchè si possano
abilita_kbd();    //accettare gli eventuali successivi scancode
outportb(0x20, 0x20); /*Utilizziamo questa istruzione per informare il PIC del completamento dell' interrupt. Questa istruzione
			 viene denominata EOI(End Of Interrupt) */



if(scancode >= KEY_RELEASE) //Controllo se si tratta del rilascio di un tasto
{
	if(scancode == (KEY_RELEASE|RSHIFT) || scancode == (KEY_RELEASE|LSHIFT)) //Controllo se è stato rilasciato il tasto shift
	current_map = key_map; //In questo caso setto la current_map con la mappa dei caratteri minuscoli

	return;
}




 switch(scancode)
 {
 case RSHIFT:
 case LSHIFT:
  current_map = shift_map; /*Se è stato premuto lo shift(destro o sinistro) allora setto la current_map con la mappa dei
  			     caratteri maiuscoli */
break;

 case ENTER: //Se viene premuto enter
 newline();  //andiamo su una nuova riga
 shell(); //Richiamo la shell
 for(i=0;i<BUF_SIZE;i++) //inizializzo il buffer
 buffer[i]=0;
 pos=0;
break;

 case BACKSPACE:
if(videomem == block_video_mem ) //Faccio questo controllo per controllare se ci troviamo in un punto dove non si deve cancellare.
			         //Se non utilizzassimo questo if l' utente potrebbe cancellare tutta la memoria video.
break;

videomem-=2; //Vado indietro di un carattere. faccio videomem-=2 perchè bisogna contare anche il colore
caratteri_stampati--; //sono andato indietro di un carattere, quindi decremento la variabile caratteri_stampati
*videomem=' '; //stampo uno spazio in modo che sovrapponendosi al carattere che vogliamo cancellare si ha la sensazione della cancellazione
pos--;        //Decremento la variabile che utilizzo come indice nel buffer;
buffer[pos] = 0; //cancello l' ultimo carattere anche dal buffer oltre che dalla memoria video
break;


	default:

	/* Stampo a video il tasto digitato */
	key = current_map[scancode];
	putk(key);
	caratteri_stampati++;

buffer[pos] = key; //Memorizzo il tasto digitato in un buffer
pos++; //quindi incremento l' indice

 }


}
/********************************************************************************
********************************************************************************/
//Viene disabilita la ricezione degli scancode ponendo il bit 7 a 1 sulla porta 0x61
void disabilita_kbd(void)
{
unsigned int x;
x=inportb(0x61);
outportb(x|0x80, 0x61);
}
/********************************************************************************
********************************************************************************/
//Viene abilitata la ricezione degli scancode ponendo il bit 7 a 0 sulla porta 0x61
void abilita_kbd(void)
{
unsigned int x;
x=inportb(0x61);
outportb(x&0x7f, 0x61);
}
