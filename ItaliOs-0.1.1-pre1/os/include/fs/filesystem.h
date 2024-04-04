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

//prova di definizione della struttura del file system del silvios
#ifndef  _FILESYSTEM_H_
#define _FILESYSTEM_H_

#define VUOTO  0  /*indica se il cluster è libero*/
#define PIENO 1

#define MAX_CLUSTER  (80*18*2)
#define BYTE_PER_SECTOR 512
#define LEN_FLOPPY (MAX_CLUSTER*BYTE_PER_SECTOR)
#define LEN_IDENT 4
#define LEN_FLOPPY_NAME 10
#define LEN_PASSWORD 10
#define LEN_VET_CLUSTER ((LEN_FLOPPY/BYTE_PER_SECTOR)/(sizeof(unsigned int) * 8))
#define EXEC 1
#define WRITE 2
#define READ 4

#define FILE struct file

#include <rtc.h>

struct tempo{
	DATE data;
	TIME time;
};

struct file_option{
	unsigned int first_block;
};

struct file_header
{
	unsigned short int magic_code;/*numero che identifica la tipologia di file*/
	unsigned int p_nascosto:1;/*indica se il file è nascosto*/
	unsigned int p_utente:3;/*1 = esecuzione; 2 = scrittura; 4 = lettura*/
	unsigned int p_gruppo:3;/*1 = esecuzione; 2 = scrittura; 4 = lettura*/
	unsigned int altri :3;/*1 = esecuzione; 2 = scrittura; 4 = lettura*/
	unsigned int vuid  :1;/*very user id: tutti gli utenti possono utilizzare il file come il proprietario*/
	unsigned int guid  :1;/*group user id: tutti gli utenti possono utilizzare il file come il gruppo del proprietario*/
	unsigned int open  :1;/*indica se il file è aperto(bit = 1)*/
	unsigned int creatore;
	unsigned int proprietario;
	unsigned short int gruppo;
	unsigned char passwd [10];/*la passwd va criptata se tutti i char sono = 0 la passwd non c'è*/
	struct tempo data_creazione;/*numero di giorni passati dal 01/01/2000*/
	struct tempo data_ultimo_salvataggio;/*numero di giorni passati dal 01/01/2000*/
	struct tempo data_ultimo_accesso;/*numero di giorni passati dal 01/01/2000*/
	unsigned int next_block;  /*indica il blocco successivo del floppy, se vale zero il file è finito*/
	unsigned int size;
};

#define READ_ONLY 1  //1
#define WRITE_ONLY 2  //10
#define READ_WRITE 4  //100
#define APPEND 8 //1000
#undef EOF
#define EOF -1

struct file{
	struct file_header header;
	unsigned char *dati;
	/*struct file_option op*/unsigned int first_block;
	unsigned char mode;
	unsigned long pos;  //indica il numero di bytes nel quele è posizionqto il file
	unsigned int sz_buf; //indica qua ti caratteri ci sono ancora nel buffer di quel file
};

struct file_cluster{
	unsigned int next_block;
	unsigned char *dati;
};


struct entry{
	struct entry *next;
	unsigned int magic_code;
	unsigned int first_block;  /*se è una dir punta a se stesso, se è un file punta la primo blocco del file*/
	unsigned char name[30];
};


struct directory{
	struct file_header header;
	struct entry *elem; /*lista dove sono memorizzati tutti gli elementi della directory*/
};

struct disk_header{
	unsigned char nome_floppy[LEN_FLOPPY_NAME+1];
	unsigned char password[LEN_PASSWORD+1];
	unsigned int numero_serie;
	unsigned int fs_start; /*indica il blocco da dove iniziano di descrittori del file system*/
};

struct fs_header{
	unsigned char identificatore[LEN_IDENT+1]; /* "ITFS"*/
	struct disk_header this_disk;
	unsigned int vet_cluster[LEN_VET_CLUSTER];
};


#endif
