#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#define byte unsigned char
#define word unsigned short int
#define dword unsigned int

char* convert_case(char* str){
	size_t i = 0;
	for(i=0; i < strlen(str); i++)
		str[i] = toupper(str[i]);	
	return str;
}

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

//il buffer viene mallocato all'interno della funzione
size_t carica_file(char* nome, void** buffer){
	FILE* program;
	size_t dim;
	byte* addr;
	if ((program = fopen(nome,"rb")) == NULL) {
		perror(nome);
		exit(-1);
    }
	fseek(program, 0, SEEK_END);
	dim = ftell(program);
	fseek(program, 0,SEEK_SET);
	*buffer = malloc(dim);
	fread(*buffer, dim, 1, program);
	return dim;
}

void chiudi_file(size_t dim, void* image){
	FILE* program;
	byte* addr;
	printf("\nDimensione = %d\n", dim);
	if ((program = fopen("output.bin","wb")) == NULL) {
		perror("output.bin");
		exit(-1);
    }
	fwrite(image, dim, 1, program);
}

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

dword *indirizzo_simbolo(size_t i, void*image){
		//elf_sect_t* section = cerca_sezione(".symtab", image);
		elf_sym_t* simbolo = (elf_sym_t*)((byte*)image + cerca_sezione(".symtab",  image)->offset);
		printf("sezione 0x%X\n", simbolo[i].section);
		if(simbolo[i].section == 0xFFF1) //SHN_ABS 
			return  NULL;
    	return  (dword*)((byte*)indirizzo_sezione(image, i))/* + simbolo[i].value)*/;
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

void stampa_nome_simbolo(FILE* file, size_t i, void* image){
		char str[50];
		//elf_sect_t* section = cerca_sezione(".symtab", image);
		elf_sym_t* simbolo = (elf_sym_t*)((byte*)image + cerca_sezione(".symtab",  image)->offset);
		fprintf(file, "#define __IT_%s__\n", convert_case(nome_simbolo(i, image, str)));
}

size_t  num_simboli(void* image){
		return cerca_sezione(".symtab",  image)->size / cerca_sezione(".symtab",  image)->ent_size;
}

int parser(char* nome_programma){
	elf_file_t *myfile;
	elf_sym_t* simbolo;
	size_t i;
	size_t* vett_rel;
	size_t num_sym;
	size_t dim;
	char* output_file_name;
	FILE* output_file;
	output_file_name = malloc(strlen(nome_programma)+20);
	strcpy(output_file_name, nome_programma);
	for(i=strlen(nome_programma); i > 0; i--)
		if(output_file_name[i] == '/'){
			output_file_name[i+1] = 0;
			break;
		}
	if(i==0)
		strcpy(output_file_name, "config_func.h");
	else
		strcat(output_file_name, "config_func.h");
	if((output_file = fopen(output_file_name, "w")) == NULL){
		perror("fopen: ");
		exit(-1);
	}
//	image = malloc(0x100000);
	dim = carica_file(nome_programma, &image);	
	myfile = (elf_file_t*)image;
	
	
	
	/*controllo se Ã¨ un ELF*/
	if((myfile->magic[0] == 0x7F) && (myfile->magic[1] == 69) && (myfile->magic[2] == 76) && (myfile->magic[3] == 70)){
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
	simbolo = (elf_sym_t*)((byte*)image + cerca_sezione(".symtab",  image)->offset);
	num_sym = num_simboli(image);
	for(i = 0; i < num_sym; i++){
		if(simbolo[i].type == 2)
			stampa_nome_simbolo(output_file, i, image);
		if(simbolo[i].type == 0)
			stampa_nome_simbolo(output_file, i, image);
	}
	fclose(output_file);
	return 0;
}
	

int main(size_t num, char** par){
	if(num != 2){
		printf("\nERRORE: paramentri non validi. Uso: %s nome_programma\n", par[0]);
		exit(-1);
	}
		
	parser(par[1]
	
	);
	return 0;
}
