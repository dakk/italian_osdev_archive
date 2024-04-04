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

#include <mytypes.h>
#include <filesystem.h>
#include <dir.h>
#include <string.h>
#include <mem.h>
#include <fdc.h>
#include <stdio.h>
#include <time.h>
#include <magic_code.h>
#include <entry.h>

//commentare queste righe per abilitare i messaggi di debug
#undef debug
#define debug(string, ...)

/*restituisce un puntatore al file, dir rappresenta la directory nella quale si vuole mettere il file */
struct entry *create(unsigned char *filename, struct directory *dir,
		     unsigned int magic_code)
{
	struct entry *file;
	struct file myfile;
	unsigned char buffer[512];
	file = add_entry_to_tree(filename, dir, magic_code,
				 get_first_free_block());
	myfile.header = fill_header(magic_code);


	myfile.header.size = 0;

	if (dir->header.size <= 512) {


		memcpy(&buffer, &(myfile.header), sizeof(struct file_header));


//memcpy(&(buffer) + sizeof(struct file_header), myfile.dati,     512 - sizeof(struct file_header));


		write_block(file->first_block, (unsigned char *) &buffer, 1);


	} else {
		printf("\nFIle troppo grandi,  contattare Silvio abruzzo per far imkplementare i file + grandi di 512 byte");
	}

	return file;
}

int rm_file(unsigned char *filename, struct directory *mydir)
{
	struct entry *car;
	unsigned int block;
	car = mydir->elem;
	block = search_filename(filename, car);
	if (!block)
		return -1;
	while (car != car->next) {	//cerco esiste il file
		debug("\nrm_file: Sto analizzando il file %s", car->name);
		if (!strcmp(car->name, filename)) {
//esiste
			if (!strcmp(car->name, mydir->elem->name)) {
				printf("\nImpossibile eliminare la cartella di lavoro, uscirne e riprovare");
				return -2;
			}
			free_entry_block(car);
			del_entry_to_tree(filename, mydir);
			return 0;
		}
		car = car->next;
	}
	debug("\nrm_file:  Sto analizzando il file %s", car->name);
	if (!strcmp(car->name, filename)) {
//esiste
		if (!strcmp(car->name, mydir->elem->name)) {
			printf("\nImpossibile eliminare la cartella di lavoro, uscirne e riprovare");
			return -2;
		}
		free_entry_block(car);
		del_entry_to_tree(filename, mydir);
		return 0;
	}
//   printf("\nErrore non esiste file");
	return -1;
}


void print_file_header(struct file_header *file)
{
	printf("\n magic_code = %d", file->magic_code);
	printf("\nnascosto = %s", (file->p_nascosto == 0) ? "NO" : "SI");
	printf("\nPermessi dell'utente = %d", file->p_utente);
	printf("\nPermessi del gruppo = %d", file->p_gruppo);
	printf("\nAltri permessi = %d", file->altri);
	printf("\nPermesso di vuid = %d", file->vuid);
	printf("\nPermesso di guid = %d", file->guid);
	printf("\nId del creatore = %d", file->creatore);
	printf("\nId del proprietario = %d", file->proprietario);
	printf("\nId del gruppo = %d", file->gruppo);
	printf("\nCreazione: %d/%d/%d alle ore %d:%d:%d",
	       file->data_creazione.data.fields.dom,
	       file->data_creazione.data.fields.month,
	       (file->data_creazione.data.fields.year <
		2000) ? file->data_creazione.data.fields.year +
	       2000 : file->data_creazione.data.fields.year,
	       file->data_creazione.time.fields.hour,
	       file->data_creazione.time.fields.minute,
	       file->data_creazione.time.fields.second);
	printf("\nUltimo salvataggio: %d/%d/%d alle ore %d:%d:%d",
	       file->data_ultimo_salvataggio.data.fields.dom,
	       file->data_ultimo_salvataggio.data.fields.month,
	       (file->data_ultimo_salvataggio.data.fields.year <
		2000) ? file->data_ultimo_salvataggio.data.fields.
	       year +
	       2000 : file->data_ultimo_salvataggio.data.fields.
	       year,
	       file->data_ultimo_salvataggio.time.fields.hour,
	       file->data_ultimo_salvataggio.time.fields.minute,
	       file->data_ultimo_salvataggio.time.fields.second);
	printf("\nUltimo accesso = %d/%d/%d alle ore %d:%d:%d",
	       file->data_ultimo_accesso.data.fields.dom,
	       file->data_ultimo_accesso.data.fields.month,
	       (file->data_ultimo_accesso.data.fields.year <
		2000) ? file->data_ultimo_accesso.data.fields.
	       year +
	       2000 : file->data_ultimo_accesso.data.fields.year,
	       file->data_ultimo_accesso.time.fields.hour,
	       file->data_ultimo_accesso.time.fields.minute,
	       file->data_ultimo_accesso.time.fields.second);
	printf("\nBlocco successivo = %d", file->next_block);
	printf("\ndimensione = %d byte", file->size);
	return;
}

long ftell(FILE * file)
{
	return file->pos;
}


BOOL close(FILE * myfile)
{
	fflush(myfile);
//update_fs();
	free(myfile->dati);
	free(myfile);
	return TRUE;
}
