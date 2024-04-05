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

#ifndef _DIR_H_
#define _DIR_H_

#include <mytypes.h>
#include <filesystem.h>


#define INESISTENTE 0

struct entry *add_dir(unsigned char *dir_name, int first_block);
unsigned int get_first_free_block(void);
void write_dir(struct directory *mydir);
struct directory * get_fs_root(void);
struct directory *create_root(void);
struct entry * search_block(unsigned int block,  struct entry *dir );
unsigned int search_filename(unsigned  char* filename, struct entry *dir );
void print_tree(struct directory *root);
BOOL init_fs_tree(void);
struct  file_header fill_header(unsigned int magic_code);
void close_fs(void);
void free_block(unsigned int block);
struct directory *mk_dir(unsigned char *dirname, struct directory *dir);
struct directory *read_dir(unsigned char *nome, struct directory *dire);

#endif
