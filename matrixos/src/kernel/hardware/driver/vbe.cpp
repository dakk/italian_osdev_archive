/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: vbe.cpp			|
	| Date: 30/01/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

/*
	**************************
	**	 Include	**
	**************************
*/

#include <hardware/driver/vbe.h>
#include <kservices/kdbg.h>
#include <hardware/cpu/bitops.h>
#include <memory/paging.h>

#include <kdef.h>

/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace hardware::driver::video;



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////



//Blocco informazioni VESA BIOS EXTENSION (VBE)
VBEModeInfoBlock *VBE::vbeinfo;

//Memoria Video.
unsigned char *VBE::vbevideomem;

unsigned int VBE::bpp;

//Puntatori alle funzioni grafiche.
void (*VBE::Clear)(char r, char g, char b);
void (*VBE::DrawFillRectangle)(int x, int y, int width, int height, char r, char g, char b);
void (*VBE::DrawImage)(int width, int height, const unsigned char *pixeldata);

/*
Inizializzo i driver VBE.
*/
bool VBE::Init(unsigned long info_mode_addr){
	//Inizializza i puntatori alla struttura vbeinfo e alla memoria video
	vbeinfo = (VBEModeInfoBlock *) info_mode_addr;
	vbevideomem = (unsigned char *) vbeinfo->physical_base_ptr;

	//Controllo se VBE e' stato abilitato
	//if (vbeinfo->signature[0] != 'V' || vbeinfo->signature[0] != 'E' || vbeinfo->signature[0] != 'S' || vbeinfo->signature[0] != 'A') return false;
	
	//Inizializzo i puntatori in base se la scheda video e a 32 o a 24 bpp
	if (vbeinfo->bits_per_pixel == 32){
		#ifdef VBE_DEBUG
			kservices::KDbg::WriteLine("vbe.cpp - bits per pixel = 32bpp");
		#endif

		Clear = Clear32;
		DrawFillRectangle = DrawFillRectangle32;
		DrawImage = DrawImage32;
		bpp = 32;
	}else{
		#ifdef VBE_DEBUG
			kservices::KDbg::WriteLine("vbe.cpp - bits per pixel = 24bpp");
		#endif
		
		Clear = Clear24;
		DrawFillRectangle = DrawFillRectangle24;
		DrawImage = DrawImage24;
		bpp = 24;
	}

	//Pulisco lo schermo grigio=0x90 nero=0x00
	Clear(0x00,0x00,0x00);

	return true;
}

/*
24bpp - Pulisco lo schermo utilizzando un colore.
*/
void VBE::Clear24(char r, char g, char b){
	int msize = vbeinfo->bytes_per_scanline * vbeinfo->y_resolution;

	for(int mvideo = 0; mvideo < msize; mvideo+=3){
		vbevideomem[mvideo] = b;
		vbevideomem[mvideo+1] = g;
		vbevideomem[mvideo+2] = r;
	}
}

/*
32bpp - Pulisco lo schermo utilizzando un colore.
*/
void VBE::Clear32(char r, char g, char b){
	int msize = vbeinfo->bytes_per_scanline * vbeinfo->y_resolution;

	for(int mvideo = 0; mvideo < msize; mvideo+=4){
		vbevideomem[mvideo] = b;
		vbevideomem[mvideo+1] = g;
		vbevideomem[mvideo+2] = r;
	}
}

/*
24bpp - Disegno un rettangolo pieno.
*/
void VBE::DrawFillRectangle24(int x, int y, int width, int height, char r, char g, char b){
	int linenum;

	for(linenum = 0; linenum <= height; linenum++){
		int startpixel = x*3 + vbeinfo->bytes_per_scanline * (y+linenum);
		int endpixel = (x+width)*3 + vbeinfo->bytes_per_scanline * (y+linenum);

		for(int i = startpixel; i <= endpixel; i+=3){
			vbevideomem[i] = b;
			vbevideomem[i+1] = g;
			vbevideomem[i+2] = r;
		}
	}
}

/*
32bpp - Disegno un rettangolo pieno.
*/
void VBE::DrawFillRectangle32(int x, int y, int width, int height, char r, char g, char b){
	int linenum;

	for(linenum = 0; linenum <= height; linenum++){
		int startpixel = x*4 + vbeinfo->bytes_per_scanline * (y+linenum);
		int endpixel = (x+width)*4 + vbeinfo->bytes_per_scanline * (y+linenum);

		for(int i = startpixel; i <= endpixel; i+=4){
			vbevideomem[i] = b;
			vbevideomem[i+1] = g;
			vbevideomem[i+2] = r;
		}
	}
}

/*
24bpp - Disegno un'ìmmagine.
La funzione disegna l'immagine a partire dal angolo in alto a sinistra. E utilizza immagini BGR.
*/
void VBE::DrawImage24(int width, int height,const unsigned char *pixeldata){
        int z;

        for(int y = 0; y < height; y++){
     		for(int x = 0; x < width*3; x++){

			vbevideomem[z] = pixeldata[x+y *width*3];
                        z++;
        	}

		z = y*1024*3;
        }
}

/*
24bpp - Disegno un'ìmmagine.
La funzione disegna l'immagine a partire dal angolo in alto a sinistra. E utilizza immagini BGR.
*/
void VBE::DrawImage32(int width, int height,const  unsigned char *pixeldata){
        int z;
	int bpp = 0;
        for(int y = 0; y < 74; y++){
     		for(int x = 0; x < 256*3; x++){

			vbevideomem[z] = pixeldata[x+y *width*3];
                        z++;
			bpp++;
			if (bpp == 3){
				z++;
				bpp = 0;
			}
        	}
		z = y*1024*4;
        }
}

/*
  Viene chiamata dopo aver abilitato il paging.
*/
void VBE::ReInit(){
	memory::Paging::PhyscalAlloc((dword) vbeinfo->physical_base_ptr, 1048576*8);
}
