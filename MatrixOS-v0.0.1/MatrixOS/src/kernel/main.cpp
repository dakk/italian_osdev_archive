/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: main.cpp			|
	| Date: 16/09/2004			|
	| Authors: Davide B.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	Prototipi	**
	**************************
*/

int main();


/*
	**************************
	**	 Include	**
	**************************
*/

#include <hardware/driver/video.h>
#include <hardware/driver/bochsdebug.h>

#include <kservices/kout.h>
#include <hardware/io.h>

/*
	**************************
	**	 Namespace	**
	**************************
*/

//using namespace



/*
	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////
*/

extern "C"{

	void _start(){
		main();
		while(1);
	}

}

int main(){
	//Inizializo i driver video e pulisco lo schermo
	hardware::driver::video::Video::Init();
	hardware::driver::video::Video::Clear();
	
	//Scrivo il messaggio iniziale
	hardware::driver::video::Video::setTextColor(hardware::driver::video::Green);
	kservices::KOut::WriteLine("MatrixOS");
	kservices::KOut::WriteLine("Build: " __DATE__ " " __TIME__);
	
	//Abilito il debuger di bochs
	hardware::driver::vm::BochsDebug::Init();

	//Entro in un ciclo infinito
	while(1);
}
