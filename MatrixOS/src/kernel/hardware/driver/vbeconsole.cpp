/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: vbeconsole.cpp		|
	| Date: 19/06/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	+---------------------------------------------------------------+
	|			Console Size				|
	+---------------------------------------------------------------+
	|  1024x768 (logo) |   43 - 3 line	|	128 - 2 col	|
	+---------------------------------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <hardware/cpu/bitops.h>

#include <hardware/driver/vbe.h>

#include <hardware/driver/vbeconsole.h> 
#include <hardware/driver/vbefont.h>
#include <hardware/driver/vbematrixlogo.h>

#include <kdef.h>



/*
	**************************
	**	  define	**
	**************************
*/

#define screen_width 126
#define screen_height 40

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


//Memoria dei caratteri stampati a video
unsigned short VBEConsole::charMem[128*128];

//Colore in uso
unsigned char VBEConsole::CColor;

//Posizione del cursore
int VBEConsole::Cy;
int VBEConsole::Cx;

//Puntatori alle funzioni grafiche
void (*VBEConsole::DrawChar)(unsigned short vchar, unsigned int vcharx, unsigned int vchary);
void (*VBEConsole::DrawCursor)(unsigned short vchar, unsigned int vcharx, unsigned int vchary);

//Colori usati dalla console (in formato RGB)
const char colors[16*3] =
{//RGB
 0,0,0,		//Black
 0,0,168,	//Blue
 0,168,0,	//Green
 0,168,168,	//Cyan
 168,0,0,	//Red
 168,0,168,	//Magenta
 168,84,0,	//Brown
 168,168,168,	//White
 84,84,84,	//Dark Grey
 84,84,252,	//Bright Blue
 84,252,84,	//Bright Green
 84,252,252,	//Bright Cyan
 252,84,84,	//Bright Red
 252,84,252,	//Bright Magenta
 252,252,84,	//Yellow
 252,252,252	//Bright White
};

/*
Inizializzo i driver vbeconsole
*/
void VBEConsole::Init(){
	//Inizializzo i puntattori alle funzioni grafiche
	if(VBE::bpp == 32){
		DrawChar = DrawChar32;
		DrawCursor = DrawCursor32;
	}else{
		DrawChar = DrawChar24;
		DrawCursor = DrawCursor24;
	}

	//Setto il colore del testo e quello dello sfondo
	setTextColor(White);
	setBackColor(Black);

	//Setto la posizione del cursore
	GotoXY(1,1);

	//Pulisco il video
	Clear();
}

/*
Pulisco il video
*/
void VBEConsole::Clear(){
	//Ottengo il colore in uso
	unsigned int backColor = getBackColor();

	//Pulisco il video
	VBE::Clear(colors[backColor*3],colors[backColor*3+1],colors[backColor*3+2]);

	//Disegno un rettangolo bianco nella parte superiore dello schermo
	VBE::DrawFillRectangle(0,0,1024,75,255,255,255);

	//Disegno il logo
	VBE::DrawImage(imgMatrixOS.width, imgMatrixOS.height, imgMatrixOS.pixel_data);

	//Pulisco la memoria dei caratteri
	unsigned int i;

	for (i = 0; i < screen_size; i++){
		charMem[i] = (unsigned char) ' ' | getFormedColor();
	}
	GotoXY(1,1);
}

/*
Pulisco il video utilizzando un Colore
*/
void VBEConsole::Clear(Color bkColor){
	//Pulisco il video
	VBE::Clear(colors[(unsigned int)bkColor*3],colors[(unsigned int)bkColor*3+1],colors[(unsigned int)bkColor*3+2]);

	//Disegno un rettangolo bianco nella parte superiore dello schermo
	VBE::DrawFillRectangle(0,0,1024,75,255,255,255);

	//Disegno il logo
	VBE::DrawImage(imgMatrixOS.width, imgMatrixOS.height, imgMatrixOS.pixel_data);

	//Pulisco la memoria dei caratteri
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
		charMem[i] = (unsigned char) ' ' | TmpColor<<8;
	}

	GotoXY(1,1);
}

/*
Visualizzo a video una stringa
*/
void VBEConsole::Print(const char *c){

	int i;
	for (i = 0; c[i] != '\0'; i++) {
		Print(c[i]);
	}
}

/*
Converto il Colore in uso in un 'unsigned short int'
*/
unsigned short int VBEConsole::getFormedColor(){

	return CColor << 8;
}


