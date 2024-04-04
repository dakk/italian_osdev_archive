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

#ifndef _FS_H_
#define _FS_H_

#include <mytypes.h>

#define Kpwd(password) cripta_pwd(password, 6)

#define DISK_FULL 1000	

void fs_init();
char *get_label(void);
void set_label(unsigned char *new_label);
BOOL is_itfs_disk(void);
char *get_passwd(void);
void set_passwd(unsigned char *new_passwd);
unsigned int get_serial_number(void);
unsigned int get_fs_start(void);
void printf_disk_info(void);
unsigned char * cripta_pwd(unsigned char * pwd, unsigned int level);
BOOL get_fs_tree_initialized(void);
void write_fs(void);

#endif
