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

#include <fdc.h>
#include <stdio.h>
#include <filesystem.h>
#include <stdio.h>
#include <mem.h>
#include <mytypes.h>
#include <fs.h>

struct fs_header my_disk;


unsigned char *cripta_pwd(unsigned char *pwd, unsigned int level)
{
	unsigned char *new;
	unsigned char *ret;
	int i;
	new = kmalloc(sizeof(unsigned char) * (LEN_PASSWORD + 1));
	if (level == 0) {
		for (i = 0; i <= strlen(pwd); i++)
			*(new + i) = *(pwd + i);
		return new;
	} else {
		for (i = 0; i < strlen(pwd) - 1; i++)
			*(new + i) = *(pwd + i) + *(pwd + i + 1);
		*(new + i) = *(pwd + i) + *pwd;
		*(new + i + 1) = '\0';
		ret = cripta_pwd(new, level - 1);
		kfree(new);
		return ret;
	}
}


char *get_label(void)
{
	return my_disk.this_disk.nome_floppy;
}

void set_label(unsigned char *new_label)
{
	strcpy(my_disk.this_disk.nome_floppy, new_label);
	return;
}


unsigned int detect_first_free_block()
{
	int i, j;
	for (i = 0; i < LEN_VET_CLUSTER; i++) {
		for (j = 0; j < (sizeof(unsigned int) * 8); j++) {
			if (((my_disk.vet_cluster[i] >> j) % 2) == 0) {
				return j + (i * (sizeof(unsigned int) * 8));
			}
		}
	}
	return DISK_FULL;
}

BOOL get_fs_tree_initialized()
{
	return (my_disk.this_disk.fs_start ==
		detect_first_free_block())? FALSE : TRUE;
}


BOOL is_itfs_disk(void)
{
	return (!strcmp(my_disk.identificatore, "ITFS") ? TRUE : FALSE);
}

char *get_passwd(void)
{
	return my_disk.this_disk.password;
}

void set_passwd(unsigned char *new_passwd)
{
	strcpy(my_disk.this_disk.password, new_passwd);
	return;
}

unsigned int get_serial_number(void)
{
	return my_disk.this_disk.numero_serie;
}

unsigned int get_fs_start(void)
{
	return my_disk.this_disk.fs_start;
}

void printf_disk_info(void)
{
	if (is_itfs_disk()) {
		printf("\nRilevato un file system di tipo ITFS");
		printf("\nIl floppy ha come etichetta: %s", get_label());
		printf("\nLa password è: %s", get_passwd());
		printf("\nIl numero di serie del floppy è: %d",
		       get_serial_number());
		printf("\nIl descrittore del file system inizia al blocco %d",
		       get_fs_start());
	} else {
		printf("\nFile system sconosciuto, inserire un floppy con file system ITFS e riprovare, consultare l'help per maggiori informazioni");
	}
	return;
}


void getpasswd(unsigned char *string)
{
	unsigned char c = 0, count = 0;
	unsigned char tempstring[2];
	tempstring[1] = 0;
	while (1) {
		c = getch();
		if (c == 10)
			break;
		else if (c == 8 && count > 0)
			count -= 2;
		else if (c == 255 || c == 1)
			continue;
		else
			string[count] = c;
		tempstring[0] = '*';
		printf(tempstring);
		count++;
	}
	string[count] = '\0';
}


void fs_init()
{
	unsigned char cluster[512];
	unsigned char *newpasswd =
		malloc(sizeof(unsigned char) + LEN_PASSWORD);
	int flag = 0;
	if (read_block(1, (unsigned char *) &cluster, 1) == FALSE) {
		printf("Kernel Panic: il floppy non risponde");
		printf("Premi un tasto e prova ancora\n");
		getch();
		fs_init();
		return;
	}
	memcpy(&my_disk, &cluster, sizeof(my_disk));
	if (is_itfs_disk()) {
		if (my_disk.this_disk.password[0] != '\0') {
			do {
				printf("\nInserisci la password: ");
				getpasswd(newpasswd);
				if (!strcmp(Kpwd(newpasswd), get_passwd())) {
					printf("\nPassword valida");
					flag = TRUE;
				} else {
					printf("\nPassword non valida");
				}
			}
			while (flag == FALSE);
		}
		printf("\nITFS inizializzato");
	} else {
		printf("\nFile system sconosciuto. Inserisci un floppy con file system ITFS e premi un tasto");
		getch();
		fs_init();
	}

	return;
}

void write_fs()
{
	unsigned char buffer[512];
	memcpy(&buffer, &my_disk, sizeof(my_disk));
	write_block(1, (unsigned char *) &buffer, 1);
	return;
}
