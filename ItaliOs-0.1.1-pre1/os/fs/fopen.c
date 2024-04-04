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

#include <filesystem.h>
#include <stdio.h>
#include <magic_code.h>
#include <file.h>
#include <dir.h>
#include <mem.h>
#include <fdc.h>

//commentaare queste righe per aggiungere i messaggi di debug
#undef debug
#define debug(string, ...)

FILE *read_only_open(FILE * myfile, unsigned char *pbuffer)
{
	unsigned int block;
	int i;
	unsigned int cur_sz;
	unsigned char *buffer = pbuffer;

	buffer -= sizeof(struct file_header) - 1;
	if (myfile->header.size < 512) {
		myfile->dati = (unsigned char *) malloc(myfile->header.size);
		debug("\nfopen: Sto allocando %d bytes per i dati",
		      myfile->header.size);
		if (myfile->header.size != 0) {
			for (i = 0; i <= myfile->header.size; i++, pbuffer++) {
				myfile->dati[i] = *pbuffer;

			}
		}
	} else {
		debug("\nSto allocando %d bytes per i dati",
		      myfile->header.size);
		myfile->dati = (unsigned char *) malloc(myfile->header.size);
		cur_sz = 0;
		for (i = 0; i < 512 - sizeof(struct file_header) - 1;
		     i++, pbuffer++) {
			myfile->dati[i + cur_sz] = *pbuffer;

		}
		cur_sz = i;
		block = myfile->header.next_block;
		while (block != 0) {
//debug("\nl'indirizzo del buffer è %d, mentre quello di pbuffer è %d", buffer, pbuffer);
			pbuffer = buffer;
//debug("\nl'indirizzo del buffer è %d, mentre quello di pbuffer è %d", buffer, pbuffer);
			debug("\nfopen:  Sto leggendo il blocco %d", block);
			read_block(block, (unsigned char *) pbuffer, 1);
			if (block != 0)
				free_block(block);
			memcpy(&block, pbuffer, sizeof(unsigned int));
			pbuffer += sizeof(unsigned int);
			debug("\nfopen:  Il nuovo blokko è %d", block);
			if (block != 0) {
				for (i = 0; i <= 512 - sizeof(unsigned int);
				     i++, pbuffer++) {
					myfile->dati[i + cur_sz] = *pbuffer;

					//debug("\nSto leggendo il dato %d", i + cur_sz);
				}
				cur_sz += i - 1;
			} else {
				for (i = 0; i < myfile->header.size - cur_sz;
				     i++, pbuffer++) {
					myfile->dati[i + cur_sz] = *pbuffer;

					//debug("\nSto leggendo il dato %d  nell'ultimo blokko e devo arrivare a %d",                      i , myfile->header.size);
				}
			}
		}
	}

	myfile->pos = 0;
	return myfile;
}

FILE *append_open(FILE * myfile, unsigned char *pbuffer)
{
	myfile = read_only_open(myfile, pbuffer);
	myfile->pos = myfile->header.size;
	myfile->sz_buf = 0;
	return myfile;
}

FILE *fopen(unsigned char *filename, struct directory * mydir,
	    const char *mode)
{
	struct file *myfile = malloc(sizeof(struct file));
	unsigned int block;
	unsigned char buffer[512];
	struct entry *dir;
	unsigned char *pbuffer = &buffer[0];

//secondo lo standard se un file  aperto a sola scrittura, se esiste viene cancellato e ricreato
	if (!strcmp(mode, "w")) {	//file aperto in sola scrittura
		rm_file(filename, mydir);
		create(filename, mydir, SCONOSCIUTO);
		debug("fopen: file a sola scrittura creato");
		myfile->mode = WRITE_ONLY;
	}
	dir = mydir->elem;
	block = search_filename(filename, dir);
	if (block == INESISTENTE)
		return NULL;
	debug("\nfopen:  Sto leggendo il blocco %d", block);
	read_block(block, (unsigned char *) pbuffer, 1);
	myfile->first_block = block;

	memcpy(&(myfile->header), pbuffer, sizeof(struct file_header));

	pbuffer += sizeof(struct file_header) + 1;

	myfile->sz_buf = 1000;
	if (!strcmp(mode, "r")) {	//file aperto in sola lettura
		myfile = read_only_open(myfile, pbuffer);
		myfile->mode = READ_ONLY;
	}
	if (!strcmp(mode, "w")) {	//file aperto in sola scrittura
		myfile->dati = (unsigned char *) malloc(1000);
		myfile->mode = WRITE_ONLY;
		myfile->pos = 0;
	}
	if (!strcmp(mode, "a")) {	//file aperto in aggiunta
		myfile = append_open(myfile, pbuffer);
		myfile->mode = APPEND;
	}

	return myfile;
}
