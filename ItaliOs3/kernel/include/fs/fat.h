#ifndef _FAT_H_
#define _FAT_H_

int strcasecmp(char *s,char *d);


void leggi_floppy(dword vpos, int lung,byte* buffer) ;

		
void init_fat();

/*Cerca se un file è presente sul floppy. Il paramentro pos è l'entry dir dove cercare un file. SI possono mettere tranquillamente anche path complicati. Ovviamente tutto deve essere assoluto rispetto alla dir puntata da pos. Restituisce 0 se il fuile non esiste. Restituisce  il cluster se il file esiste*/
int trova_file(char* nome, dword pos);

/*legge un file e lo memorizza nella memoria puntata da buff, restituisce la lunghezza in byte del file*/
dword leggi_file(int cluster, void* buff);

size_t carica_file(char* nome, void* buffer);

size_t dim_file(char* nome);

#endif
