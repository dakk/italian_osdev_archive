/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: keyboard.cpp		|
	| Date: 31/12/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <hardware/cpu/irq.h>
#include <hardware/driver/initconsole.h>
#include <hardware/io.h>
#include <hardware/driver/keytraslation.h>
#include <hardware/driver/keyboard.h>

#include <kdef.h>

/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace hardware::driver::keyboard;



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////


//Buffer
int Keyboard::buf[BUF_SIZE];
//Indice del carattere nel bufer
int Keyboard::pos;



/*
Inizializzo i driver della tastiera
*/
void Keyboard::Init(){
	KeyTraslation::InitMap();

	hardware::cpu::IRQ::Add(KeyboardISR,1);
	hardware::cpu::IRQ::EnableIRQ(1);

	for(int i = 0; i<BUF_SIZE; i++) buf[i] = NO_VALID_SCANDODE;
}

/*
Gestore dell'IRQ della tastiera (Interrupt Sevice Routine)
*/
void Keyboard::KeyboardISR(){
	//Porta: 0x60
	BufWrite(inportb(0x60));
}

/*
Scrivo uno scancode nel buffer
*/
void Keyboard::BufWrite(int scanc){
	pos++;
	
	if (pos >= 250){
		pos = 0;
		for(int i = 0; i<BUF_SIZE; i++) buf[i] = NO_VALID_SCANDODE;
	}

	buf[pos] = scanc;
}

/*
Leggo uno scancode dal buffer
*/
int Keyboard::BufRead(){
	return buf[pos];
}

/*
Leggo un carattere
*/
char Keyboard::getChar(){
	
	int c;
	do{
		asm("cli");
		c = KeyTraslation::Scan2Ascii(BufRead());
		asm("sti");
	}while(c == NO_VALID_SCANDODE);
	asm("cli");
	pos++;
	asm("sti");
		
	return (char)c;
}

/*
Leggo una stringa
*/
void Keyboard::getStr(char *str, size_t len){
	
	for(size_t i = 0;i < (len-1) ;i++){
		asm("cli");

		char chr = 0;
		chr = getChar();

		switch(chr){
		case '\n':
			str[i] = '\0';
			hardware::driver::video::InitConsole::Print("\n");
			pos = 0;
			for(int z = 0; z<BUF_SIZE; z++) buf[z] = NO_VALID_SCANDODE;
			return;
		case BACKSPACE:
			//Codice backspace --POSSIBILI BUG---
			if(i>0){
				i-=2;

				if (hardware::driver::video::InitConsole::getX() == 1){
					hardware::driver::video::InitConsole::GotoXY(hardware::driver::video::InitConsole::getWidth(),hardware::driver::video::InitConsole::getY()-1);
					hardware::driver::video::InitConsole::Print(" ");
					hardware::driver::video::InitConsole::GotoXY(hardware::driver::video::InitConsole::getWidth(),hardware::driver::video::InitConsole::getY());

				}else{
					hardware::driver::video::InitConsole::GotoXY(hardware::driver::video::InitConsole::getX()-1,hardware::driver::video::InitConsole::getY());
					hardware::driver::video::InitConsole::Print(" ");
					hardware::driver::video::InitConsole::GotoXY(hardware::driver::video::InitConsole::getX()-1,hardware::driver::video::InitConsole::getY());
				}

			}else{
				i--;
			}

			break;
		default:
			str[i] = chr;
			char echostr[2];
			echostr[0] = chr;
			echostr[1] = '\0';

			//Scrivo sullo schermo i caratteri digitati.
			hardware::driver::video::InitConsole::Print(echostr);
			break;
		}
		str[len - 1] = '\0';

		asm("sti");
	}
}
