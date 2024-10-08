#include <cstddef>
#include <fat.h>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <debug.h>

namespace{

#define INDIRIZZO_TEXT(image)  indirizzo_sezione(image, 1)

void *image;
#define REAL_VALUE(X) (((dword)(X))-((dword)image))

typedef struct{
	byte magic[4]		__attribute__((__packed__)); //identificazione del file elf
	byte bitness		__attribute__((__packed__));	//quantitÃ  di bit a cui Ã¨ compilato
	byte endian		__attribute__((__packed__));//little o big endian
	byte elf_ver_1	__attribute__((__packed__));//versione dell'elf
	byte res[9]		__attribute__((__packed__));//inutili, utili solo ad allineare
	word file_type	__attribute__((__packed__));//tipo del binario
	word machine	__attribute__((__packed__));//tipo di architettura od os
	dword elf_ver_2	__attribute__((__packed__));//altra versione ma stavolta del file 
	dword entry_pt	__attribute__((__packed__));//inizio del file (entry point EIP)
	dword phtab_offset	__attribute__((__packed__));//c'Ã¨ l'offset dall'inizio del file in byte
																				//in cui inizia l'header
	dword shtab_offset	__attribute__((__packed__));//header della sezione sempre in byte
	dword flags		__attribute__((__packed__));//flag del programma
	word file_hdr_size	__attribute__((__packed__)); //dimensione dell'header ELF in byte
	word phtab_ent_size	__attribute__((__packed__));//dimensione di una entry dell'header dell'ELF
	word num_phtab_ents	__attribute__((__packed__));//numero di entry nel file
	word shtab_ent_size	__attribute__((__packed__));//dimensione in byte di una sezione
	word num_sects	__attribute__((__packed__));//numero di sezioni
	word shstrtab_index	__attribute__((__packed__));//al momento bho  :-(
} elf_file_t;

typedef struct{
	dword sect_name	__attribute__((__packed__));
	dword type		__attribute__((__packed__));
	dword flags		__attribute__((__packed__));
	dword virt_adr	__attribute__((__packed__));
	dword offset		__attribute__((__packed__));
	dword size		__attribute__((__packed__));
	dword link		__attribute__((__packed__));
	dword info		__attribute__((__packed__));
	dword align		__attribute__((__packed__));
	dword ent_size	__attribute__((__packed__));
} elf_sect_t;

typedef struct{
	dword addr		__attribute__((__packed__));
	dword info __attribute__((__packed__));
} elf_reloc_t;

typedef struct{
	char* nome;
	dword* addr;
	dword type;
	dword valore;
} oggetto;

typedef struct{
	dword name		__attribute__((__packed__));
	dword value		__attribute__((__packed__));
	dword size		__attribute__((__packed__));
	unsigned type : 4	__attribute__((__packed__));
	unsigned binding : 4	__attribute__((__packed__));
	byte zero		__attribute__((__packed__));
	word section	__attribute__((__packed__));
} elf_sym_t;

char *tipi[]= {"SHT_NULL",
						"SHT_PROGBITS",
						"SHT_SYMTAB",
						"SHT_STRTAB",
						"SHT_RELA",
						"SHT_HASH",
						"SHT_DYNAMIC",
						"SHT_NOTE",
						"SHT_NOBITS",
						"SHT_REL",
						"SHT_SHLIB",
						"SHT_DYNSYM",
						"SHT_LOPROC",
						"SHT_HIPROC",
						"SHT_LOUSER",
						"SHT_HIUSER"};


#define RELOC_BASE 0x10000

//restituisce il puntatore ad una sezione dentro il file, restituisce NULL se la sezione non Ã¨ presente
elf_sect_t* cerca_sezione(char* nome, void* image){
		elf_file_t *myfile;
		elf_sect_t* section;
		int i;
		char* nome_sec;
		myfile = (elf_file_t*)image;
		section = (elf_sect_t*)((byte*)image+myfile->shtab_offset);
		nome_sec = (char*)((byte*) image + section[myfile->shstrtab_index].offset);
		for(i=0; i < myfile->num_sects; i++)
			if(!strcmp(nome,  nome_sec + section[i].sect_name)) return section + i;
		return NULL;
}

void* indirizzo_sezione(void* image, size_t i){
		elf_file_t *myfile;
		elf_sect_t* section;
		myfile = (elf_file_t*)image;
		section = (elf_sect_t*)((byte*)image+myfile->shtab_offset);
		return ((byte*) image + section[i].offset);
}

dword info_sezione(void* image, size_t i){
	elf_file_t *myfile;
	elf_sect_t* section;
	myfile = (elf_file_t*)image;
	section = (elf_sect_t*)((byte*)image+myfile->shtab_offset);
	return section[i].info;
}

dword *indirizzo_simbolo(size_t i, void*image){
		elf_sym_t* simbolo = (elf_sym_t*)((byte*)image + cerca_sezione(".symtab",  image)->offset);
	//	printf("sezione 0x%X\n", simbolo[i].section);
		if(simbolo[i].section == 0xFFF1) //SHN_ABS
			return  NULL;
		if(simbolo[i].section == 0xFFF2) //SHN_ABS
			return (dword*)((byte*) image + simbolo[i].value);
		return  (dword*)((byte*)indirizzo_sezione(image, simbolo[i].section));
}

char* nome_sezione(void* image, size_t i, char* nome){
		elf_file_t *myfile;
		elf_sect_t* section;
		char* nome_sec;
		myfile = (elf_file_t*)image;
		section = (elf_sect_t*)((byte*)image+myfile->shtab_offset);
		nome_sec = (char*)((byte*) image + section[myfile->shstrtab_index].offset);
		strcpy(nome,  nome_sec + section[i].sect_name);
		return nome;
}

//restituisce un vettore mallocato dentro la funzione e il numero di sezioni rilocabili
size_t* elenco_rel(void* image, size_t* num_rel_sec){
		elf_file_t *myfile;
		elf_sect_t* section;
		int i;
		size_t* vett;
		myfile = (elf_file_t*)image;
		section = (elf_sect_t*)((byte*)image+myfile->shtab_offset);
		//nome_sec = (char*)((byte*) image + section[myfile->shstrtab_index].offset);
		*num_rel_sec = 0;
		for(i=0; i < myfile->num_sects; i++)
			if(section[i].type == 9)
				(*num_rel_sec)++;
		vett = (size_t*) malloc((*num_rel_sec+1) * sizeof(size_t));
		*num_rel_sec = 0;
		for(i=0; i < myfile->num_sects; i++)
			if(section[i].type == 9){
				vett[*num_rel_sec] = i;
				(*num_rel_sec)++;
			}
		return vett;
}


dword valore_simbolo(size_t i, void* image){
		//elf_sect_t* section = cerca_sezione(".symtab", image);
		elf_sym_t* simbolo = (elf_sym_t*)((byte*)image + cerca_sezione(".symtab",  image)->offset);
		return simbolo[i].value;
}


char* nome_simbolo(size_t i, void* image, char* str){
		//elf_sect_t* section = cerca_sezione(".symtab", image);
		elf_sym_t* simbolo = (elf_sym_t*)((byte*)image + cerca_sezione(".symtab",  image)->offset);
		char* strtab = (char*)((byte*)image + cerca_sezione(".strtab",  image)->offset);
		strcpy(str, strtab+simbolo[i].name);
		return str;
}

void stampa_simbolo(size_t i, void* image){

		char str[50];
		//elf_sect_t* section = cerca_sezione(".symtab", image);
		elf_sym_t* simbolo = (elf_sym_t*)((byte*)image + cerca_sezione(".symtab",  image)->offset);
		printf("%d) nome = %s valore = 0x%X  len = %d type=%d section = 0x%X\n", i, nome_simbolo(i, image, str), simbolo[i].value,   simbolo[i].size,   simbolo[i].type,    simbolo[i].section);
}


void stampa_oggetto(oggetto myobj){
	printf("nome = %s valore all'indirizzo= 0x%X  tipo=%d valore intrinseco=0x%X\n", myobj.nome, myobj.addr-(dword)image, myobj.type, myobj.valore-(dword)image);
	return;
}

oggetto cerca_simbolo_rilocabile(size_t indice, size_t sezione,  void* image){
	oggetto myobj;
	char str[50];
	elf_reloc_t* ogg_rel = (elf_reloc_t*) indirizzo_sezione(image, sezione);
	int use_sec =info_sezione(image, sezione) ;
	size_t sym_sect = ogg_rel[indice].info>>8;
	nome_simbolo(sym_sect, image,str);
	strlen(str);
	myobj.nome = (char*) malloc(15/*sizeof(char) * strlen(str)*/);
	strcpy(myobj.nome, str);
	myobj.addr =(dword*) ((dword)indirizzo_sezione(image, use_sec)  + ogg_rel[indice].addr);
	myobj.type = ogg_rel[indice].info & 0xFF;
	if(myobj.type == 1){
		myobj.valore = (dword)indirizzo_simbolo(sym_sect, image) + valore_simbolo(sym_sect, image);
	}
	else
		myobj.valore = (dword)indirizzo_sezione(image, 1) + valore_simbolo(sym_sect, image);
	return myobj;
}

size_t numero_oggetti_rilocabili(size_t sezione,  void* image){
	elf_file_t *myfile;
	elf_sect_t* section;
	myfile = (elf_file_t*)image;
	section = (elf_sect_t*)((byte*)image+myfile->shtab_offset);
	return  (section[sezione].size / section[sezione].ent_size);
}

void riloca(oggetto myobj){
		//stampa_oggetto(myobj);
	switch(myobj.type){
		case 1: /* S + A */
			*myobj.addr += myobj.valore;
		break;
	case 2: /* S + A - P */
		((dword*)myobj.addr)[0] +=  myobj.valore - (dword)(&((dword*)myobj.addr)[0]);
		break;
	}
	return;
}
int parser(char* nome_programma){
	elf_file_t *myfile;
	size_t i;
	size_t* vett_rel;
	size_t num_rel;
	size_t dim;
	image = malloc(0x10000);
	dim = carica_file(nome_programma, image);
	if(!dim){
		printf("\nNon esiste il file %s\n", nome_programma);
		return -1;
	}
	myfile = (elf_file_t*)image;
	/*controllo se Ã¨ un ELF*/
	if((myfile->magic[0] == 0x7F) && (myfile->magic[1] == 'E') && (myfile->magic[2] == 'L') && (myfile->magic[3] == 'F')){
		//printf("Eseguibile di tipo ELF\n");
	}else{
		printf("Formato non valido\n");
		return -2;
	}
	switch(myfile->bitness){
		case 0:
			printf("Oggetto non valido\n");
			return -3;
		case 1:
			//printf("Oggetto a 32 bit\n");
			break;
		case 2:
			//printf("Oggetto a 64 bit\n");
			break;
		default:
			printf("Oggetto di tipo sconosciuto (%d)\n", myfile->bitness);
			return -4;
	}

	switch(myfile->file_type ){
		case 0:
			printf("Nessun tipo\n");
			return -3;
		case 1:
		//	printf("file rilocabile\n");
			break;
		case 2:
			//printf("file eseguibile\n");
			break;
		case 3:
			//printf("file oggetto condiviso\n");
			break;
		default:
			printf("Oggetto di tipo sconosciuto (%d)\n", myfile->file_type);
			return -4;
	}
	vett_rel = elenco_rel(image, &num_rel);
	for(int k = 0; k < 	num_rel; k++){
		for(i=0; i < numero_oggetti_rilocabili(vett_rel[k], image); i++){
			riloca(cerca_simbolo_rilocabile(i, vett_rel[k], image));
		}
	}
	return 0;
}
}

bool exec(char* file_name, char* parametri){
	if(parser(file_name) != 0)
		return false;
	int (*program)(char* par) = (int(*)( char*))INDIRIZZO_TEXT(image);
	program(parametri);
	free(image);
	return true;
}
