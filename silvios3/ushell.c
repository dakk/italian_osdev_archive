/*
SilviOs

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
#include <ushell.h>
#include <stdio.h>
#include <speaker.h>

void help (void){
	kprint("\n Ciao benvenuto nella guida in linea del Silvios: ");
	kprint("\n Comandi disponibili: ");
	kprint("\n         help = questo comando");
	kprint("\n         ver = versione del SilviOs");
	kprint("\n         cls = pulisce lo schermo");
	kprint("\n         time = visualizza l'ora");
	kprint("\n         date = visualizza la data");
	kprint("\n         sound = fa suonare un suono Inseriserire quando richiesto la frequenza\n                            e il tempo in millisecondi.");
	kprint("\n         reboot = fa riavviare il pc ");
	kprint("\n         text_color = imposta il colore del testo. ");
	kprint("\n         back_color = imposta il colore dello sfondo");
	kprint("\n         list_color = lista dei colori");
	kprint("\n Per altre informazioni goku_vegeta@libero.it");
	kprint("\n");
	return;
}

void list_color(void){
	kprint("\n");
	kprint("\n               NERO\t\t0");
	kprint("\n               BLU\t\t1");
	kprint("\n               VERDE\t\t2");
	kprint("\n               CELESTE\t\t3");
	kprint("\n               ROSSO\t\t4");
	kprint("\n               VIOLA\t\t5");
	kprint("\n               MARRONE\t\t6");
	kprint("\n               BIANCO_SPORCO\t\t7");
	kprint("\n               GRIGIO\t\t8");
	kprint("\n               BLU_CHIARO\t\t9");
	kprint("\n               VERDE_CHIARO\t\t10");
	kprint("\n               CELESTINO\t\t11");
	kprint("\n               ARANCIONE\t\t12");
	kprint("\n               ROSA\t\t13");
	kprint("\n               GIALLO\t\t14");
	kprint("\n               BIANCO\t\t15");
	return;
}

/*void inno_italia(void){
	struct note fratelli;

	fratelli.nota[1].frequenza=RE;
	fratelli.nota[1].durata=QUARTO;

	fratelli.nota[2].frequenza=RE;
	fratelli.nota[2].durata=CROMA+PUNTO;

	fratelli.nota[3].frequenza=RE;
	fratelli.nota[3].durata=SEMICROMA;

	fratelli.nota[4].frequenza=RE;
	fratelli.nota[4].durata=QUARTO;

	fratelli.nota[5].frequenza=PAUSA;
	fratelli.nota[5].durata=QUARTO;

	fratelli.nota[6].frequenza=SI;
	fratelli.nota[6].durata=QUARTO;

	fratelli.nota[7].frequenza=SI;
	fratelli.nota[7].durata=CROMA;

	fratelli.nota[8].frequenza=SI;
	fratelli.nota[8].durata=SEMICROMA;


	fratelli.nota[9].frequenza=SI;
	fratelli.nota[9].durata=QUARTO;


	fratelli.nota[10].frequenza=PAUSA;
	fratelli.nota[10].durata=QUARTO;


	fratelli.nota[11].frequenza=SI;
	fratelli.nota[11].durata=QUARTO;
	fratelli.num_note = 11;
	musica(&fratelli);
	return;
	}*/