/*
Visualizzo a video un carattere
*/
void VBEConsole::Print(const char c){
	//Serve per vedere se è necessario andare a capo
	static bool gotonewline = false;

	//Vado a capo
	if (c == '\n'){
		NewLine();
		return;
	}

	if (c == '\t'){
		for(size_t i = 0; i < tab_size; i++){
			Print(' ');
		}
		return;
	}

	//Verifico se è necessario andare a capo.
	if(gotonewline == true && Cx == screen_width){
		NewLine();
		//Aggiorno la variabile
		gotonewline = false;
	
	//Verifico se la variabile gotonewline ha un valore corretto
	}else if(gotonewline == true && Cx != screen_width){
		gotonewline = false;
	}	
	//Se ho raggiunto il limite dello schermo
	if((Cx-1 + (Cy-1)*screen_width) == screen_size){
		//Scroling
		for(size_t i=0; i<screen_size - screen_width; i++){
			charMem[i] = charMem[i+screen_width];
		}
		
		//Cancella l'ultima riga
		for(size_t i = screen_size - screen_width; i <screen_size; i++){
			charMem[i] = (unsigned char) ' '  | getFormedColor() ;
		
		}

		//Ridisegno i caratteri
		for (int redrawY = 1; redrawY <= screen_height; redrawY++){
			for (int redrawX = 1; redrawX <= screen_width; redrawX++){
				//Controllo se è necessario ridisegnare un carattere
				if ((charMem[redrawX-1 +  (redrawY-1)*screen_width] != charMem[redrawX-1 +  (redrawY-2)*screen_width]) || (redrawY < 2)){
					DrawChar(charMem[redrawX-1 +  (redrawY-1)*screen_width],redrawX,redrawY);
				}
			}
		}
		
		//Mi posiziono nel ultima riga dopo aver fatto lo scroling
		GotoXY(1,screen_height);
	}
	
	//Scrivo il carattere
	unsigned int CharMemI = (Cx-1) + (Cy-1)*screen_width;
	charMem[CharMemI] = (unsigned char) c | getFormedColor();
	DrawChar(charMem[CharMemI],Cx,Cy);

	//Vado a capo la prossima volta che scrivo un carattere
	if(Cx == screen_width){
		gotonewline = true;
	}else{
		GotoXY(Cx+1,Cy); //posizione successiva
	}
}

/*
Nuova linea
*/
void VBEConsole::NewLine(){
	//Se la posizione di y è minore dell'altezza dello schermo non è necessario fare lo scroling
	if(Cy < screen_height){
		GotoXY(1,Cy + 1);
	}else if(Cy == screen_height){

		//Scroling
		for(size_t i=0; i<screen_size - screen_width; i++){
			charMem[i] = charMem[i+screen_width];
		}
		
		//Cancella l'ultima riga
		for(size_t i = screen_size - screen_width; i <screen_size; i++){
			charMem[i] = (unsigned char) ' '  | getFormedColor() ;
		}

		//Ridisegno i caratteri
		for (int redrawY = 1; redrawY <= screen_height; redrawY++){
			for (int redrawX = 1; redrawX <= screen_width; redrawX++){
				//Controllo se è necessario ridisegnare un carattere
				if ((charMem[redrawX-1 +  (redrawY-1)*screen_width] != charMem[redrawX-1 +  (redrawY-2)*screen_width]) || (redrawY < 2)){
					DrawChar(charMem[redrawX-1 +  (redrawY-1)*screen_width],redrawX,redrawY);
				}
			}
		}

		GotoXY(1,screen_height); //Mi posiziono nel ultima riga dopo aver fatto lo scroling
	}
}

/*
Posiziono il cursore in un punto dello schermo
*/
void VBEConsole::GotoXY(unsigned int x, unsigned int y){

	/*Verifico che x e y non contengano valori errati:
	  i valori devono essere compresi da 1 alla dimensione massima (estremi inclusi)
	*/
	if( (x <= screen_width && y <= screen_height)  &&  (x>0 && y>0) ){	

		DrawChar(charMem[(Cx-1) + (Cy-1)*screen_width],Cx,Cy);

		Cx = x;
		Cy = y;
		
		unsigned short cursorchar = (unsigned char) ' ' | getFormedColor();
		DrawCursor(cursorchar,x,y);
	}
	
}

