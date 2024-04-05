/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: video.h			|
	| Date: 30/01/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _HARDWARE_DRIVER_VIDEO_H_
#define _HARDWARE_DRIVER_VIDEO_H_


/*
	**************************
	**	 Include	**
	**************************
*/

#include <kdef.h>


namespace hardware{

	namespace driver{
	
		namespace video{
		
			//Enum che contiene i Colori supportati + il lampeggio
			enum Color{
				Black,
				Blue,
				Green,
				Cyan,
				Red,
				Magenta,
				Brown,
				White,
				DarkGrey,
				BrightBlue,
				BrightGreen,
				BrightCyan,
				BrightRed,
				BrightMagenta,
				Yellow,
				BrightWhite,
				Blink = 128
			};
		
			
			class Video{
			public:
				static void Init(); //Inizializzo i driver video
			
				static void Clear(); //Pulisco il video
				static void Clear(Color bkColor); //Pulisco il video utilizzando un Colore
				
				static void Print(const char *c); //Visualizzo a video una stringa
				static void Print(const char c); //Visualizzo a video un carattere
				
				static void NewLine(); //Nuova linea
				
				static void GotoXY(unsigned int x, unsigned int y); //Posiziono il cursore in un punto dello schermo
				
				static void setBackColor(Color bkColor); //Setto il Colore dello sfondo
				static Color getBackColor(); //Leggo il Colore dello sfondo
				
				static void setTextColor(Color txtColor); //Setto il Colore del testo
				static Color getTextColor(); //Leggo il Colore del testo
				
				static unsigned int getWidth(); //Leggo la larghezza dello schermo
				static unsigned int getHeight(); //Leggo l'altezza dello schermo
				
				static unsigned int getX(); //Leggo la posizione del cursore dal lato sinistro dello schermo
				
				static unsigned int getY(); //Leggo la posizione del cursore dal lato sopra dello schermo
				
			private:
				static unsigned short *videomem; //Memoria video

				static unsigned char CColor; //Colore in uso

				static int Cy; //Posizione del cursore
				static int Cx; //Posizione del cursore
	
				static unsigned short int Video::getFormedColor(); //Converto il Colore in uso in un 'unsigned short int'
			};
		
		}
	}
}

#endif
