 /*
 * Dreamos
 * fismem.c
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

 /*
  * Autore Ivan Gualandri
  * Prima versione: 21/11/2003
  * Thanks to: Silvio Abruzzo
  */

#include <fismem.h>
#include <io.h>
#include <8259.h>
#include <video.h>



/*-----------------Dichiarazione variabili-------------------------------*/
mem_area_pointer mem_list = (mem_area_pointer)0x100000;
mem_area_pointer mem_list_root  = (mem_area_pointer)0x100000;

size_t total_mem;                            /*La memoria totale calcolata*/
mem_info free_pages;                    /*Struttura che contiene info sulle pagine libere.*/

unsigned int pos_mem;                   /*Dovrebbe puntare alla prima locazione di memoria dispinibile.*/
int max_pages;                              /*Numero Massimo di pagine disponibili*/
int max_vet;                                  /*Numero di elementi del vettore.*/

/*--------------------------------------------------------------------------------------------------------*/

void mem_size()
{
/*Calculate the size of RAM, try 2 write and read in memory, if i read the same value, the location*/
/*exists, else, i will stop. I use a while(TRUE) cycle.*/
    unsigned int *mem_pointer;

    mem_pointer = (unsigned int *) BASE_MEM;    /*Faccio partire mem_pointer da 1Mb*/

    outportb(0xFF, MASTER2_PIC);                     /*Disabilito gli interrupt hardware*/
    outportb(0xFF, SLAVE2_PIC);

    *mem_pointer = VALUE_MEM;                      /*Assegno a quella locazione di memoria un valore*/
    while(1)
    {
    /*Try write - read in mermory*/
        if(*mem_pointer == VALUE_MEM)
        {
            *mem_pointer=~VALUE_MEM;
            asm("": : :"memory");
            if((*mem_pointer==ANTI_VALUE_MEM))
            {
                mem_pointer = mem_pointer + OFFSET_MEM;
                *mem_pointer = VALUE_MEM;
                asm("": : :"memory");
            }
            /*Location doesn't exists*/
            else break;
        }
        else break;

        asm("": : :"memory");
    }
/*Update the value of total_mem*/
total_mem = (unsigned long) mem_pointer;
}

/*----------------------------------------------------------------------------------------------*/

void init_pages()
{
    int i=0;
    /*Inizializza a 0 i 32 bit della bitmap di memoria.*/
    /*The first 256 pages are reserved to the kernel, then i will*/
    /*set first 8 element of array to FFFFFFFF (every element is 32bit).*/
    while (i<SIZE_VETT)
    {
        if(i<8)
            free_pages.mem_map[i] = 0xFFFFFFFF;  /*Sto nell'area kernel*/
        else
            free_pages.mem_map[i] = 0x00000000;   /*Il resto dello spazio e' da segnare come libero*/
        i++;
    }


    /*Al primo elemento della strutturina assegno lo spazio del kernel*/
    mem_list->pages_info[0].inizio = 0x00000000;
    mem_list->pages_info[0].num_pages = 256;
    mem_list->next_free = 1; /*Prossimo elemento del vettore libero.*/
    mem_list->next = NULL;  /*Puntatore alla prossima struttura.*/

    max_pages = total_mem /4096;  /*Calcolo il numero di pagine che ho a disposizione sulla macchina.*/
    max_vet = max_pages /32;       /*Divido max_pages per 32 di modo che ottengo il numero che mi interessa*/
                                                /*di elementi del vettore.*/
}

/*---------------------------------------------------------------------------------------------*/
void* richiedi_pagine(int num)
{
    int i, j, count;
    size_t address;
    i=0;
    j=0;

    /*Questa funzione si controlla la presenza di n bit consecuitvi posti a zero.*/
    /*Serve per leggere il valore di un singolo bit*/
   count=0;
    while(count < num)
    {
    /*Questo ciclo si scorre la bitmap fino a che nn trova num bit liberi.*/
       if(free_pages.mem_map[i] & (1<<j))
       {
             /*se la pagina corrente risulta occupata avanza con j e riazzera il contatore*/
                j++;
                count=0;
       }
       else
       {
            /*altrimenti incrementa il contatore*/
            count++;
            j++;
       }
       if(j==32 && i<max_vet && count<num)
       {
           /*se j==31 e nn sono alla fine del vettore e nn ho ancora trovato la sequenza di bit posti a zero allora prosegui con il ciclo.*/
           i++;
           j=0;
       }
       if(i==max_vet && j==31)
           /*Non ho trovato una pagina disponibile, quindi ritorno null*/
           return NULL;
    }
    /*Calcolo l'indirizzo della cella di memoria finale.*/
   address= ((i*32+j)-count)*4096;

   i= (address/4096)/32;
   j= (address/4096)%32;

  count=0;
  while(count<num)
  {
      free_pages.mem_map[i] = (free_pages.mem_map[i] | (1<<j));
      j++;
      count++;
      if(j==32 && count<num)
      {
          i++;
          j=0;
      }
  }
  add_mem_list(address, num);
  return (void *)address;
}

