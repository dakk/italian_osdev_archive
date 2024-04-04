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
#include <speaker.h>
#include <stdio.h>
#include <app.h>
#include <video.h>
#include <fs.h>
#include <dir.h>
#include <magic_code.h>
#include <fs_app.h>
#include <tema.h>
#include <vga.h>
#include <task.h>

void tu_scendi_dalle_stelle(void)
{
	struct note stelle;

	stelle.nota[1].frequenza = SOL3;
	stelle.nota[1].durata = CROMA;

	stelle.nota[2].frequenza = SOL3;
	stelle.nota[2].durata = SEMIMINIMA;

	stelle.nota[3].frequenza = LA3;
	stelle.nota[3].durata = CROMA;

	stelle.nota[4].frequenza = SOL3;
	stelle.nota[4].durata = SEMIMINIMA;

	stelle.nota[5].frequenza = FA3;
	stelle.nota[5].durata = SEMIMINIMA;

	stelle.nota[6].frequenza = FA3;
	stelle.nota[6].durata = CROMA;

	stelle.nota[7].frequenza = MI3;
	stelle.nota[7].durata = SEMIMINIMA * 2;

	stelle.nota[8].frequenza = RE3;
	stelle.nota[8].durata = CROMA;

	stelle.nota[9].frequenza = MI3;
	stelle.nota[9].durata = CROMA;

	stelle.nota[10].frequenza = FA3;
	stelle.nota[10].durata = CROMA;

	stelle.nota[11].frequenza = SOL3;
	stelle.nota[11].durata = CROMA;

	stelle.nota[12].frequenza = FA3;
	stelle.nota[12].durata = CROMA;

	stelle.nota[13].frequenza = MI3;
	stelle.nota[13].durata = CROMA;

	stelle.nota[14].frequenza = RE3;
	stelle.nota[14].durata = CROMA;

	stelle.nota[15].frequenza = MI3;
	stelle.nota[15].durata = CROMA;

	stelle.nota[16].frequenza = RE3;
	stelle.nota[16].durata = SEMIMINIMA;

	stelle.nota[17].frequenza = MI3;
	stelle.nota[17].durata = CROMA;

	stelle.nota[18].frequenza = FA3;
	stelle.nota[18].durata = SEMIMINIMA;

	stelle.nota[19].frequenza = MI3;
	stelle.nota[19].durata = CROMA;

	stelle.nota[20].frequenza = RE3;
	stelle.nota[20].durata = SEMIMINIMA;

	stelle.nota[21].frequenza = LA3;
	stelle.nota[21].durata = SEMIMINIMA;

	stelle.nota[22].frequenza = SOL3;
	stelle.nota[22].durata = CROMA;

	stelle.nota[23].frequenza = LA3;
	stelle.nota[23].durata = SEMIMINIMA;

	stelle.nota[24].frequenza = SOL3;
	stelle.nota[24].durata = CROMA;

	stelle.nota[25].frequenza = FA3;
	stelle.nota[25].durata = SEMIMINIMA;

	stelle.nota[26].frequenza = MI3;
	stelle.nota[26].durata = SEMIMINIMA;

	stelle.nota[27].frequenza = RE3;
	stelle.nota[27].durata = SEMIMINIMA;

	stelle.nota[28].frequenza = MI3;
	stelle.nota[28].durata = CROMA;

	stelle.nota[29].frequenza = MI3;
	stelle.nota[29].durata = CROMA;

	stelle.nota[30].frequenza = RE3;
	stelle.nota[30].durata = SEMIMINIMA;

	stelle.nota[31].frequenza = MI3;
	stelle.nota[31].durata = CROMA;

	stelle.nota[32].frequenza = FA3;
	stelle.nota[32].durata = SEMIMINIMA;

	stelle.nota[33].frequenza = MI3;
	stelle.nota[33].durata = CROMA;

	stelle.nota[34].frequenza = RE3;
	stelle.nota[34].durata = SEMIMINIMA;

	stelle.nota[35].frequenza = LA3;
	stelle.nota[35].durata = SEMIMINIMA;

	stelle.nota[36].frequenza = SOL3;
	stelle.nota[36].durata = CROMA;

	stelle.nota[37].frequenza = LA3;
	stelle.nota[37].durata = SEMIMINIMA;

	stelle.nota[38].frequenza = SOL3;
	stelle.nota[38].durata = CROMA;

	stelle.nota[39].frequenza = FA3;
	stelle.nota[39].durata = SEMIMINIMA;

	stelle.nota[40].frequenza = MI3;
	stelle.nota[40].durata = SEMIMINIMA;

	stelle.nota[41].frequenza = RE3;
	stelle.nota[41].durata = SEMIMINIMA;

	stelle.nota[42].frequenza = DO3;
	stelle.nota[42].durata = CROMA;

	stelle.num_note = 42;
	stelle.tempo = 80;
	musica(&stelle);


	return;
}


