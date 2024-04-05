/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: vbeconsole.h		|
	| Date: 18/06/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

#ifndef _HARDWARE_DRIVER_VBECONSOLE_H
#define _HARDWARE_DRIVER_VBECONSOLE_H

#include <kdef.h>
#include <hardware/driver/video.h>

namespace hardware{

	namespace driver{
	
		namespace video{

			class VBEConsole{
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
				static unsigned short charMem[128*128];
				static int Cy;
				static int Cx;
				static unsigned char CColor;
				static unsigned short int getFormedColor();
	
				static void (*DrawChar)(unsigned short vchar, unsigned int vcharx, unsigned int vchary);
				static void DrawChar32(unsigned short vchar, unsigned int vcharx, unsigned int vchary);
				static void DrawChar24(unsigned short vchar, unsigned int vcharx, unsigned int vchary);

				static void (*DrawCursor)(unsigned short vchar, unsigned int vcharx, unsigned int vchary);
				static void DrawCursor32(unsigned short vchar, unsigned int vcharx, unsigned int vchary);
				static void DrawCursor24(unsigned short vchar, unsigned int vcharx, unsigned int vchary);
			};
		}
	}
}

#endif
