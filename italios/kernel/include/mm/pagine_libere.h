#ifndef _PAGINE_LIBERE_H_
#define _PAGINE_LIBERE_H_

#include <cstddef>

//pagine libere DEVE essere un vettore di 32786 elementi
void init_spazio_virtuale_libero(dword* pagine_libere);

void prendi_pagina_virtuale(size_t indice, dword* pagine_libere);

//questa funzione si occupa di 
//1) Cercare lo spazio libero per il numero di pagine richieste. Viene usato il algoritmo first fit. 
//Pertanto viene cercato il primo blocco grande a sufficenza per  contenere le pagine richiestre
//2)dopo averle cercate le marca come usate. NB E' compito di qualche altra funzione tenere conto delle pagine usate e poi liberarle 1 per 1
//
//restituisce il puntatore all'indirizzo della prima pagina
void* prendi_spazio_virtuale(size_t pagine, dword* pagine_libere);

//marca come libera una pagina virtuale
void libera_pagina_virtuale_libera(size_t pagina, dword* pagine_libere);

#endif
