/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: keyboard.cpp		|
	| Date: 31/12/2004			|
	| Authors: Davide B.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <hardware/cpu/irq.h>
#include <hardware/driver/video.h>
#include <hardware/io.h>
#include <hardware/driver/keytraslation.h>
#include <hardware/driver/keyboard.h>


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
Scrivo un scancode nel buffer
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
Leggo un scancode dal buffer
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
			hardware::driver::video::Video::Print("\n");
			pos = 0;
			for(int z = 0; z<BUF_SIZE; z++) buf[z] = NO_VALID_SCANDODE;
			return;
		case BACKSPACE:
			//Codice backspace --POSSIBILI BUG---
			if(i>0){
				i-=2;

				if (hardware::driver::video::Video::getX() == 1){
					hardware::driver::video::Video::GotoXY(80,hardware::driver::video::Video::getY()-1);
					hardware::driver::video::Video::Print(" ");
					hardware::driver::video::Video::GotoXY(80,hardware::driver::video::Video::getY());

				}else{
					hardware::driver::video::Video::GotoXY(hardware::driver::video::Video::getX()-1,hardware::driver::video::Video::getY());
					hardware::driver::video::Video::Print(" ");
					hardware::driver::video::Video::GotoXY(hardware::driver::video::Video::getX()-1,hardware::driver::video::Video::getY());
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
			hardware::driver::video::Video::Print(echostr);
			break;
		}
		str[len - 1] = '\0';

		asm("sti");
	}
}
