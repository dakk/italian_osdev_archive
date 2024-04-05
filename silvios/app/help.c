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

void help(void)
{
	printf("\n Ciao benvenuto nella guida in linea del ItaliOs: ");
	printf("\n Per salire e scendere dal terminare usare pagsu e paggiù ");
	printf("\n Comandi disponibili: ");
	printf("\n         aiuto = questo comando");
	printf("\n         ver = versione del ItalianOs");
	printf("\n         pulisci = pulisce lo schermo");
	printf("\n         ora = visualizza l'ora");
	printf("\n         data = visualizza la data");
	printf("\n         suona = Emette un suono all'altoparlante interno. Inserire quando richiesto la frequenza e il tempo in millisecondi.");
	printf("\n         riavvia = fa riavviare il pc ");
	printf("\n         colore_testo = imposta il colore del testo. ");
	printf("\n         colore_sfondo = imposta il colore dello sfondo");
	printf("\n         list_color = lista dei colori");
	printf("\n         infodisk = stampa a video le caratteristiche del floppy");
	printf("\n         list = stampa l'albero delle cartelle");
	printf("\n         crea_file = crea un file");
	printf("\n         leggi_file = legge un file");
	printf("\n         scrivi_file = scrive un file (ATTENZIONE: tutto il conenuto del file verra' perso)");
	printf("\n         aggiungi_file = aggiunge del testo in un file");
	printf("\n         info_file = da informazioni su di un file");
	//printf("\n         car_file = stampa un file sulla stampante parallela");
	printf("\n         cancella = cancella un file o una cartella");
	printf("\n         crea_car = crea una cartella");
	printf("\n         cc = cambia la cartella.");
	printf("\n         aggiorna_fs = fa' in modo che il file system sia completamente aggiornato su floppy");

	//printf("\n         parla = piccola chat via porta seriale per 2 pc ");
	printf("\n         tema = permette di impostare un tema della shell");
	printf("\n         ps = elenco dei processi correnti");
	printf("\n         indovina = piccolo gioco dove lo scopo è indovinare un numero");
	printf("\n         dice = gioco di dadi con la possibilità di scegliere il livello di diffocoltà e scommettere una somma");
	printf("\n Per altre informazioni goku_vegeta@libero.it");
	printf("\n");
	return;
}