/*---------------------------------------------------------------------------------------------------*/
void add_mem_list(size_t address, int num)
{
/*Questa funzione si occupa di aggiungere nella lista che tiene traccia degli indirizzi occupati*/
/*un campo relativo alla nuova allocazione.*/
    int pos;
    pos = mem_list->next_free;
    if(pos < 510)
    {  /*Se pos<510 allora ho ancora elementi disponibili, quinid aggiorno la strutturina.*/
        mem_list->pages_info[pos].inizio = (void*) address;
        mem_list->pages_info[pos].num_pages = num;
        mem_list->next_free++;
    }
    else
    {   /*Altrimenti, mi serve di allocare una nuova struttura di tipo mem_area e linkarla a quella attuale.*/
        /*Utilizzo la funzione richiedi_pagine.*/
        mem_list->next = (mem_area_pointer) richiedi_pagine(1);
        if(mem_list != NULL)
        {
             mem_list->pages_info[0].inizio = (void*) address;
             mem_list->pages_info[0].num_pages = num;
             mem_list->next_free=1;
        }
    }
}

/*-----------------------------------------------------------------------------------------------*/
void rilascia_pagine(void *indirizzo)
{
/*Questa funzione si occupa di liberare locazioni di memoria precedentemente occupate.*/
    void *min, *max;
    int last;
    char vet_char[20] = {};
    mem_area_pointer mem_tmp;
    mem_tmp = mem_list_root;
    int  i = 0;
    int  k = ((int)indirizzo/4096)/32; 
    int  j = ((int)indirizzo/4096)%32;
    int count = 0;
    int num = 0;
 

    //"Cancelliamo" le informazioni riguardanti le pagine che si devono deallocare
    while(i<510)
    {

        max = mem_tmp->pages_info[i].inizio + mem_tmp->pages_info[i].num_pages;
        min = mem_tmp->pages_info[i].inizio;
        if(min<=indirizzo && max>=indirizzo)
            {
	    	num = mem_tmp->pages_info[i].num_pages; //Salvo il numero di pagine che dopo dobbiamo "deallocare" nella bitmap
		
                last=mem_list->next_free;
                last--;
                mem_tmp->pages_info[i].inizio = mem_list->pages_info[last].inizio;
                mem_tmp->pages_info[i].num_pages = mem_list->pages_info[last].num_pages;
                mem_list->next_free--;

                break;
            }
           i++;
           if(i== 509 && mem_tmp->next != NULL)
               mem_tmp = mem_tmp->next;
     }
     
     
     if(num != 0)
     {
     	//Liberiamo i bit nella bitmap
	while(count < num) 
	{
  
		free_pages.mem_map[k] = (free_pages.mem_map[k] ^ (1<<j)); 
		j++;
		count++;		
		if(j==32 && count < num) 
		{
			k++; 
			j=0;
		} 
	} 
     }
     
     
    _kntos(vet_char, (int) mem_tmp->pages_info[i].inizio, 10);
    _kputs("Dopo il rilascio ho nella posizione della mia strutturina: "); _kputs(vet_char); _kputs(" - ");
    _kntos(vet_char, mem_tmp->pages_info[i].num_pages, 10);
    _kputs("Num Pagine: "); _kputs(vet_char); _kputs(" - ");
}
/*--------------------------------------------------------------------------*/
void print_mem_list()
{
    int i;
    i=0;
    char vet_char[20];
    while(i<6)
    {
        _kntos(vet_char, (int) mem_list->pages_info[i].inizio, 10);
        _kputs(" - "); _kputs(vet_char); _kputs(" --");
        _kntos(vet_char, mem_list->pages_info[i].num_pages, 10);
        _kputs(vet_char); _kputs("\n");
        i++;
    }
    _kputs("\n");
}
/*-------------------------------------------------------------------------*/

void *fis_alloc(const size_t size) {
      return richiedi_pagine(size % 4096 ? size / 4096 + 1 : size / 4096);
}

/*--------------------------------------------------------------------------*/

void fis_free(void *ptr)
{
    rilascia_pagine(ptr);
}



/************************DA ELIMINARE*********************/

/*void rilascia_pagine(void *indirizzo)
{
//Questa funzione si occupa di liberare locazioni di memoria precedentemente occupate.
    int i;
    void *min, *max;
    int last;
    char vet_char[20] = {};
    i=0;
    mem_area_pointer mem_tmp;
    mem_tmp = mem_list_root;
    while(i<510)
    {

        max = mem_tmp->pages_info[i].inizio + mem_tmp->pages_info[i].num_pages;
        min = mem_tmp->pages_info[i].inizio;
        if(min<=indirizzo && max>=indirizzo)
            {
                last=mem_list->next_free;
                last--;
                mem_tmp->pages_info[i].inizio = mem_list->pages_info[last].inizio;
                mem_tmp->pages_info[i].num_pages = mem_list->pages_info[last].num_pages;
                mem_list->next_free--;
                break;
            }
           i++;
           if(i== 509 && mem_tmp->next != NULL)
               mem_tmp = mem_tmp->next;
     }
    _kntos(vet_char, (int) mem_tmp->pages_info[i].inizio, 10);
    _kputs("Dopo il rilascio ho nella posizione della mia strutturina: "); _kputs(vet_char); _kputs(" - ");
    _kntos(vet_char, mem_tmp->pages_info[i].num_pages, 10);
    _kputs("Num Pagine: "); _kputs(vet_char); _kputs(" - ");
}*/