/*
Setto il Colore dello sfondo
*/
void VBEConsole::setBackColor(Color bkColor){

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
Color VBEConsole::getBackColor(){
	return (Color) ( (CColor & 0x7f) >>4 );
}

/*
Setto il Colore del testo
*/
void VBEConsole::setTextColor(Color txtColor){

	char mycolor = (char) txtColor;
	CColor = CColor & 0x70;
	CColor |= (mycolor & 0xF);
}

/*
Leggo il Colore del testo
*/
Color VBEConsole::getTextColor(){
	return (Color) ( CColor & 0xF );
}

/*
Leggo la larghezza dello schermo
*/
unsigned int VBEConsole::getWidth(){
	return 126; 
}

/*
Leggo l'altezza dello schermo
*/
unsigned int VBEConsole::getHeight(){
	return 48;
}




/*
Leggo la posizione del cursore dal lato sinistro dello schermo
*/
unsigned int VBEConsole::getX(){

	return Cx;
}



/*
Leggo la posizione del cursore dal lato sopra dello schermo
*/
unsigned int VBEConsole::getY(){

	return Cy;
}


/*
Disegno un carattere. vchar è un carattere nel formato della memoria video. - Funzione a 32 bit
*/
void VBEConsole::DrawChar32(unsigned short vchar, unsigned int vcharx, unsigned int vchary){

	char c = (char)vchar;

	unsigned char bcolor = ((vchar & 0xF000) >> 12);
	unsigned char tcolor = ((vchar & 0x0F00) >> 8);

	int n = 32 + 32*(vcharx-1);
	int sy = 80 + 16*(vchary-1);


	int vmemi = n+sy*1024*4;
	int fontb = 0;

	for(int pixb = 0; pixb <= 16; pixb++){
		int pix = 0;

		for(pix = 7; pix >= 0; pix--){
			if (test_bit(pix, &vgafont16[c*16+fontb]) == 1){
				VBE::vbevideomem[vmemi] = colors[tcolor*3+2];//0xFF;
				VBE::vbevideomem[vmemi+1] = colors[tcolor*3+1];//0xFF;
				VBE::vbevideomem[vmemi+2] = colors[tcolor*3];//0xFF;
			}else{
				VBE::vbevideomem[vmemi] = colors[bcolor*3+2];//0xFF;
				VBE::vbevideomem[vmemi+1] = colors[bcolor*3+1];//0xFF;
				VBE::vbevideomem[vmemi+2] = colors[bcolor*3];//0xFF;
			}
			vmemi += 4;
		}

		vmemi = (sy+pixb)*1024*4+n;
		fontb++;
	}

}

/*
Disegno un carattere. vchar è un carattere nel formato della memoria video. - Funzione a 24 bit
*/
void VBEConsole::DrawChar24(unsigned short vchar, unsigned int vcharx, unsigned int vchary){

	char c = (char)vchar;

	unsigned char bcolor = ((vchar & 0xF000) >> 12);
	unsigned char tcolor = ((vchar & 0x0F00) >> 8);

	int n = 24 + 24*(vcharx-1);
	int sy = 80 + 16*(vchary-1);

	/*
		******************************************
		**	  Codice NON ottimizzato	**
		******************************************
		
	int vmemi = n+sy*1024*3;
	int fontb = 0;

	for(int pixb = 0; pixb <= 16; pixb++){
		int pix = 0;

		for(pix = 7; pix >= 0; pix--){
			if (test_bit(pix, &vgafont16[c*16+fontb]) == 1){
				VBE::vbevideomem[vmemi] = colors[tcolor*3+2];
				VBE::vbevideomem[vmemi+1] = colors[tcolor*3+1];
				VBE::vbevideomem[vmemi+2] = colors[tcolor*3];
			}else{
				VBE::vbevideomem[vmemi] = colors[bcolor*3+2];
				VBE::vbevideomem[vmemi+1] = colors[bcolor*3+1];
				VBE::vbevideomem[vmemi+2] = colors[bcolor*3];
			}
			vmemi += 3;
		}

		vmemi = (sy+pixb)*1024*3+n;
		fontb++;
	}
	
		******************************************
		**     Fine Codice NON ottimizzato	**
		******************************************
	*/

	/*
	  Versione ottimizzata
	*/
	register int vmemi = n+sy*1024*3;
	register int fontb = 0;

	int c16 = c*16;
	int tcolor32 = tcolor*3+2;
	int tcolor31 = tcolor*3+1;
	int tcolor3 = tcolor*3;

	int bcolor32 = bcolor*3+2;
	int bcolor31 = bcolor*3+1;
	int bcolor3 = bcolor*3;

	if (c != 0){
		for(register int pixb = 0; pixb <= 16; pixb++){

			if (test_bit(7, &vgafont16[c16+fontb]) == 1){
				VBE::vbevideomem[vmemi] = colors[tcolor32];
				VBE::vbevideomem[vmemi+1] = colors[tcolor31];
				VBE::vbevideomem[vmemi+2] = colors[tcolor3];
			}else{
				VBE::vbevideomem[vmemi] = colors[bcolor32];
				VBE::vbevideomem[vmemi+1] = colors[bcolor31];
				VBE::vbevideomem[vmemi+2] = colors[bcolor3];
			}

			if (test_bit(6, &vgafont16[c16+fontb]) == 1){
				VBE::vbevideomem[vmemi+3] = colors[tcolor32];
				VBE::vbevideomem[vmemi+4] = colors[tcolor31];
				VBE::vbevideomem[vmemi+5] = colors[tcolor3];
			}else{
				VBE::vbevideomem[vmemi+3] = colors[bcolor32];
				VBE::vbevideomem[vmemi+4] = colors[bcolor31];
				VBE::vbevideomem[vmemi+5] = colors[bcolor3];
			}

			if (test_bit(5, &vgafont16[c16+fontb]) == 1){
				VBE::vbevideomem[vmemi+6] = colors[tcolor32];
				VBE::vbevideomem[vmemi+7] = colors[tcolor31];
				VBE::vbevideomem[vmemi+8] = colors[tcolor3];
			}else{
				VBE::vbevideomem[vmemi+6] = colors[bcolor32];
				VBE::vbevideomem[vmemi+7] = colors[bcolor31];
				VBE::vbevideomem[vmemi+8] = colors[bcolor3];
			}

			if (test_bit(4, &vgafont16[c16+fontb]) == 1){
				VBE::vbevideomem[vmemi+9] = colors[tcolor32];
				VBE::vbevideomem[vmemi+10] = colors[tcolor31];
				VBE::vbevideomem[vmemi+11] = colors[tcolor3];
			}else{
				VBE::vbevideomem[vmemi+9] = colors[bcolor32];
				VBE::vbevideomem[vmemi+10] = colors[bcolor31];
				VBE::vbevideomem[vmemi+11] = colors[bcolor3];
			}

			if (test_bit(3, &vgafont16[c16+fontb]) == 1){
				VBE::vbevideomem[vmemi+12] = colors[tcolor32];
				VBE::vbevideomem[vmemi+13] = colors[tcolor31];
				VBE::vbevideomem[vmemi+14] = colors[tcolor3];
			}else{
				VBE::vbevideomem[vmemi+12] = colors[bcolor32];
				VBE::vbevideomem[vmemi+13] = colors[bcolor31];
				VBE::vbevideomem[vmemi+14] = colors[bcolor3];
			}

			if (test_bit(2, &vgafont16[c16+fontb]) == 1){
				VBE::vbevideomem[vmemi+15] = colors[tcolor32];
				VBE::vbevideomem[vmemi+16] = colors[tcolor31];
				VBE::vbevideomem[vmemi+17] = colors[tcolor3];
			}else{
				VBE::vbevideomem[vmemi+15] = colors[bcolor32];
				VBE::vbevideomem[vmemi+16] = colors[bcolor31];
				VBE::vbevideomem[vmemi+17] = colors[bcolor3];
			}

			if (test_bit(1, &vgafont16[c16+fontb]) == 1){
				VBE::vbevideomem[vmemi+18] = colors[tcolor32];
				VBE::vbevideomem[vmemi+19] = colors[tcolor31];
				VBE::vbevideomem[vmemi+20] = colors[tcolor3];
			}else{
				VBE::vbevideomem[vmemi+18] = colors[bcolor32];
				VBE::vbevideomem[vmemi+19] = colors[bcolor31];
				VBE::vbevideomem[vmemi+20] = colors[bcolor3];
			}

			if (test_bit(0, &vgafont16[c16+fontb]) == 1){
				VBE::vbevideomem[vmemi+21] = colors[tcolor32];
				VBE::vbevideomem[vmemi+22] = colors[tcolor31];
				VBE::vbevideomem[vmemi+23] = colors[tcolor3];
			}else{
				VBE::vbevideomem[vmemi+21] = colors[bcolor32];
				VBE::vbevideomem[vmemi+22] = colors[bcolor31];
				VBE::vbevideomem[vmemi+23] = colors[bcolor3];
			}

			vmemi = (sy+pixb)*1024*3+n;
			fontb++;
		}
	}else{
		for(register int pixb = 0; pixb <= 16; pixb++){
			VBE::vbevideomem[vmemi] = colors[tcolor32];
			VBE::vbevideomem[vmemi+1] = colors[tcolor31];
			VBE::vbevideomem[vmemi+2] = colors[tcolor3];
			VBE::vbevideomem[vmemi+3] = colors[tcolor32];
			VBE::vbevideomem[vmemi+4] = colors[tcolor31];
			VBE::vbevideomem[vmemi+5] = colors[tcolor3];
			VBE::vbevideomem[vmemi+6] = colors[tcolor32];
			VBE::vbevideomem[vmemi+7] = colors[tcolor31];
			VBE::vbevideomem[vmemi+8] = colors[tcolor3];
			VBE::vbevideomem[vmemi+9] = colors[tcolor32];
			VBE::vbevideomem[vmemi+10] = colors[tcolor31];
			VBE::vbevideomem[vmemi+11] = colors[tcolor3];
			VBE::vbevideomem[vmemi+12] = colors[tcolor32];
			VBE::vbevideomem[vmemi+13] = colors[tcolor31];
			VBE::vbevideomem[vmemi+14] = colors[tcolor3];
			VBE::vbevideomem[vmemi+15] = colors[tcolor32];
			VBE::vbevideomem[vmemi+16] = colors[tcolor31];
			VBE::vbevideomem[vmemi+17] = colors[tcolor3];
			VBE::vbevideomem[vmemi+18] = colors[tcolor32];
			VBE::vbevideomem[vmemi+19] = colors[tcolor31];
			VBE::vbevideomem[vmemi+20] = colors[tcolor3];
			VBE::vbevideomem[vmemi+21] = colors[tcolor32];
			VBE::vbevideomem[vmemi+22] = colors[tcolor31];
			VBE::vbevideomem[vmemi+23] = colors[tcolor3];

			vmemi = (sy+pixb)*1024*3+n;
			fontb++;
		}
	}
}

/*
Disegno il cursore - Funzione a 32 bit
*/
void VBEConsole::DrawCursor32(unsigned short vchar, unsigned int vcharx, unsigned int vchary){

	unsigned char tcolor = ((vchar & 0x0F00) >> 8);

	int n = 32 + 32*(vcharx-1);
	int sy = 80 + 16*(vchary-1);


	int vmemi = n+sy*1024*4;
	int fontb = 0;

	for(int pixb = 0; pixb <= 16; pixb++){
		int pix = 0;

		for(pix = 7; pix >= 0; pix--){
			VBE::vbevideomem[vmemi] = colors[tcolor*3+2];
			VBE::vbevideomem[vmemi+1] = colors[tcolor*3+1];
			VBE::vbevideomem[vmemi+2] = colors[tcolor*3];
			vmemi += 4;
		}

		vmemi = (sy+pixb)*1024*4+n;
		fontb++;
	}

}

/*
Disegno il cursore - Funzione a 24 bit
*/
void VBEConsole::DrawCursor24(unsigned short vchar, unsigned int vcharx, unsigned int vchary){

	unsigned char tcolor = ((vchar & 0x0F00) >> 8);

	int n = 24 + 24*(vcharx-1);
	int sy = 80 + 16*(vchary-1);


	int vmemi = n+sy*1024*3;
	int fontb = 0;

	for(int pixb = 0; pixb <= 16; pixb++){
		int pix = 0;

		for(pix = 7; pix >= 0; pix--){
			VBE::vbevideomem[vmemi] = colors[tcolor*3+2];
			VBE::vbevideomem[vmemi+1] = colors[tcolor*3+1];
			VBE::vbevideomem[vmemi+2] = colors[tcolor*3];
			vmemi += 3;
		}

		vmemi = (sy+pixb)*1024*3+n;
		fontb++;
	}

}
