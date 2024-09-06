/*
 * Dreamos
 * fismem.h
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
  * Prima versione: 27/10/2003
  * Thanks to: Silvio Abruzzo ^_^ che come sempre senza di lui nn potevo fare nemmeno questa :)
  */

/***************************************************************************************
 *Informazioni sulla ram:                                                              *
 *Allora massimo indirizzabile: 4gb = 4* 1024*1024*1024 = 4294967296byte               *
 *In esadecimale e': 0x100000000.                                                      *
 *Numero massimo di pagine da 4k e': 1048576 pagine.                                   *
 *numero elementi vettore e' 32768                                                     *
 ***************************************************************************************/
#ifndef __FISMEM_H
#define __FISMEM_H

#include <mydef.h>

#define BASE_MEM 0x100000  //Faccio Partire il calcolo da 1mb

#define OFFSET_MEM 0x400   //L'offset e' di 1k
#define VALUE_MEM 0xAA55AA55  //Il primo valore che provo a scrivere
#define ANTI_VALUE_MEM ~0xAA55AA55 //Il secondo valore che provo a scrivere

#define MAX_NUM_PAGES 0x100000
#define SIZE_VETT 0x8000




/*************************************************************************************
 *                       Strutture Dati                                                             *
 *************************************************************************************/
typedef struct {
    size_t mem_map[SIZE_VETT];
}mem_info; // Contiene informazioni sulle pagine libere, e' un vettore con elementi da 32 bit, ogni bit equivale a una pagina.

typedef struct {
    void* inizio;
    size_t num_pages;
}pages_struct; //Contiene le informazioni su quante pagine alloco ogni volta e l'indirizzo di partenza

struct mem_area{
    pages_struct pages_info[510];
    size_t next_free;
    struct mem_area* next;
}; //questa struttura dati, contiene un vettore della struttura precedente, in modo che mi permette una  facile localizzazione.
                //contiene campo che mi dice quale e' il prossimo elemento libero e un puntatore a una struttura analoga in caso ne serva
               //piu di una.

typedef struct mem_area *mem_area_pointer;
/**************************************************************************************
 *                       Variabili globali                                                            *
 *************************************************************************************/
extern mem_area_pointer mem_list;            //Lista di strutture contenente le info sulle pagine allocate
extern size_t total_mem;                            //La memoria totale calcolata
extern mem_info free_pages;                     //Contiene le pagine libere (la bitmap)
extern unsigned int pos_mem;                   //Dovrebbe puntare alla prima locazione di memoria dispinibile. Dovrebbe partire da 9? (nn so se serve)
extern int max_pages;                              //Numero Massimo di pagine disponibili
extern int max_vet;                                  //Numero di elementi del vettore.


/**************************************************************************************
 *                       Prototipi di funzioni                                                      *
 *************************************************************************************/
void init_pages();                          //Azzera il vettore delle pagine allocate
void mem_size();                           //Calcola la quantita di memoria disponibile
void* richiedi_pagine(int);               //Alloca x pagine di memoria
void rilascia_pagine(void *);     //Rilascia x pagine di memoria
void add_mem_list(size_t, int);       //Aggiunge un elemento nel vettore di strutturine.
void print_mem_list();
void *fis_alloc(const size_t size);
void fis_free(void *);
#endif
