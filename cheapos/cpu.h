
#ifndef	_CPU_
#define	_CPU_

#include	"config.h"

#include	"kconsole.h"
#include	"cpudetect.h"
#include	"time.h"
#include	"irq.h"
#include	"isr.h"


#define	IRQ_START		0x20			//	Da dove rimappo IQR (da 32 a 256-32=224)  _NON_CAMBIARE_MAI_!_


void	cpu_init();			//Non usare Init gestione interrupt e Time Clock

inline void cpu_sti();		//STI
inline void cpu_cli();		//CLI

#endif
