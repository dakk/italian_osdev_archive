
#ifndef	_CACHE_
#define	_CACHE_

#include	"config.h"

#include	"mem.h"

#define	CACHE_MAXACCESSI	1024

//Un Elemento della CACHE
struct cache_element_t
	{
	int lba;
	int accessi;
	char	image[512];
	}__attribute__ ((packed));

//Una CACHE abbinabile ad un disposistivo
struct cache_t
	{
	int misscount,hitcount;
	int maxblocknum;
	int cachedblocknum;
	struct cache_element_t* block;
	};

bool cache_init			(struct cache_t* cache,int maxblocknum);	//Inizzializza la CACHE
void cache_clear		(struct cache_t* cache);					//Svuota la CACHE

bool cache_findcopy	(struct cache_t* cache,int lba,mem_t buffer);	//Se trova il Blocco 'lba' in CACHe lo copia in buffer
bool cache_insertrefresh	(struct cache_t* cache,int lba,mem_t buffer);	//Se il Blocco e' gioa' in CACHe lo aggiorna altrimenti ce lo inserisce (serve il puntatore al buffer)
														//TRUE insert FALSE refresh

void cache_dumpscreen	(struct cache_t* cache);					//Stampa a schermo la situazione della CACHE
int	cache_dump			(char*buffer,struct cache_t* cache);

#endif

