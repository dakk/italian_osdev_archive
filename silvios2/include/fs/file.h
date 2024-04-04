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

#ifndef _FILE_H_
#define _FILE_H_

#include <mytypes.h>
#include <filesystem.h>



FILE *fopen(unsigned char *filename, struct directory * mydir, const char *mode);
struct entry * create(unsigned char *filename, struct directory *dir, unsigned int magic_code);
BOOL close(FILE *myfile);
BOOL print_File(FILE * file);
void rm_file(unsigned char *filename, struct directory * mydir);
void print_file_header(struct file_header *file);

#endif
