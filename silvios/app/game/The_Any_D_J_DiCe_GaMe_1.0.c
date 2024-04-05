/*
ItaliOs
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void help(void);
int diff();

int dice(){

	int a, b, val,val2,ris, ret=0, sot,capitale=5000,sw,capatt=0,nc=capitale;

	printf("\n\nThE DicE GaMe V 1.0 --* By Any_D_J *-- ThE DicE GaMe V 1.0\n\n");
	printf("\a\a!!!Benvenuto al gioco dei dadi!!!\n\n");
	printf("Hai un capitale di 5000 $ da scommetere.\n\n");
	printf("Inserisci -1 per uscire\n\n");
	printf("Premi :\n1.Help\n2.Impostazioni\n3.Inizia il gioco\n");
	sw = get_number();
	
	if(sw==-1){
	printf("Sei uscito dal programma... A presto\n\n");
	return 0;}

	printf("\n\n");
	
	while(sw>3||sw==0){
		printf("\a\a!!! Inserire un numero compreso tra 1 e 3 : ");
			sw = get_number();
	printf("\n");}
	

	switch(sw){
	case 1:
		help();
	break;
	case 2:
	ret=diff();
		break;}
	if(ret==0){
		printf("Imposta la dificolta' : \n\n");
		ret=diff();}
		
		
	srand((unsigned)time(NULL));

	

	while(capatt!=0){
		
		a=1+rand()%6;
	    b=1+rand()%6;

		if(capatt==0)
			break;

	printf("Inserisci la somma da scommettere sul lancio : ");
	sot = get_number();
   
	if(sot==-1){
		printf("Sei uscito dal programma... A presto\n\n");
		break;}

	if(ret==1){
    val=(a+b)-4;
    val2=(a+b)+4;
printf("Il risultato e' compreso tra %d e %d\n",val,val2);}

	if(ret==2){
       if((a+b)%2==0)
		   printf("\a!!! Io proverei con un numero pari !!!\n");
	   else
		   printf("\a!!! Io proverei con un numero dispari !!!\n");}

	printf("\n\n");
	printf("Inserisci il risultato che prevedi : ");
	ris = get_number();
	printf("\n\n");

	
	
	if(ris!=a+b){
		
	capatt=nc-sot;
	nc=capatt;
	printf("Sbagliato...VINCE IL BANCO\n\n");
	printf("Il risultato della somma dei dadi e' : %d",a+b);
	printf("\n");
	printf("Il tuo capitale ora e' di : %d\n\n",capatt);}
	
	
	
	if(ris==a+b){

	capatt=nc+sot;
	nc=capatt;
	printf("\aIncredibbbbbile.... HAI VINTO\n\n");
	printf("Il risultato della somma dei dadi e' proprio : %d",a+b);
	printf("\n");
	printf("Il tuo capitale ora è di : %d\n\n",capatt);}

	}
	
	if(capatt==0)
	printf("\a\a!!!! NOOOOOOOOOO HAI PERSO TUTTO :(\n\n");
	return 0;} 
	
void help(){
	printf("Questo programma calcola la somma di un lancio di \n"
		   "due dadi. Ricorda che la somma delle faccie di un \n"
		   "quadrato e' compresa tra 2 e 36. Se perderai tutto\n"
		   "il capitale a disposizione (5000 $) avrai perso.\n\n");
   
}

int diff(){
	int ld;
	printf("Scegli il livello di difficolta' : \n");
	printf("1.Facile\n2.Medio\n3.Difficile\n");
	ld = get_number();
	while(ld>3||ld==0){
		printf("Inserisci un valore compreso tra 1 e 3 : ");
		ld = get_number();
		printf("\n");}
	return ld;
	
}

