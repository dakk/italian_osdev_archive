/*
 * OSSO - A Micro Kernel OS
 * Copyright (c) 2000 Alessandro Iurlano.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

// A little test program drawing a Julia Fractal image.
// OSSO must activate graphics mode for it to work. It is done while in 
// realmode via int 13h

//#include <math.h>
#include <types.h>

double cos(double kk);
double sin(double kk);

/* Dimensioni dell'immagine */
#define IMAGE_X 320
#define IMAGE_Y 200

#define _32_Bits_per_Pixel

typedef byte TImage[IMAGE_Y][IMAGE_X];

TImage * pimage;

int main(void) {
  int i,x,y;
  double ppy,px,py,t;
  double cx,cy;
  int angle;

  cx=0;
  cy=-1;
  /* Prende il display di default */
  /* Alloca memoria per l'immagine */
  pimage=0xA0000;
      
  _printf("JULIA\n"); /* Hello World ! */
  _printf("Inizio calcolo...\n");
  while (1) {
    double px2,py2;
    int k;
    for (angle=-180;angle<180;angle+=3) {
      cx=-0.6*cos((3.141*angle)/180);
      cy=1.6*sin((3.141*angle)/180);
      //    cx=0.4;
      //cy=1.2;
      /*cx e cy sono i parametri dell'insieme di julia
	sono le componenti di un numero complesso 
	c = cx + cyi

	la formula di julia e': scelto il valore di c,
	va calcolato Z = Z ^ 2 + c
	finche' il modulo di Z supera 2 (o un certo numero di iterazioni)
	dove Z e' il numero complesso Z=px+py*i
	cosicche' Z^2 = px^2 - py^2 + (2*px*py)*i
	Ogni punto dell'immagine corrisponde ad un valore iniziale diverso
	di px e py
	il colore del punto e' dato dal numero di iterazioni.
      */


      for (x=0;x<IMAGE_X;x++) {
	ppy=((4.0*x)/IMAGE_X) -2.0;
	for (y=0;y<(IMAGE_Y>>1)+1;y++) {
	  i=0;
	  py=ppy;
	  px=((4.0*y)/IMAGE_Y)-2.0;
	  px2=px*px;
	  py2=py*py; /* Ci sono un po' di ottimizzazioni */
	  while ((px2+py2<4)&&(i<64)) {
	    t=2*px*py+cy;
	    px=px2-py2+cx;
	    py=t;
	    px2=px*px;
	    py2=py*py;
	    i++;
	  } 
	  k=i<<2; /* il colore e' RGB
		     non avevo voglia di perarmici, cosi' sono
		     andato a caso.... */
	  (*pimage)[y][x]=k;
	  /*Piccola ottimizzazione: sfrutta il fatto che Julia e' simmetrico
	    rispetto all'origine */
	  (*pimage)[IMAGE_Y-y][IMAGE_X-x]=k;
	}
      }
    }
  }
}


