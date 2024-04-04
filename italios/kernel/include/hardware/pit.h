#ifndef _PIT_H_
#define _PIT_H_

#include <cstddef>
#include <video.h>

typedef size_t frequenza;
class pit_count{
public:
	enum mode{
		mode0,
		mode1,
		mode2,
		mode3,
		mode4,
		mode5
	};
	enum count_mode{
		binary_count,  //da 0 a FFFF
		bcd_count	//da 0 a 9999
	};
	enum access_mode{
			counter_latch,  //counter latch instruction
			byte_low,	//solo il byte basso
			byte_high,	//solo il byte alto
			byte_low_high	//prima il byte basso e poi il byte alto
	};
	enum canale{
		canale0,
		canale1,
		canele2
	};

	pit_count(frequenza freq=19, canale chan=canale0, mode 				funzionamento=mode2,access_mode acm= byte_low_high, 				count_mode cm=binary_count );
	frequenza set_frequenza(frequenza freq);
	void set_canale(canale chan);
	void set_count_mode(count_mode cm);
	void set_access_mode(access_mode acsmod);
	void set_mode(mode funzionamento);
	
	//si occupa semplicemente di inizializzare il pit
	//ma non abilita l'interrupt e non si occupa
	//di impostare l'isr
	void carica();
private:
	static const word control = 0x43;
	/*static const word counter0 = 0x40;
	static const word counter1 = 0x41;
	static const word counter2 = 0x42;*/
	static const word base_counter = 0x40; //indica da dove iniziano i contatori
	static const dword pit_freq = 1193180;
	//indica la porta rispettiva del pit
	//0x40 indica il contatore0
	//0x41 indica il contatore1
	//0x42 indica il contatore2
	word porta_di_lavoro;
	struct control_register{
		byte count:1;	//modo di contare
		byte mode :3;	//modalità di funzionamento
		byte rw : 2;	//operazione da compiere
		byte sc : 2;	//selezione contatore
	};
	union control_reg{
		struct control_register reg;
		byte preg;
	};
	control_reg registro_di_controllo;
	word _freq;
};

#endif
