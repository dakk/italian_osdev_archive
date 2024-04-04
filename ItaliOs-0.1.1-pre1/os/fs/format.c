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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/fs/filesystem.h"
#include "../include/fs/fs.h"

#undef FILE

#define SVUOTA_BUF while(getchar() != '\n')

#define ADDR (*(volatile long *) addr)


 void set_bit(int nr, volatile void * addr)
{
	__asm__(
		"btsl %1,%0"
		:"=m" (ADDR)
		:"Ir" (nr));
}

/*
   struct disk_header{
   unsigned char nome_floppy[LEN_FLOPPY_NAME+1];
   unsigned char password[LEN_PASSWORD+1];
   unsigned int numero_serie;
   unsigned int fs_start; //indica il blocco da dove iniziano di descrittori del file system
   }; */


/*struct fs_header{
   char identificatore[LEN_IDENT]; // "ITFS"
   struct disk_header this_disk;
   unsigned int vet_cluster[LEN_VET_CLUSTER];
   }; */

void printb(unsigned int d)
{
	char tmp[35];
	int i;

	if (d == 0) {
		putchar('0');
	} else {
		i = 0;
		tmp[0] = '0';
		while (d > 0) {
			tmp[i++] = '0' + (d & 1);
			d >>= 1;
		}
		for (i--; i >= 0; i--)
			putchar(tmp[i]);
	}
}

unsigned char *cripta_pwd(unsigned char *pwd, unsigned int level)
{
	unsigned char *new;
	unsigned char *ret;
	int i;
	new = malloc(sizeof(unsigned char) * (strlen(pwd) + 1));
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
		free(new);
		return ret;
	}
}


int main(int argc, char **argv)
{
	struct fs_header my_disk;
	int i = 0, j;
	char c;
	FILE *fd_image;
	unsigned char floppy[512];

	for (i = 0; i < 512; i++)
		floppy[i] = -1;

	srand((unsigned int) time(NULL));

	strcpy(my_disk.identificatore, "ITFS");

	for (i = 0; i < LEN_VET_CLUSTER; i++)
		my_disk.vet_cluster[i] = 0;

/*
 * printf("\nNumero di argomenti %d",argc);
 * for(i=0; i<argc; i++)
 * printf("\n%s", argv[i]); 
 */

	if (argc == 1) {
		printf("\nInserisci il nome del floppy: ");
		for (i = 0; i < LEN_FLOPPY_NAME; i++) {
			c = getchar();
			if (c != '\n')
				my_disk.this_disk.nome_floppy[i] = c;
			else
				break;
		}
		my_disk.this_disk.nome_floppy[i] = '\0';

		printf("\nVuoi inserire una password?[y][n] ");
		scanf("%c", &c);
		SVUOTA_BUF;
		if (c == 'y') {
			for (i = 0; i < LEN_PASSWORD; i++) {
				c = getchar();
				if (c != '\n')
					my_disk.this_disk.password[i] = c;
				else
					break;
			}
			my_disk.this_disk.password[i] = '\0';
			strcpy(my_disk.this_disk.password,
			       Kpwd(my_disk.this_disk.password));
		} else {
			for (i = 0; i < LEN_PASSWORD; i++)
				my_disk.this_disk.password[i] = '\0';
		}

//printf("\nLa passwd è: %s",my_disk.this_disk.password);

	} else {
		strcpy(my_disk.this_disk.nome_floppy, argv[1]);
		strcpy(my_disk.this_disk.password, argv[2]);
	}


	fd_image = fopen("len_kernel.txt", "r");
	fscanf(fd_image, "%d", &(my_disk.this_disk.fs_start));
	fclose(fd_image);

	my_disk.this_disk.fs_start += 3;	//aggiungo bootloader e fs

/*
 * my_disk.vet_cluster[1] = 444178;      //7 = 111 cioè i primi 3 bit settati 
 */

	my_disk.this_disk.numero_serie = rand();


	for(j=0; j<=my_disk.this_disk.fs_start; j++){
		set_bit(j,  my_disk.vet_cluster);		
	}
	

	fd_image = fopen("fs.disk", "wb");


	fwrite(&floppy, sizeof(char), 512, fd_image);
	fseek(fd_image, 0, SEEK_SET);
	fwrite(&my_disk, sizeof(my_disk), 1, fd_image);


	return 0;
}