void inno_italia(void)
{
	struct note fratelli;

	fratelli.nota[1].frequenza = SOL3;
	fratelli.nota[1].durata = SEMIMINIMA;

	fratelli.nota[2].frequenza = SOL3;
	fratelli.nota[2].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[3].frequenza = LA3;
	fratelli.nota[3].durata = SEMICROMA;

	fratelli.nota[4].frequenza = SOL3;
	fratelli.nota[4].durata = SEMIMINIMA;

	fratelli.nota[5].frequenza = 0;
	fratelli.nota[5].durata = SEMIMINIMA;

	fratelli.nota[6].frequenza = MI4;
	fratelli.nota[6].durata = SEMIMINIMA;

	fratelli.nota[7].frequenza = MI4;
	fratelli.nota[7].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[8].frequenza = FA4;
	fratelli.nota[8].durata = SEMICROMA;

	fratelli.nota[9].frequenza = MI4;
	fratelli.nota[9].durata = SEMIMINIMA;


	fratelli.nota[10].frequenza = 0;
	fratelli.nota[10].durata = SEMIMINIMA;


	fratelli.nota[11].frequenza = MI4;
	fratelli.nota[11].durata = SEMIMINIMA;

	fratelli.nota[12].frequenza = SOL4;
	fratelli.nota[12].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[13].frequenza = FA4;
	fratelli.nota[13].durata = SEMICROMA;

	fratelli.nota[14].frequenza = MI4;
	fratelli.nota[14].durata = MINIMA;

	fratelli.nota[15].frequenza = RE4;
	fratelli.nota[15].durata = SEMIMINIMA;

	fratelli.nota[16].frequenza = MI4;
	fratelli.nota[16].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[17].frequenza = RE4;
	fratelli.nota[17].durata = SEMICROMA;

	fratelli.nota[18].frequenza = DO4;
	fratelli.nota[18].durata = SEMIMINIMA;

	fratelli.nota[19].frequenza = 0;
	fratelli.nota[19].durata = SEMIMINIMA;


	fratelli.nota[1 + 19].frequenza = SOL3;
	fratelli.nota[1 + 19].durata = SEMIMINIMA;

	fratelli.nota[2 + 19].frequenza = SOL3;
	fratelli.nota[2 + 19].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[3 + 19].frequenza = LA3;
	fratelli.nota[3 + 19].durata = SEMICROMA;

	fratelli.nota[4 + 19].frequenza = SOL3;
	fratelli.nota[4 + 19].durata = SEMIMINIMA;

	fratelli.nota[5 + 19].frequenza = 0;
	fratelli.nota[5 + 19].durata = SEMIMINIMA;

	fratelli.nota[6 + 19].frequenza = MI4;
	fratelli.nota[6 + 19].durata = SEMIMINIMA;

	fratelli.nota[7 + 19].frequenza = MI4;
	fratelli.nota[7 + 19].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[8 + 19].frequenza = FA4;
	fratelli.nota[8 + 19].durata = SEMICROMA;

	fratelli.nota[9 + 19].frequenza = MI4;
	fratelli.nota[9 + 19].durata = SEMIMINIMA;


	fratelli.nota[10 + 19].frequenza = 0;
	fratelli.nota[10 + 19].durata = SEMIMINIMA;


	fratelli.nota[11 + 19].frequenza = MI4;
	fratelli.nota[11 + 19].durata = SEMIMINIMA;

	fratelli.nota[12 + 19].frequenza = SOL4;
	fratelli.nota[12 + 19].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[13 + 19].frequenza = FA4;
	fratelli.nota[13 + 19].durata = SEMICROMA;

	fratelli.nota[14 + 19].frequenza = MI4;
	fratelli.nota[14 + 19].durata = MINIMA;

	fratelli.nota[15 + 19].frequenza = RE4;
	fratelli.nota[15 + 19].durata = SEMIMINIMA;

	fratelli.nota[16 + 19].frequenza = MI4;
	fratelli.nota[16 + 19].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[17 + 19].frequenza = RE4;
	fratelli.nota[17 + 19].durata = SEMICROMA;

	fratelli.nota[18 + 19].frequenza = DO4;
	fratelli.nota[18 + 19].durata = SEMIMINIMA;

	fratelli.nota[19 + 19].frequenza = 0;
	fratelli.nota[19 + 19].durata = SEMIMINIMA;


	fratelli.nota[39].frequenza = MI4;
	fratelli.nota[39].durata = SEMIMINIMA;

	fratelli.nota[40].frequenza = MI4;
	fratelli.nota[40].durata = SEMIMINIMA;

	fratelli.nota[41].frequenza = SI3;
	fratelli.nota[41].durata = MINIMA;

	fratelli.nota[42].frequenza = DO4;
	fratelli.nota[42].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[43].frequenza = RE4;
	fratelli.nota[43].durata = SEMICROMA;

	fratelli.nota[44].frequenza = DO4;
	fratelli.nota[44].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[45].frequenza = SI3;
	fratelli.nota[45].durata = SEMICROMA;

	fratelli.nota[46].frequenza = LA3;
	fratelli.nota[46].durata = SEMIMINIMA;

	fratelli.nota[47].frequenza = 0;
	fratelli.nota[47].durata = SEMIMINIMA;

	fratelli.nota[48].frequenza = DO4;
	fratelli.nota[48].durata = SEMIMINIMA;

	fratelli.nota[49].frequenza = SI3;
	fratelli.nota[49].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[50].frequenza = DO4;
	fratelli.nota[50].durata = SEMICROMA;

	fratelli.nota[51].frequenza = RE4;
	fratelli.nota[51].durata = SEMIMINIMA;

	fratelli.nota[52].frequenza = 0;
	fratelli.nota[52].durata = SEMIMINIMA;

	fratelli.nota[53].frequenza = MI3;
	fratelli.nota[53].durata = SEMIMINIMA;

	fratelli.nota[54].frequenza = MI4;
	fratelli.nota[54].durata = MINIMA;

	fratelli.nota[55].frequenza = FA4;
	fratelli.nota[55].durata = SEMIMINIMA;

	fratelli.nota[1 + 55].frequenza = SOL3;
	fratelli.nota[1 + 55].durata = SEMIMINIMA;

	fratelli.nota[2 + 55].frequenza = SOL3;
	fratelli.nota[2 + 55].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[3 + 55].frequenza = LA3;
	fratelli.nota[3 + 55].durata = SEMICROMA;

	fratelli.nota[4 + 55].frequenza = SOL3;
	fratelli.nota[4 + 55].durata = SEMIMINIMA;

	fratelli.nota[5 + 55].frequenza = 0;
	fratelli.nota[5 + 55].durata = SEMIMINIMA;

	fratelli.nota[6 + 55].frequenza = MI4;
	fratelli.nota[6 + 55].durata = SEMIMINIMA;

	fratelli.nota[7 + 55].frequenza = MI4;
	fratelli.nota[7 + 55].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[8 + 55].frequenza = FA4;
	fratelli.nota[8 + 55].durata = SEMICROMA;

	fratelli.nota[9 + 55].frequenza = MI4;
	fratelli.nota[9 + 55].durata = SEMIMINIMA;


	fratelli.nota[10 + 55].frequenza = 0;
	fratelli.nota[10 + 55].durata = SEMIMINIMA;


	fratelli.nota[11 + 55].frequenza = MI4;
	fratelli.nota[11 + 55].durata = SEMIMINIMA;

	fratelli.nota[12 + 55].frequenza = SOL4;
	fratelli.nota[12 + 55].durata = CROMA + PUNTO(CROMA);

	fratelli.nota[13 + 55].frequenza = FA4;
	fratelli.nota[13 + 55].durata = SEMICROMA;

	fratelli.nota[69].frequenza = MI4;
	fratelli.nota[69].durata = SEMIMINIMA + PUNTO(SEMIMINIMA);

	fratelli.nota[70].frequenza = SOL4;
	fratelli.nota[70].durata = CROMA;

	fratelli.nota[71].frequenza = RE4;
	fratelli.nota[71].durata = CROMA;

	fratelli.nota[72].frequenza = SOL4;
	fratelli.nota[72].durata = CROMA;

	fratelli.nota[73].frequenza = DO4;
	fratelli.nota[73].durata = SEMIMINIMA;

	fratelli.num_note = 73;
	fratelli.tempo = 120;
	musica(&fratelli);

	suicide();

	return;
}

