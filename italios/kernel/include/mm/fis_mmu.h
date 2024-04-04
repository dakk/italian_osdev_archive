#ifndef _FIS_MMU_
#define _FIS_MMU_


#include <cstddef>

size_t count_memory();

void init_spazio_fisico();

//restituisce la base della pagina, se non ci sono + pagine restituisce NULL
void*  prendi_pagina_fisica();

//libera una pagina, se è una pagina valida viene liberata e viene tornato true, altrimenti viene tornato false
//viene tornato false se ad esempio l'indirizzo non è  un indirizzo restituito da prendi_pagina_fisica
bool rilascia_pagina(void *addr);


//prende pagine che vanno da inizio a fine, restituisce il puntatore alla prima pagina
void* prendi_spazio_fisico(void *iinizio, void *ifine);

//libera pagine che vanno da inizio a fine, restituisce il puntatore alla prima pagina
void libera_spazio_fisico(void *iinizio, void *ifine);

#endif
