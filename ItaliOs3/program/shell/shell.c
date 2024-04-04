#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/io.h>
#include <sys/vt100.h>
#include <sys/exec.h>

#define NULL 0


void riavvia(){
	while (inportb(0x64) & 0x02);	// Riavvia il computer
	outportb(0x64, 0xfe);
	asm("cli;hlt");
	return;			/*non saràmai eseguito ma evita un warning ^___^  */
}

void aiuto(){
	printf("\nver = Visualizza la versione");
	printf("\nriavvia = Riavvia il computer");
	printf("\naiuto = Questo comando");
	printf("\ncanc = cancella lo schermo");
	printf("\nstampa = stampa a video una stringa");
	printf("\nlancia nome_file = esegue un programma");
	printf("\nesci = esce dalla shell\n");
	printf("\n");
}



#define PROMPT "ItaliOs3:> "
#define LUN_LINEA 250
#define LEN_COMANDO 20
#define LEN_PARAMETRO 50

static char riga[LUN_LINEA];

char *sh_gets(char *string);

short lenCommandHistory;

#define DIVERSO(str1, str2) strcmp(str1, str2)
#define UGUALE(str1, str2) !strcmp(str1, str2)

int main()
{
	char comando[LEN_COMANDO];
	char parametro[LEN_PARAMETRO];
	/* Qua' va implementato il caricamente dello storico dei comandi
	 * precedentemente salvati nella cartella personale dell'utente */
	/* Per adesso presuppongo che non vi sia alcun storico */
	printf("\nBenvenuto in ItaSh 0.3.\nPer informazioni digitare aiuto\n");
	while (1) {
		printf(PROMPT);
		sh_gets(riga);
		sscanf(riga, "%s %s", comando, parametro);
		if (UGUALE(comando, "")) {
			//              printf("\n");
			continue;
		}
		if (UGUALE(comando, "ver")) {
			printf("ItaliOs 0.3 www.italios.it\n");
			continue;
		}
		if (UGUALE(comando, "riavvia")) {
			riavvia();
			continue;
		}
		if (UGUALE(comando, "aiuto")) {
			aiuto();
			continue;
		}
		if (UGUALE(comando, "canc")) {
		  printf("\033[2J");
		  continue;
		}
		if (UGUALE(comando, "stampa")) {
		  printf("%s\n", parametro);
		  continue;
		}
		if (UGUALE(comando, "lancia")) {
			exec(parametro, riga);
			continue;
		}
		if (UGUALE(comando, "esci")) {
			return 0;
		}
		if (UGUALE(comando, "esporta")) {
			printf("devo esportare %s\n", parametro);
			return 0;
		}
		printf("Comando non valido: %s\n", comando);
	}
}


#define NUM_STO_COM 100
word maxNumber = 100;
struct command_history *firstCommand = NULL;	// quello che sto' scrivendo ora
struct command_history *thisCommand = NULL;	// quello che sto' visualizzando ora
struct command_history *lastCommand = NULL;

char *sh_gets(char *string){
	size_t pos;
	size_t i;
	// Se non c'e' lo storico lo creo
	if (firstCommand == NULL) {
		firstCommand =
			(struct command_history *) malloc(sizeof(struct command_history));
		firstCommand->comando = NULL;
		firstCommand->back = NULL;
		firstCommand->next = NULL;
		thisCommand = lastCommand = firstCommand;
		lenCommandHistory = 1;
	}
	// Altrimenti creo un altra struttura per il prossimo comando
	// TODO: mettere un limite al numero di comandi memorizzati
	else {
		if (lenCommandHistory > maxNumber) {
			if (lastCommand->comando != NULL)
				free(lastCommand->comando);
			lastCommand = lastCommand->back;
			free(lastCommand->next);
			lastCommand->next = NULL;
			lenCommandHistory = NUM_STO_COM;
		} else
			lenCommandHistory++;
		firstCommand->back =
			(struct command_history *) malloc(sizeof(struct command_history));
		firstCommand->back->next = firstCommand;
		firstCommand = firstCommand->back;
		firstCommand->back = NULL;
		firstCommand->comando = NULL;
		thisCommand = firstCommand;
	}
	dword count = 0;	// indica la posizione del prossimo carattere da scrivere
	int c = 0;		// e' il carattere da acquisire
	pos = get_scr_pos();

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
		if (c == FRECCIA_ALTA) {	// devo visualizzare il comando precedente
			/* Se il comando ora visualizzato corrisponde al primo, quello che sto' scrivendo 
			 * allora lo salvo cosi' quando torno indietro ritrovo quello che avevo gia' scritto */
			if (thisCommand == firstCommand) {
				if (count > 0) {
					if (firstCommand->comando != NULL) {
						free(firstCommand->comando);
					}
					firstCommand->comando =
						(char *) malloc((count + 1) *
								sizeof(char));
					string[count] = 0;
					strcpy(firstCommand->comando, string);
				} else {
					if (firstCommand->comando != NULL)
						free(firstCommand->comando);
					firstCommand->comando =
						(char *) malloc(1);
					firstCommand->comando = "";
				}
			}
			if (thisCommand->next != NULL) {
				thisCommand = thisCommand->next;
				strcpy(string, thisCommand->comando);
				dword len = count;
				count = strlen(string);
				for (i = 0; i < len; i++)
					printf("\b");
				printf(string);
			}
			continue;
		}
		if (c == FRECCIA_BASSA) {
			if (thisCommand->back != NULL) {
				thisCommand = thisCommand->back;
				dword len = count;
				strcpy(string, thisCommand->comando);
				count = strlen(string);
				for (i = 0; i < len; i++)
					printf("\b");
				printf(string);
			}
			continue;
		}
		thisCommand = firstCommand;
		string[count] = c;
		count++;
		printf("%c", c);
	}
	string[count] = 0;
	if (count == 0) {
		// Se non ho scritto niente e' inutile che salvi nello storico...
		thisCommand = firstCommand->next;
		if (firstCommand->comando != NULL)
			free(firstCommand->comando);
		free(thisCommand->back);
		firstCommand = thisCommand;
	} else {
		firstCommand->comando =
			(char *) malloc((count + 1) * sizeof(char));
		strcpy(firstCommand->comando, string);
	}
	return string;
}
