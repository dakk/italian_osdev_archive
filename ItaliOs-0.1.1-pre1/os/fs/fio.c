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
#include <fdc.h>
#include <mem.h>
#include <file.h>
#include <dir.h>
#include <entry.h>
#include <filesystem.h>
#include <magic_code.h>

int fgetc(FILE * file)
{
	if (file == NULL) {
//      printf("\nFile inesistente");
		return EOF;
	}
	if (file->header.magic_code == DIRECTORY) {
//      printf("\nErrore: impossibile leggere così in una cartella");
		return EOF;
	}
	if (file->mode == READ_ONLY) {
//debug("\nIl file è grosso %d e siano solo a %d",file->header.size, ftell(file));
		if (ftell(file) < file->header.size) {
			file->header.data_ultimo_accesso.data =
				get_sys_date();
			file->header.data_ultimo_accesso.time =
				get_sys_time();
			return (unsigned char) file->dati[file->pos++];
		} else		//significa che sono uguali
			return EOF;
	}
// printf("\nERRORE: FILE NON IN READ-ONLY MODE");
	return EOF;
}

int fputc(unsigned char c, FILE * file)
{
	if (file->header.magic_code == DIRECTORY) {
//printf("\nErrore: impossibile scrivere così in una cartella");
		return FALSE;
	}
	if (file->mode == WRITE_ONLY || file->mode == APPEND) {
		if (file->sz_buf == 0) {
			file->sz_buf = 1000;
			file->dati =
				realloc(file->dati,
					(sizeof(unsigned char) *
					 file->header.size) + file->sz_buf);
		}
		//debug("\nSto per scriver nella posizione %d con un massimo di %d",file->pos, file->header.size);
		//      putchar(c);
		file->dati[file->pos++] = (unsigned char) c;
		file->sz_buf--;
		file->header.size++;
		file->header.data_ultimo_accesso.data =
			file->header.data_ultimo_salvataggio.data =
			get_sys_date();
		file->header.data_ultimo_accesso.time =
			file->header.data_ultimo_salvataggio.time =
			get_sys_time();
		return 0;
	}
	return EOF;		//errore
}

int fputs(unsigned char *string, FILE * stream)
{
	//debug("\nSto scrivendo %s", string);
	while (*string)		//aspetto il '\0'
		fputc(*string++, stream);

	return 0;
}

void fflush(FILE * myfile)
{
	unsigned char buffer[512];
	unsigned int i;
	unsigned int block = myfile->first_block;
	unsigned int succ_block;
	unsigned char *pdata = &(myfile->dati[0]);
	unsigned int wr_size;
	/*for (i = 0; i < myfile->header.size; i++)
	 * putchar(myfile->dati[i]); */
	if (myfile->header.size <= (512 - sizeof(struct file_header))) {
		myfile->header.next_block = 0;	//indico cheil file è finito
		memcpy(&buffer, &(myfile->header),
		       sizeof(struct file_header));
		for (i = 0; i <= myfile->header.size; i++) {
			buffer[i + sizeof(struct file_header) + 1] = *pdata;
			pdata++;
		}
		write_block(block, (unsigned char *) &buffer, 1);
	} else {
		block = myfile->header.next_block = get_first_free_block();	//imposto il blocco successivo
//      debug("\nclose:   Il nuovo blokko successivo e' : %d", block);
		memcpy(&buffer, &(myfile->header),
		       sizeof(struct file_header));
//imposto il primo blocco, quello che ha l'header del file
		for (i = 0; i < (512 - sizeof(struct file_header)) - 1; i++) {
			buffer[i + sizeof(struct file_header) + 1] = *pdata;
			pdata++;
		}
		write_block(myfile->first_block, (unsigned char *) &buffer,
			    1);
//debug("\nclose:    Sto scrivendo nel blocco %d",    myfile->first_block);
		wr_size = (512 - sizeof(struct file_header));
//ora ciclo impostando tutti i blocchi tranne l'ultimo
		while (wr_size + (512 - sizeof(unsigned int)) <
		       myfile->header.size) {
			succ_block = get_first_free_block();
//debug("\nclose:   Il nuovo blokko successivo e' : %d",  succ_block);
			memcpy(&buffer, &succ_block, sizeof(unsigned int));
			for (i = 0; i < (512 - sizeof(unsigned int)); i++) {
				buffer[i + sizeof(unsigned int)] = *pdata;
				pdata++;
			}
			write_block(block, (unsigned char *) &buffer, 1);
//debug("\nclose:  Sto scrivendo il blocco %d", block);
			block = succ_block;
			wr_size += 512 - sizeof(unsigned int);
		}
		i = 0;
//debug("\nclose:  l'ultimo blocco  e' : %d", i);
		memcpy(&buffer, &i, sizeof(unsigned int));
		for (i = 0; i <= myfile->header.size - wr_size; i++) {
			buffer[i + sizeof(unsigned int)] = *pdata;

			pdata++;
		}

		write_block(block, (unsigned char *) &buffer, 1);
//debug("\nfflush:   Sto scrivendo il blocco %d", block);
	}
//    write_fs();
	return;
}

int fseek(FILE * stream, long offset, int dove)
{
	if (dove == SEEK_SET) {
		stream->pos = offset;
		if (stream->pos > stream->header.size)
			return -1;	//errore, la nuova posizione è fuori il file
		return 0;
	}
	if (dove == SEEK_END) {
		stream->pos = stream->header.size - offset;
		if (stream->pos < 0)
			return -1;	//errore, la nuova posizione è fuori il file
		return 0;
	}
	if (dove == SEEK_CUR) {
		stream->pos += offset;
		if (stream->pos < 0 || stream->pos > stream->header.size)
			return -1;	//errore, la nuova posizione è fuori il file
		return 0;
	}
	return -1;
}
