/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: pit.cpp			|
	| Date: 21/01/2005			|
	| Authors: See the AUTHOR file		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/
#include <hardware/driver/pit.h>
#include <kservices/kdbg.h>

#include <hardware/io.h>

#include <kdef.h>

/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace hardware::driver;


	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////



/*
  Inizializza i driver del PIT
*/
void PIT::Init(){
}

/*
  Costruttore del oggetto
*/
PIT::PIT(PIT::CounterMode countermode,PIT::AccessMode accessmode, PIT::Mode mode, PIT::Chan chan, size_t frequency){
	setCounterMode(countermode);
        setAccessMode(accessmode);
	setMode(mode);
	setChan(chan);
        setFrequency(frequency);
}


/*
  Setta la modalità del contattore (BCD o Binaria)
*/
void PIT::setCounterMode(PIT::CounterMode countermode){
	PIT_Reg.ControlRegister.counter_mode = (byte) countermode;
}

/*
  Legge la modalità del contattore (BCD o Binaria)
*/
PIT::CounterMode PIT::getCounterMode(){
	return (PIT::CounterMode) PIT_Reg.ControlRegister.counter_mode;
}



/*
  Setta la modalità di funzionamento del PIT
*/
void PIT::setMode(PIT::Mode mode){
	PIT_Reg.ControlRegister.mode = (byte) mode;
}

/*
  Legge la modalità di funzionamento del PIT
*/
PIT::Mode PIT::getMode(){
	return (PIT::Mode) PIT_Reg.ControlRegister.mode;
}



/*
  Setta il canale
*/
void PIT::setChan(PIT::Chan chan){
	PIT_Reg.ControlRegister.chan = (byte) chan;
        
        port = PIT_Reg.ControlRegister.chan + BASE_COUNTER;
}

/*
  Legge il canale
*/
PIT::Chan PIT::getChan(){
	return (PIT::Chan) PIT_Reg.ControlRegister.chan;
}



/*
  Setta la frequenza
*/
void PIT::setFrequency(size_t frequency){
	kservices::KDbg::WriteLine((int) (PIT_FREQ / frequency));
	freq = PIT_FREQ / frequency;
        kservices::KDbg::WriteLine(freq);
}

/*
  Legge la frequenza
*/
size_t PIT::getFrequency(){
	return freq * PIT_FREQ;
}



/*
  Setta la modalità di accesso.
*/
void PIT::setAccessMode(PIT::AccessMode accessmode){
	PIT_Reg.ControlRegister.access_mode = (byte) accessmode;
}

/*
  Legge la modalità di accesso.
*/
PIT::AccessMode PIT::getAccessMode(){
	return (PIT::AccessMode) PIT_Reg.ControlRegister.access_mode;
}



/*
  Abilita il PIT
*/
bool PIT::Enable(){
	outportb_p(PIT_CONTROL, PIT_Reg.pit_register);
	
        outportb_p(port, freq & 0xFF);
        outportb_p(port, freq >> 8);
	return true;
}
