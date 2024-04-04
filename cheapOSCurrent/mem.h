
#ifndef	_MEM_
#define	_MEM_

#include	"config.h"

//Brutto ma necessario!
typedef	unsigned long	mem_t;

struct mem_infocmos_t
	{
	mem_t	base;
	mem_t	ext;
	bool compare_error;
	};

#include	"io.h"
#include	"kconsole.h"
#include	"cmos.h"

//un elemento della tabella di memoria
struct	mem_elemento_t
	{
	mem_t	base;
	mem_t	size;
	unsigned char	flag;
	};

void mem_init();											//Non usare Inizzializza le tabelle di memoria

mem_t mem_alloc(mem_t size);								//Alloca una zona di memoria di dimensione 'size'
bool mem_free(mem_t puntatore);								//Libera la zona
bool mem_resize(mem_t puntatore,mem_t newsize);				//Ridimensiona la zona di memoria

mem_t mem_totale();
mem_t mem_libera();
mem_t mem_usata();

bool	mem_copy(mem_t porigine,mem_t pdestinazione,mem_t size);	//Copia
bool	mem_clear(mem_t puntatore,mem_t size);					//cancella

//Funzioni interne(non usare)
mem_t mem_conta_memoria();								//Rilevamento automatico MEM TOTALE okkio non funge su 386
void	mem_defrag();											//Deframmenta la memoria

void mem_dumpscreen(mem_t address);						//Scrive a schermo la zona di memoria 'address'
void mem_tabledumpscreen();
int	mem_tabledump(char* buffer);

#endif