unsigned char *del_init_spaces(unsigned char *string)
{
	while (*string == ' ')
		string++;
	return string;
}

extern struct directory *current_dir;

void shell()
{
	unsigned char tempstring[255];
	init_fs_app();

//fopen("storico.sh",current_dir,  "w");

//      add_task(inno_italia, "Inno di Mameli");
//tu_scendi_dalle_stelle();
	kprint("\nPer la lista dei comandi fare aiuto\n");
	while (1) {
		kprint("\nItaliOs: %s] /: ", current_dir->elem->name);
		getstring(tempstring);

		strcpy(tempstring, del_init_spaces(tempstring));

		if (!strcmp(tempstring, "")) {
			continue;
		}

		if (!strcmp(tempstring, "ver")) {
			print_version();
			continue;
		}


		if (!strcmp(tempstring, "aggiorna_fs")) {
			write_dir(current_dir);
			close_fs();
			continue;
		}



		if (!strcmp(tempstring, "data")) {
			date();
			continue;
		}


		if (!strcmp(tempstring, "ora")) {
			ora();
			continue;
		}
		if (!strcmp(tempstring, "riavvia")) {
			reboot();
			break;
		}

		if (!strcmp(tempstring, "pulisci")) {
			cls();
			continue;
		}


		if (!strcmp(tempstring, "suona")) {
			nota();
			continue;
		}

		if (!strcmp(tempstring, "colore_testo")) {
			text_colore();
			continue;
		}

		if (!strcmp(tempstring, "colore_sfondo")) {
			back_colore();
			continue;
		}

		if (!strcmp(tempstring, "list_color")) {
			list_color();
			continue;
		}

		if (!strcmp(tempstring, "aiuto")) {
			help();
			continue;
		}

		if (!strcmp(tempstring, "infodisk")) {
			printf_disk_info();
			continue;
		}

		if (!strcmp(tempstring, "list")) {
			lista();
			continue;
		}

		if (!strcmp(tempstring, "crea_file")) {
			crea();
			continue;
		}

		if (!strcmp(tempstring, "leggi_file")) {
			leggi();
			continue;
		}
		if (!strcmp(tempstring, "scrivi_file")) {
			scrivi();
			continue;
		}
		if (!strcmp(tempstring, "info_file")) {
			info();
			continue;
		}
		if (!strcmp(tempstring, "riempi")) {
			riempi();
			continue;
		}
		if (!strcmp(tempstring, "cancella")) {
			elimina();
			continue;
		}
		/*
		 * if (!strcmp(tempstring, "car_file")) {
		 * stampa();
		 * continue;
		 * } */
		if (!strcmp(tempstring, "tema")) {
			tema();
			continue;
		}
		if (!strcmp(tempstring, "vga")) {
			test_vga();
			continue;
		}
		if (!strcmp(tempstring, "crea_car")) {
			crea_dir();
			continue;
		}
		if (!strcmp(tempstring, "aggiungi_file")) {
			aggiungi();
			continue;
		}
		if (!strcmp(tempstring, "cc")) {
			cambia_dir();
			continue;
		}

		if (!strcmp(tempstring, "parla")) {
			chatta();
			continue;
		}

		if (!strcmp(tempstring, "ps")) {
			ps();
			continue;
		}

		if (!strcmp(tempstring, "indovina")) {
			indovina1();
			continue;
		}


		if (!strcmp(tempstring, "dice")) {
			dice();
			continue;
		}
		if (!strcmp(tempstring, "rete")) {
                   	rete();
			continue;
		}


/*
 * if(!strcmp(tempstring, "fstest")){
 * test_fs();
 * continue;
 * } 
 */
		printf("\n%s: comando sconosciuto, per l'elenco dei comandi digitare aiuto. Ritenta e sarai + fortunato", tempstring);
	}
	return;
}
