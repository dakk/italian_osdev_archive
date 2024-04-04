#ifndef _RTC_H_
#define _RTC_H_

#ifdef __cplusplus

extern "C"{
	
#endif

#include <stddef.h>

void init_rtc();

//Domenica è 0									  
size_t leggi_numero_giorno_della_settimana();			  
const char* leggi_nome_giorno_della_settimana();
byte leggi_giorno_del_mese();
byte leggi_numero_mese();
word leggi_anno();
byte leggi_secondi();
byte leggi_minuti();
byte leggi_ore();

	
#ifdef __cplusplus

};
	
#endif
	
#endif
