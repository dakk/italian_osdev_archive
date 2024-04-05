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

#include <mem.h>
#include <string.h>
#include <filesystem.h>
#include <fs.h>
#include <dir.h>
#include <magic_code.h>
#include <stdio.h>
#include <fdc.h>
#include <printer.h>
#include <rtc.h>
#include <file.h>
#include <entry.h>

//commentare queste righe per aggiungere i messaggi di debug
#undef debug
#define debug(string, ...)

#define MAX_LEN 30

struct dir_temp {
	unsigned int magic_code;
	unsigned int first_block;	/*se è una dir punta a se stesso, se è un file punta la primo blocco del file */
	unsigned char name[30];
};


extern struct fs_header my_disk;

struct directory *fs;

struct directory *get_fs_root()
{
	return fs;
}

/*cerca n blocco libero sul floppy e lo occupa. Restituisce il numero di questo blocco */
unsigned int get_first_free_block()
{
	int i, j;
	for (i = 0; i < LEN_VET_CLUSTER; i++) {
		for (j = 0; j < (sizeof(unsigned int) * 8); j++) {
			if (((my_disk.vet_cluster[i] >> j) % 2) == 0) {
				my_disk.vet_cluster[i] += 1 << j;

//write_fs();
				return j + (i * (sizeof(unsigned int) * 8)) +
					1;
			}
		}
	}
	return DISK_FULL;
}

/*libera un blocco che era precedentemente occupato */
void free_block(unsigned int block)
{
	int elemento = block / ((sizeof(unsigned int) * 8));
	int query = (block % ((sizeof(unsigned int) * 8))) - 1;
	my_disk.vet_cluster[elemento] -= 1 << query;
//write_fs();
	return;
}


/*scrive la cartella e la sua struttura su disco */
void write_dir(struct directory *mydir)
{
	struct dir_temp
		bufferd[((512 -
			  sizeof(struct file_header)) /
			 sizeof(struct dir_temp))];
	struct entry *car;
	unsigned char buffer[512];
	unsigned char *pbuffer = &buffer[0];
	int i = 0;
	car = mydir->elem;
	if (mydir->header.size <= 512) {
		memcpy(pbuffer, &(mydir->header), sizeof(struct file_header));
		pbuffer += sizeof(struct file_header) + 1;
		while (car != car->next) {
			bufferd[i].magic_code = car->magic_code;
			bufferd[i].first_block = car->first_block;
			strcpy(bufferd[i].name, car->name);
			debug("\nwrite_dir: %s ha il primo blocco a %d e il suo magic_code e' %d", bufferd[i].name, bufferd[i].first_block, bufferd[i].magic_code);
			i++;
			car = car->next;
		}
/*
 * copio l'ultimo elemento
 */
		bufferd[i].magic_code = car->magic_code;
		bufferd[i].first_block = car->first_block;
		strcpy(bufferd[i].name, car->name);
		debug("\nwrite_dir: %s ha il primo blocco a %d e il suo magic_code e' %d", bufferd[i].name, bufferd[i].first_block, bufferd[i].magic_code);

//mettendo il primo blocco a zero, quando devo leggere capisco che è l'ultimo elemento
		bufferd[i + 1].first_block = 0;



//copio il tutto in buffer
		memcpy(pbuffer, &bufferd, sizeof(bufferd));
		write_block(bufferd[0].first_block, &(buffer[0]), 1);
		debug("\nwrite_dir: Memorizzo la cartella nel blocco %d",
		      bufferd[0].first_block);
	} else {
		debug("\nwrite_dir: cartella troppo grandi,  contattare Silvio abruzzo per far imkplementare le cartelle + grandi di 512 byte");
	}
	return;
}



struct entry *search_block(unsigned int block, struct entry *dir)
{
	struct entry *car;
	car = dir;
	while (car != car->next) {
		if (car->first_block == block)
			return car;
		car = car->next;
	}
	return NULL;
}

/*dato un file cerca in che blocco è memorizzato questo file */
unsigned int search_filename(unsigned char *filename, struct entry *dir)
{
	struct entry *car;
	car = dir;
	while (car != car->next) {
		if (!strcmp(car->name, filename))
			return car->first_block;
		car = car->next;
	}
	if (!strcmp(car->name, filename))
		return car->first_block;
	return INESISTENTE;
}

/*stampa il conenuto di quella cartella */
void print_tree(struct directory *root)
{
	struct entry *dir;
	dir = root->elem;
	printf("\nBlocco iniziale\tnome\t\ttipo");
	while (dir != dir->next) {
		printf("\n%d          %s            %s", dir->first_block,
		       dir->name, (dir->magic_code ==
				   DIRECTORY) ? "CARTELLA" : "SCONOSCIUTO");
		dir = dir->next;
	}
	printf("\n%d          %s            %s", dir->first_block, dir->name,
	       (dir->magic_code == DIRECTORY) ? "CARTELLA" : "SCONOSCIUTO");
	return;
}

