
#ifndef	_KCONSOLE_
#define	_KCONSOLE_

#include	"config.h"

#include	"kstring.h"

#define	KONS_X		80
#define	KONS_Y		25
#define	TAB_LUNG	3

//Stampa a video (interna) del kernel

void kconsole_init();					//Inizzializza Kconsole

void	kclrscr();						//Cancella lo schermo
void kscrllscr();						//Scrolla lo schermo

void	kbaseprint(const char* stringa);	//stampa stringa 'semplice'


//Funzioni che puoi usare------------------------------------------------------------
void	kprint(const char*stringa,...);		//stampa stringa 'con argomenti'

#endif
