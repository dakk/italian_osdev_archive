/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: pit.h			|
	| Date: 21/01/2005			|
	| Authors: See the AUTHOR file		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

#ifndef _HARDWARE_DRIVER_PIT_H_
#define _HARDWARE_DRIVER_PIT_H_


/*
	**************************
	**	 Include	**
	**************************
*/

#include <kdef.h>

#define BASE_COUNTER 0x40
#define PIT_CONTROL 0x43
#define PIT_FREQ 1193180

namespace hardware{

	namespace driver{
	
		class PIT{
		public:
			enum CounterMode{
				BinaryCounter,
				BCDCounter,
			};
			
			enum Mode{
				Mode0,
				Mode1,
				Mode2,
				Mode3,
				Mode4,
				Mode5
			};
			
                        enum AccessMode{
				CounterLatch,	//Counter latch instruction
				ByteLow, 	//Solo il byte basso
				ByteHigh,	//Solo il byte alto
				ByteLowHigh	//Prima il byte basso e poi il byte alto
			};
                        
			enum Chan{
				Chan0,
				Chan1,
				Chan2
			};
			
			static void Init(); //Inizializza i driver del PIT
			

			PIT(PIT::CounterMode countermode = BinaryCounter ,PIT::AccessMode accessmode = ByteLowHigh, PIT::Mode mode = Mode2, PIT::Chan chan = Chan0, size_t frequency = 19); //Costruttore del oggetto
                        
			void setCounterMode(CounterMode countermode); //Setta la modalità del contattore (BCD o Binaria)
			CounterMode getCounterMode(); //Legge la modalità del contattore (BCD o Binaria)
			
			void setMode(Mode mode); //Setta la modalità di funzionamento del PIT
			Mode getMode(); //Legge la modalità di funzionamento del PIT
			
			void setChan(Chan chan); //Setta il canale
			Chan getChan(); //Legge il canale
			
			void setFrequency(size_t frequency); //Setta la frequenza
			size_t getFrequency(); //Legge la frequenza
			
			void setAccessMode(AccessMode accessmode); //Setta la modalità di accesso.
			AccessMode getAccessMode(); //Legge la modalità di accesso.
                        
			bool Enable(); //Abilita il PIT
		private:
			word freq;
			byte port;
                        
			struct control_register{
				byte counter_mode :1;	//modo di contare
				byte mode :3;		//modalità di funzionamento
				byte access_mode : 2;	//operazione da compiere
				byte chan : 2;		//selezione contatore
			};
			
			union pit_reg{
				struct control_register ControlRegister;
				byte pit_register;
			};
			
		pit_reg PIT_Reg;
		};
	}
}

#endif