/*legge una cartella */
struct directory *read_dir(unsigned char *nome, struct directory *dire)
{
	struct dir_temp
		bufferd[((512 -
			  sizeof(struct file_header)) /
			 sizeof(struct dir_temp))];
	unsigned char buffer[512];
	unsigned char *pbuffer = &buffer[0];
	int block = search_filename(nome, dire->elem);
	int i;
	struct directory *dir;

	if (block == 0)
		return NULL;

	read_block(block, pbuffer, 1);
//aloco spazio necessario per la directory
	dir = (struct directory *) malloc(sizeof(struct directory));
//alloco il primo elemento della lista

	memcpy(&(dir->header), pbuffer, sizeof(struct file_header));
	pbuffer += sizeof(struct file_header) + 1;
	memcpy(&bufferd, pbuffer, sizeof(bufferd));
	dir->elem = add_dir(bufferd[0].name, bufferd[0].first_block);
	debug("\nHo aggiunto");
	i = 1;
	while (bufferd[i].first_block != 0) {
		debug("\nsto per entrare.........");
		add_entry_to_tree(bufferd[i].name, dir,
				  bufferd[i].magic_code,
				  bufferd[i].first_block);
		debug("\nread_dir: aggiungo all'albero %s ha il primo blocco a %d e il suo magic_code e' %d", bufferd[i].name, bufferd[i].first_block, bufferd[i].magic_code);

		i++;
	}
	return dir;
}



BOOL init_fs_tree(void)
{
	unsigned char buffer[512];
	unsigned char *pbuffer = &buffer[0];
	int i;
	struct dir_temp
		bufferd[((512 -
			  sizeof(struct file_header)) /
			 sizeof(struct dir_temp))];
	if (get_fs_tree_initialized()) {
		printf("\nL'albero delle directory esiste gia'");
		read_block(get_fs_start(), pbuffer, 1);
//aloco spazio necessario per la directory
		fs = (struct directory *) malloc(sizeof(struct directory));
//alloco il primo elemento della lista
		fs->elem = add_dir("Radice", my_disk.this_disk.fs_start);
		memcpy(&(fs->header), pbuffer, sizeof(struct file_header));
		pbuffer += sizeof(struct file_header) + 1;
		memcpy(&bufferd, pbuffer, sizeof(bufferd));
		i = 1;
		while (bufferd[i].first_block != 0) {
			add_entry_to_tree(bufferd[i].name, get_fs_root(),
					  bufferd[i].magic_code,
					  bufferd[i].first_block);
			i++;
		}
		return TRUE;
	}
	//è la prima volta che l'os viene avviato
	fs = create_root();
	return FALSE;
}


void close_fs()
{
	write_dir(get_fs_root());
	write_fs();
}



struct file_header fill_header(unsigned int magic_code)
{
	struct file_header header;
	header.magic_code = magic_code;
	header.p_nascosto = 0;
	header.p_utente = EXEC | WRITE | READ;
	header.p_gruppo = WRITE | READ;
	header.altri = READ;
	header.vuid = 0;
	header.guid = 0;
	header.open = 0;
	header.creatore = 1;
	header.proprietario = 1;
	header.gruppo = 1;
	strcpy(header.passwd, "");
	header.data_creazione.data = header.data_ultimo_salvataggio.data =
		header.data_ultimo_accesso.data = get_sys_date();
	header.data_creazione.time = header.data_ultimo_accesso.time =
		header.data_ultimo_salvataggio.time = get_sys_time();
	header.next_block = 0;
	header.size = 0;
	return header;
};

struct entry *add_dir(unsigned char *dir_name, int first_block)
{
	struct entry *root;
	root = (struct entry *) malloc(sizeof(struct entry));
	root->next = root;
	root->magic_code = DIRECTORY;
	strcpy(root->name, dir_name);
	root->first_block = first_block;
	debug("\nadd_dir: magic_code=%d, nome=%s, first_block=%d",
	      root->magic_code, root->name, root->first_block);
	return root;
}

struct directory *internal_mk_dir(unsigned char *dirname, unsigned int block)
{
	struct directory *mydir;

//alloco spazio necessario per la directory
	mydir = (struct directory *) malloc(sizeof(struct directory));
//alloco il primo elemento della lista
	mydir->elem = add_dir(dirname, block);
	mydir->header = fill_header(DIRECTORY);
	mydir->header.size = 0;
	return mydir;
}



/*restituisce un puntatore al file, dir rappresenta la directory nella quale si vuole mettere il file */
struct directory *mk_dir(unsigned char *dirname, struct directory *dir)
{
	struct directory *mydir;
	unsigned int block;

	block = search_filename(dirname, dir->elem);
	if (block != 0)
		return NULL;

	block = get_first_free_block();

	mydir = internal_mk_dir(dirname, block);
	add_entry_to_tree(dirname, dir, DIRECTORY, block);
	add_entry_to_tree("..", mydir, DIRECTORY, dir->elem->first_block);
	write_dir(mydir);
	write_dir(dir);
	return mydir;
}

/*crea la root, usata quando si crea il fs */
struct directory *create_root(void)
{
	struct directory *root;
	root = internal_mk_dir("Radice", my_disk.this_disk.fs_start);
	write_dir(root);
	return root;
}
