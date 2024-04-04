/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: main.cpp			|
	| Date: 16/09/2004			|
	| Authors: See the AUTHOR file		|
	| License: GNU General Public License	|
	+---------------------------------------+
	| Atenzione: Parte del codice fa parte	|
	|	del sistema operativo ItaliOS	|
	|	e del sistema operativo DreamOS	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <hardware/driver/video.h>

#include <kdef.h>

#include <hardware/io.h>

/*
	**************************
	**	  define	**
	**************************
*/

#define screen_width 80
#define screen_height 25

#define screen_size screen_height*screen_width

#define tab_size 5


/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace hardware::driver::video;



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////



//Memoria video
unsigned short *Video::videomem;	

//Colore in uso
unsigned char Video::CColor;

//Posizione del cursore
int Video::Cy;
int Video::Cx;



/*
Inizializzo i driver video
*/
void Video::Init(){

	//Inizializzo la memoria video
	videomem = (unsigned short *) 0xB8000;

	//Setto il colore del testo e quello dello sfondo
	setTextColor(White);
	setBackColor(Black);
	
	//Setto la posizione del cursore
	GotoXY(0,0);
	
	//Pulisco il video
	Clear();
}



/*
Pulisco il video
*/
void Video::Clear(){

	unsigned int i;

	for (i = 0; i < screen_size; i++){
		videomem[i] = (unsigned char) ' ' | getFormedColor();
	}

	GotoXY(0,0);
}

/*
Pulisco il video utilizzando un Colore
*/
void Video::Clear(Color bkColor){

	char TmpColor = CColor; //Aggiorno la variabile temporanea
	char mycolor = (char) bkColor;
	
	//azzero i bit deillo sfondo
	TmpColor &=0x8f; //ottengo x000 xxxx

	//preparo i bit dello sfondo
	mycolor &= 0x7;  //cancello tutti i i bbit tranne i primi tre quindi alla fine ottengo 0000 0xxx
	mycolor <<=4;  //shifto tutto di 4 in questo modo li allineo per bene e ottengo 0xxx 0000

	//setto il colore per bene
	TmpColor |= mycolor;

	
	unsigned int i;
	
	for (i = 0; i < screen_size; i++){
		videomem[i] = (unsigned char) ' ' | TmpColor<<8;
	}

	GotoXY(0,0);
}



/*
Visualizzo a video una stringa
*/
void Video::Print(const char *c){

	int i;
	for (i = 0; c[i] != '\0'; i++) {
		Print(c[i]);
	}
}

/*
Visualizzo a video un carattere
*/
void Video::Print(const char c){

	if(c == '\n'){
		NewLine();
		return;
	}

	if(c == '\t'){
		for(size_t i = 0; i < tab_size; i++){
			Print(' ');
		}
		return;
	}
	
	if(Cx >= screen_width){
		NewLine();
	}
	
	if((Cx + Cy*screen_width) >= screen_size){
		//Scroling
		for(size_t i=0; i<screen_size - screen_width; i++){
			videomem[i] = videomem[i+screen_width];
		}
		
		//Cancella l'ultima riga
		for(size_t i = screen_size - screen_width; i <screen_size; i++){
			videomem[i] = (unsigned char) ' '  | getFormedColor() ;
		}
		
		GotoXY(0,Cy);
	}
	
	videomem[Cx + Cy*screen_width] = (unsigned char) c | getFormedColor();
	
	GotoXY(Cx+1,Cy);
}



/*
Nuova linea
*/
//Todo:	Completare NewLine
void Video::NewLine(){
	GotoXY(0,Cy + 1);
}

/*
Posiziono il cursore in un punto dello schermo
*/
void Video::GotoXY(unsigned int x, unsigned int y){
	//Verifico che x e y non contengano valori errati
	//if(x <= screen_width | y <= screen_height){

		Cx = x;
		Cy = y;

		//Posiziono X
		outportb(0x3D4, 0x0F);
		outportb(0x3D5, (byte) x);

		//Posiziono Y
		outportb(0x3D4, 0x0E);
		outportb(0x3D5, (byte) y);
	//}
}



/*
Setto il Colore dello sfondo
*/
void Video::setBackColor(Color bkColor){

	char mycolor = (char) bkColor;
	//azzero i bit deillo sfondo
	CColor &=0x8f; //ottengo x000 xxxx

	//preparo i bit dello sfondo
	mycolor &= 0x7;  //cancello tutti i i bbit tranne i primi tre quindi alla fine ottengo 0000 0xxx
	mycolor <<=4;  //shifto tutto di 4 in questo modo li allineo per bene e ottengo 0xxx 0000

	//setto il colore per bene
	CColor |= mycolor;
}

/*
Leggo il Colore dello sfondo
*/
Color Video::getBackColor(){

	return (Color) ( (CColor & 0x7f) >>4 );
}



/*
Setto il Colore del testo
*/
void Video::setTextColor(Color txtColor){

	char mycolor = (char) txtColor;
	CColor = CColor & 0x70;
	CColor |= (mycolor & 0xF);
}

/*
Leggo il Colore del testo
*/
Color Video::getTextColor(){

	return (Color) ( CColor & 0xF );
}



/*
Leggo la larghezza dello schermo
*/
unsigned int Video::getWidth(){

	return screen_width;
}

/*
Leggo l'altezza dello schermo
*/
unsigned int Video::getHeight(){

	return screen_height;
}



/*
Leggo la posizione del cursore dal lato sinistro dello schermo
*/
int Video::getX(){

	return Cx;
}



/*
Leggo la posizione del cursore dal lato sopra dello schermo
*/
int Video::getY(){

	return Cy;
}



/*
Converto il Colore in uso in un 'unsigned short int'
*/
unsigned short int Video::getFormedColor(){

	return CColor << 8;
}
