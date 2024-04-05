#include <fs_app.h>
#include <dir.h>
#include <magic_code.h>
#include <stdio.h>
#include <printer.h>
#include <mem.h>
#include <dir.h>
#include <file.h>
#include <string.h>


struct directory *current_dir;

void init_fs_app(void)
{
	current_dir = get_fs_root();
}

void crea(void)
{
	unsigned char nome[30];
//print_tree(get_fs_root());
	printf("\nInserisci il nome del file: ");
	getstring(nome);
	create(nome, current_dir, SCONOSCIUTO);
	return;
}

void leggi(void)
{
	FILE *file;
	unsigned char nome[30];
	int c;
	printf("\nInserisci il nome del file: ");
	getstring(nome);
	file = fopen(nome, current_dir, "r");
	if (file == NULL) {
		printf("\nERRORE: impossibile aprire il file %s", nome);
		return;
	}
	putchar('\n');
	while ((c = fgetc(file)) != EOF) {
//        printf("\nSto leggendo %d e il carattere è ", ftell(file)-1);
		putchar(c);
	}
	close(file);
	return;
}

void scrivi(void)
{
	FILE *file;
	unsigned char nome[30];
	unsigned char testo[10000];
	printf("\nInserisci il nome del file: ");
	getstring(nome);
	file = fopen(nome, current_dir, "w");
/*
 * if (file == NULL) {
 * printf("\nFile inesistente");
 * return;
 * } 
 */
	printf("\nInserisci il testo: massimo 10000 caratteri, si finsce con un invio\n");
	getstring(testo);
	fputs(testo, file);
	close(file);
	return;
}



void aggiungi(void)
{
	FILE *file;
	unsigned char nome[30];
	unsigned char testo[10000];
	int i;
	printf("\nInserisci il nome del file: ");
	getstring(nome);
	file = fopen(nome, current_dir, "a");
	if (file == NULL) {
		printf("\nFile inesistente");
		return;
	}
	printf("\nInserisci il testo: massimo 10000 caratteri, si finsce con un invio\n");
	getstring(testo);
	i = 0;
	while (testo[i] != '\0') {
		fputc(testo[i++], file);
	}
//getch();
//for(i = 0; i < file->header.size; i++)
//     putchar(file->dati[i]);
	close(file);
	return;
}


void riempi(void)
{
	FILE *file;
	unsigned char nome[30];
	unsigned char numero[50];
	int i;
	printf("\nInserisci il nome del file: ");
	getstring(nome);
	file = fopen(nome, current_dir, "w");
	for (i = 0; i < 1000; i++) {
		itoa(i, numero);
		//printf("\n%s", numero);
		fputs(numero, file);
		fputc(' ', file);
	}
	close(file);
	return;
}

void info(void)
{
	FILE *file;
	unsigned char nome[30];
	printf("\nInserisci il nome del file: ");
	getstring(nome);
	file = fopen(nome, current_dir, "r");
	print_file_header(&(file->header));
	close(file);
	return;
}

void stampa(void)
{
	FILE *file;
	unsigned char nome[30];
	unsigned char scelta;
	printf("\nPossiedi una stampante su porta parallela?[s][n] ");
	scelta = getch();
	if (scelta == 'n') {
		printf("\nMI dispaice non puoi stampare");
		return;
	}
	printf("\nInserisci il nome del file: ");
	getstring(nome);
	file = fopen(nome, current_dir, "r");
	printf("\nAccendi la stampante e premi un tasto: ");
	getch();
	reset_lp(1);
//print_File(file);
	close(file);
	return;
}


void elimina(void)
{
	unsigned char nome[30];
//print_tree(get_fs_root());
	printf("\nInserisci il nome del file: ");
	getstring(nome);
	rm_file(nome, current_dir);
	return;
}

void crea_dir(void)
{
	unsigned char nome[30];
//print_tree(get_fs_root());
	printf("\nInserisci il nome della cartella: ");
	getstring(nome);
	if (mk_dir(nome, current_dir) == NULL) {
		printf("\nErrore: impossibile creare la cartella");
		return;
	}
	return;
}

void cambia_dir(void)
{
	unsigned char nome[30];
	struct directory *temp_dir;
//print_tree(get_fs_root());
	write_dir(current_dir);
	printf("\nInserisci il nome della cartella: ");
	getstring(nome);
//if (strcmp(nome, "su")){
//   current_dir++;
//    if((get_fs_root() == current_dir)&& !strcmp(nome, current_dir->elem->name))
//   return;
	temp_dir = read_dir(nome, current_dir);
	if (temp_dir == NULL) {
		printf("\nERRORE: cartella inesistente");
		return;
	}
	current_dir = temp_dir;

//}else
//current_dir--;
	return;
}

void lista(void)
{
	print_tree(current_dir);
	return;
}
