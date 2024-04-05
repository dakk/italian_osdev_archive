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
#include <mem.h>
#include <stdio.h>
#include <fdc.h>
#include  <dir.h>

//commentare queste righe per abilitare i messaggi di debug
#undef debug
#define debug(string, ...)

void del_entry_to_tree(const char *name, struct directory *dir)
{
	struct entry *en;
	int pos = 0;
	en = dir->elem;
	while (en != en->next) {
		if (!strcmp(en->name, name)) {
			en = dir->elem;
			while (--pos)
				en = en->next;
			en->next = en->next->next;
			return;
		}
		pos++;
		en = en->next;
	}

	if (!strcmp(en->name, name)) {
		en = dir->elem;
		while (--pos)
			en = en->next;
		en->next = en;
	}
	return;
}

struct entry *add_entry_to_tree(const char *filename,
				struct directory *dir,
				unsigned int magic_code, unsigned int block)
{
	struct entry *file, *app;
	file = (struct entry *) malloc(sizeof(struct entry));
	file->magic_code = magic_code;
	file->first_block = block;
	strcpy(file->name, filename);
	debug("\nadd_entry_to_tree: nome: %s   first_block: %d    magic_code: %d nella cartella %s", file->name, file->first_block, file->magic_code, dir->elem->name);
	file->next = file;
	app = dir->elem;
	while (app != app->next)
		app = app->next;
	app->next = file;

	return file;
}

/*libera tutti i blocchi  occupati da una entry */
void free_entry_block(struct entry *en)
{
	unsigned char buffer[512];
	struct file_header header;
	unsigned char *pbuffer;
	unsigned int block;

	block = en->first_block;
	debug("\n\nfree_entry_block: Il blocco da liberare è %d", block);
	pbuffer = &buffer[0];
	read_block(block, (unsigned char *) pbuffer, 1);
	memcpy(&header, pbuffer, sizeof(struct file_header));
	free_block(block);
	debug("\nfree_entry_block: Sto liberando il  blocco %d", block);
	block = header.next_block;
	while (block != 0) {
		pbuffer = &buffer[0];
		read_block(block, (unsigned char *) pbuffer, 1);
		if (block != 0) {
			free_block(block);
			debug("\nfree_entry_block: Sto liberando il  blocco %d", block);
			memcpy(&block, pbuffer, sizeof(unsigned int));
			debug("\n\nfree_entry_block: Il blocco da liberare è %d", block);
		}
	}
	return;
}
