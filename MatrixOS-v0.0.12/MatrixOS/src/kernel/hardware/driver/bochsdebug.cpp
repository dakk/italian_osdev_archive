/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: bochsdebug.cpp		|
	| Date: 23/11/2004			|
	| Authors: Davide B.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <hardware/driver/bochsdebug.h>

#include <kdef.h>

#include <hardware/io.h>

/*
	**************************
	**	  define	**
	**************************
*/

#define BOCHS_DEBUG_PORT 0x8A00

#define BOCHS_DEBUG_ENABLE 			0x8A00 //Usato per abilitare il debug 
#define BOCHS_DEBUG_SR0				0x8A01 //Selects register 0: Indirizzo dell'inizio del monitoraggio della memoria (inclusivo)
#define BOCHS_DEBUG_SR1				0x8A02 //Selects register 1: Indirizzo della fine del monitoraggio della memoria (esclusivo) 
#define BOCHS_DEBUG_ENABLE_MEM_MONITOR		0x8A80 //Abilita il monitoraggio della memoria come indicato dai registri 0 e 1 e cancella entrambi i registri  
#define BOCHS_DEBUG_PROMPT_RETURN		0x8AE0 //Ritorna il Prompt di debug 
#define BOCHS_DEBUG_ISTRUCTION_TRACE_OFF	0x8AE2 //Disabilita il trace delle istruzioni 
#define BOCHS_DEBUG_ISTRUCTION_TRACE_ON		0x8AE3 //Abilita il trace delle istruzioni 
#define BOCHS_DEBUG_REG_TRACE_ON		0x8AE5 //Disabilita il trace dei registri 
#define BOCHS_DEBUG_REG_TRACE_OFF		0x8AE4 //Disabilita il trace dei registri 
#define BOCHS_DEBUG_DISABLE			0x8AFF //Disabilita l'i/o debug e le funzioni di monitoraggio della memoria


/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace hardware::driver::vm;



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////

/*
Inizializza il debug di bochs
*/
void BochsDebug::Init(){
	outportw(BOCHS_DEBUG_PORT, BOCHS_DEBUG_ENABLE);
	OutputMsg("\n\n Inizializato BochsDebug\n");
}

/*
Scrive un carattere nel output di bochs
*/
void BochsDebug::OutputMsg(const char c){
	outportb(0xE9, c);
}

/*
Scrive una stringa nel output di bochs
*/
void BochsDebug::OutputMsg(const char *c){
	int i;
	
	for (i = 0; c[i] != '\0'; i++){
		OutputMsg(c[i]);
	}
}

/*
Setta lo stato del trace delle istruzioni
*/
void BochsDebug::IstructionTrace(TraceStatus status){
	if (status == tsON){
		outportw(BOCHS_DEBUG_PORT, BOCHS_DEBUG_ISTRUCTION_TRACE_ON);
	}else if (status == tsOFF){
		outportw(BOCHS_DEBUG_PORT, BOCHS_DEBUG_ISTRUCTION_TRACE_OFF);
	}
}

/*
Setta lo stato del trace dei registri
*/
void BochsDebug::RegisterTrace(TraceStatus status){
	if (status == tsON){
		outportw(BOCHS_DEBUG_PORT, BOCHS_DEBUG_REG_TRACE_ON);
	}else if (status == tsOFF){
		outportw(BOCHS_DEBUG_PORT, BOCHS_DEBUG_REG_TRACE_OFF);
	}
}

/*
Ferma il debug
*/
void BochsDebug::Stop(){
	outportw(BOCHS_DEBUG_PORT, BOCHS_DEBUG_PROMPT_RETURN);
}
