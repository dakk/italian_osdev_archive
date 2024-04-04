/*
  ItaliOs
  Brainfuck esteso di Michele Nardella (miki00)

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
#include <string.h>
#include <stdlib.h>
#include <sys/io.h>
#include <sys/vt100.h>
#include <sys/exec.h>


void aiuto() {
  printf("Sintassi: bf <programma>\n"); 
  printf(" A disposizione un array di unsigned char di 256 elementi: (da 0 a 255)\n");
  printf(" e un puntatore a questo array\n\n");

  printf("  Comandi Standard:\n");
  printf(" . :stampa il carattere contenuto nella variabile puntata\n");
  printf(" < :decrementa il puntatore\n");
  printf(" > :incrementa il puntatore\n");
  printf(" + :incrementa il valore della variabile puntata\n");
  printf(" - :decrementa il valore della variabile puntata\n");
  printf(" [ :inizio di un ciclo\n");
  printf(" ] :controlla la variabile puntata: se 0 continua, altrimenti salta a inizio ciclo\n\n");

  printf("  Comandi Estesi:\n");
  printf(" numero :indica quante volte si deve fare l'azione successiva\n");
  printf(" ;; :permette di inserire una stringa nell'array\n");
  printf(" u :stampa il contenuto della variabile puntata in decimale\n");
  printf(" n :stampa il valore del puntatore in decimale\n");
  printf(" # :resetta il valore del puntatore ( = 0 )\n\n");
  printf(" q :uscita\n");
/*
  printf(" Es: ./bf \"++++++++++++++++++++++++++++++++++++++++++++++++.\"\n");
  printf("     stampa a video 0\n");
  printf(" Es: ./bf \"48+.\"\n");
  printf("     stampa a video 0\n");*/
}



char *sh_gets(char *string)
{
	size_t i;
	dword count = 0;	// indica la posizione del prossimo carattere da scrivere
	int c = 0;		// e' il carattere da acquisire
	size_t pos = get_scr_pos();

	while (1) {
		c = getchar();
		if (c == BACKSPACE) {
			if (get_scr_pos() == pos)
				continue;
			printf("\b");
			if (count)
				count--;
			continue;
		}
		if (c == NEWLINE) {
			printf("\n");
			break;
		}
		string[count] = c;
		count++;
		printf("%c", c);
	}
	string[count] = 0;	
	return string;
}


int main() {
 unsigned char var[256];
 unsigned char ciclo[256];
 unsigned char nvar = 0,nciclo = 0;
 char *prog;
 int tt;
 int num = 0;
 int stringa = 0;
 prog = malloc(500);
 while (prog[0] != 'q') {
  printf("BF: >");sh_gets(prog);
  nvar = 0;
  nciclo = 0;
  num = 0;
  stringa = 0;
  for (tt = 0;tt < 256; tt++) var[tt] = 0;
  for (tt = 0;tt < 256; tt++) ciclo[tt] = 0;
  if (prog[0] == '?') aiuto();
  for (tt = 0; prog[tt]; tt++) {
     if (prog[tt] == ';') stringa = 1 - stringa; else
     if (prog[tt] == ':') stringa = 1 - stringa; else
     if (stringa) var[nvar++] = prog[tt]; else
     if ((prog[tt] >='0') && (prog[tt] <= '9')) num = num*10+prog[tt]-48; else
     {
        if (num == 0) num = 1;
/*        printf("nciclo=%u ciclo[nciclo]=%u\n",nciclo,ciclo[nciclo]); */
        switch (prog[tt]) {
          case '.' : printf("%c",var[nvar]); break;
          case 'n' : printf("[%u] ",nvar); break;
          case 'u' : printf("%u ",var[nvar]); break;
          case '+' : var[nvar] += num; break;
          case '-' : var[nvar] -= num; break;
        }
        switch (prog[tt]) {
          case '>' : nvar += num ; break;
          case '<' : nvar -= num ; break;
          case '#' : nvar = 0 ; break;
          case '[' : ciclo[nciclo++] = (unsigned char)tt; break; 
          case ']' : if (var[nvar]) tt = ciclo[nciclo-1]; else
                                    nciclo--;
                     break;
        }
/*        printf("tt = %u [%c]\t\t var [%u] = %u\t\t\t ciclo = %u\n",tt,prog[tt],nvar,var[nvar],ciclo);
*/
        num = 0;
     }
  }
 printf("\n"); 
 }
  free(prog); 
  return 0;
}

