#include<io.h>


char *addr_videomem = (char *) 0xb8000;	/*da dove inizia la memoria della scheda video*/
int righe=0;
int spazi_stampati;
char *videomem_max_in_una_schermata;

colore=0x7;


/********************************************************************************
********************************************************************************/
void putk(char carattere){
caratteri_stampati_assoluti++; //incremento la variabile

if(caratteri_stampati == SCR_C) //Controllo se sono stati stampati SCR_C caratteri
{
caratteri_stampati = 0;        //Se � cosi azzero il numero dei caratteri stampati
righe++;		      //Se sono stati stampati SCR_C caratteri vuol dire che ci troviamo in una nuova riga :)
scroll();
}

	*videomem++ = carattere;	/*imposto il carattere*/
	*videomem++ = colore;           /*imposto gli attributi del carattere*/

}
/********************************************************************************
********************************************************************************/
void printk(char *stringa)
{

	int i = 0;

 while(stringa[i] != 0)  {

    if(stringa[i] == '\n')
	newline();
	
    else if(stringa[i] == '\t')
 	printab();

     else {
	putk(stringa[i]);
 	caratteri_stampati++; //ad ogni carattere stampato incremento la variabile 'caratteri_stampati'
	  }
	

i++;

			} //chiude il while

} //chiude la funzione
/********************************************************************************
********************************************************************************/
//INIZIO: newline
//Stampa tanti spazi quanto � grande la riga dello schermo impostata nella costante SCR_C
void newline(void)
{

spazi_stampati = caratteri_stampati; /*se uno /n viene invocato alla fine stringa (ad esempio) la routine 
				       deve stampare 'SCR_C - caratteri_stampati' di spazi. Pi� semplicemente: 
				       deve stampare gli spazi partendo dalla posizione dell' ultimo carattere 
				       stampato*/

while(spazi_stampati != SCR_C) {


putk(' ');    //viene stampato uno spazio
spazi_stampati++;

}

caratteri_stampati=0; //una volta andati su una nuova riga azzero i caratteri stampati
righe++;	     //Bhe se invochiamo lo /n ci troviamo su una nuova riga e quindi incremento la variabile
scroll();

}
//FINE: newline
/********************************************************************************
********************************************************************************/
//INIZIO: klear
//Cancella lo schermo stampando tanti spazi

void klear(void)
{
init_videomem(); //inizializzo tutto
int spazi_da_stampare = SCR_C * SCR_R * 2; //mi calcolo quanti spazi devo stampare. Ho moltiplicato per due perch� bisogna tenere conto anche del byte del colore

while(spazi_da_stampare != 0) {

putk(' '); //viene stampato uno spazio
spazi_da_stampare--; //decremento la variabile fin quando non � 0

}
videomem = (char *) 0xb8000; //alla fine ripristino il valore di videomem
}
//FINE: klear
/********************************************************************************
********************************************************************************/
//INIZIO: printab
//Simula il tab

void printab(void)
{
int dim_tab = DIM_TAB;

  while(dim_tab != 0) {

   putk(' '); //viene stampato uno spazio
   dim_tab--;
   caratteri_stampati++;

  }


}
//FINE: printab
/********************************************************************************
********************************************************************************/
//INIZIO: scroll
//Gestisce lo scroll dello schermo nel caso in cui le righe stampate sono piu di quelle che pu� contenere in una schermata

void scroll(void)
 {

if(righe >= SCR_R)   //controllo se le righe stampate sono maggiori o uguali a quelle che pu� contenere una schermata-video
  {
 char *i;
 spazi_stampati = 0;
 videomem_max_in_una_schermata = addr_videomem + (SCR_R * SCR_C * 2); //Abbiamo moltiplicato per due perch� bisogna contare anche il byte del colore

      for (i=addr_videomem; i <= videomem_max_in_una_schermata; i++) //Scorro tutta la memoria video di una scheramata
      *i = i[SCR_C * 2]; //Copio man mano tutta la memoria video di una schermata partendo dalla seconda riga(la prima viene saltata)

 videomem = videomem - (SCR_C * 2); //Ora che c' � una riga in meno bisogna inizializzare opportunamente videomem con una riga in meno
	
 --righe; //Quindi decremento le righe


//Il codice sottostante serve per colorare il background del testo in modo da uniformarsi con il resto dello schermo
  while(spazi_stampati != SCR_C) {

	*videomem++ = ' ';	/*imposto il carattere*/
	*videomem++ = colore;           /*imposto gli attributi del carattere*/
	spazi_stampati++;

}

videomem = videomem - SCR_C * 2;
	
   }
	}
//FINE: scroll
/********************************************************************************
********************************************************************************/
void printok(void)
{

int distanza = (SCR_C - caratteri_stampati);
spazi_stampati = 0;

while(spazi_stampati < distanza)
{
putk(' ');
spazi_stampati++;
caratteri_stampati++;
}
if(distanza > 5)
{
videomem -= 10; /*videomem sposta il cursore all' indietro di 5 caratteri. Ricordare sempre che bisogna tener conto anche del
		 colore; ecco perch� videomem -= 10;*/
caratteri_stampati -= 5;  //allora noi togliamo cinque caratteri dalla variabile 'caratteri_stampati'
}

printk("[OK]\n");

//Ripristino tutto

}
/********************************************************************************
********************************************************************************/
void init_videomem(void)
{
videomem=(char *)0xb8000;
caratteri_stampati = 0;
righe=0;
}

