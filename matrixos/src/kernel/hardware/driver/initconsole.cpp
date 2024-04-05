/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: initconsole.cpp		|
	| Date: 03/09/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <hardware/driver/initconsole.h>
#include <hardware/driver/video.h>
#include <hardware/driver/vbeconsole.h>
#include <hardware/driver/vbe.h>
#include <boot/mboot.h>

#include <kdef.h>

#include <hardware/io.h>

/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace hardware::driver::video;



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////


//Tipo di console
console_type InitConsole::con;


/*
Inizializzo i driver InitConsole
*/
void InitConsole::Init(console_type consoleType){
	con = consoleType;
	if (con == video){
		Video::Init();
	}else{
		VBE::Init(boot::mboot::getVBEModeInfoBlock());
		VBEConsole::Init();
	}
}

/*
Pulisco il InitConsole
*/
void InitConsole::Clear(){
	if (con == video){
		Video::Clear();
	}else{
		VBEConsole::Clear();
	}
}

/*
Pulisco il InitConsole utilizzando un Colore
*/
void InitConsole::Clear(Color bkColor){
	if (con == video){
		Video::Clear(bkColor);
	}else{
		VBEConsole::Clear(bkColor);
	}
}

/*
Visualizzo a InitConsole una stringa
*/
void InitConsole::Print(const char *c){
	if (con == video){
		Video::Print(c);
	}else{
		VBEConsole::Print(c);
	}
}

/*
Visualizzo a InitConsole un carattere
*/
void InitConsole::Print(const char c){
	if (con == video){
		Video::Print(c);
	}else{
		VBEConsole::Print(c);
	}
}

/*
Nuova linea
*/
void InitConsole::NewLine(){
	if (con == video){
		Video::NewLine();
	}else{
		VBEConsole::NewLine();
	}
}

/*
Posiziono il cursore in un punto dello schermo
*/
void InitConsole::GotoXY(unsigned int x, unsigned int y){
	if (con == video){
		Video::GotoXY(x, y);
	}else{
		VBEConsole::GotoXY(x, y);
	}
}

/*
Setto il Colore dello sfondo
*/
void InitConsole::setBackColor(Color bkColor){
	if (con == video){
		Video::setBackColor(bkColor);
	}else{
		VBEConsole::setBackColor(bkColor);
	}
}

/*
Leggo il Colore dello sfondo
*/
Color InitConsole::getBackColor(){
	if (con == video){
		return Video::getBackColor();
	}else{
		return VBEConsole::getBackColor();
	}
}

/*
Setto il Colore del testo
*/
void InitConsole::setTextColor(Color txtColor){
	if (con == video){
		Video::setTextColor(txtColor);
	}else{
		VBEConsole::setTextColor(txtColor);
	}
}

/*
Leggo il Colore del testo
*/
Color InitConsole::getTextColor(){
	if (con == video){
		return Video::getTextColor();
	}else{
		return VBEConsole::getTextColor();
	}
}

/*
Leggo la larghezza dello schermo
*/
unsigned int InitConsole::getWidth(){
	if (con == video){
		return Video::getWidth();
	}else{
		return VBEConsole::getWidth();
	}
}

/*
Leggo l'altezza dello schermo
*/
unsigned int InitConsole::getHeight(){
	if (con == video){
		return Video::getHeight();
	}else{
		return VBEConsole::getHeight();
	}
}

/*
Leggo la posizione del cursore dal lato sinistro dello schermo
*/
unsigned int InitConsole::getX(){
	if (con == video){
		return Video::getX();
	}else{
		return VBEConsole::getX();
	}
}

/*
Leggo la posizione del cursore dal lato sopra dello schermo
*/
unsigned int InitConsole::getY(){
	if (con == video){
		return Video::getY();
	}else{
		return VBEConsole::getY();
	}
}
