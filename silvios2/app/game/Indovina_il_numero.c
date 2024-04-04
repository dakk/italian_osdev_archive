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
#include <video.h>

void indovina1(){

	int a, ris;
	unsigned int count = 1;
	unsigned char color = get_text_color();

	srand((unsigned)time(NULL));
	a=1+rand()%1000;

	set_text_color(ROSSO+LAMPEGGIO);
	printf("\n\n\t\tIndovina il numero 0.1\n\n");
	set_text_color(color);
	
	printf("\nIo ho un numero tra 1 e 1000   \n\n");
	printf("Vuoi conoscere il mio numero...\n\n");
	printf("Inserisci il tuo numero : ");
	ris = get_number();
		printf("\n");

	while(ris!=a){
		if (ris > a)
			printf("\nTroppo alto, ritenta\t");
		else
			printf("\nTroppo basso, ritenta\t");
		printf("\n\nInserisci il tuo numero : ");
		ris = get_number();
		printf("\n");
		count++;
	}

	if(ris==a)
		printf("!!! Bene hai trovato il numero !!! Ti ci sono voluti %d tentativi", count);
	    printf("\n\n");

	return;
}

