/*
     Pallo

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/* io.c */
#include "io.h"

unsigned char chz,chzold;
unsigned long prima[10]={0,0,0,0,0,0,0,0,0,0},adesso=0;
unsigned char leggichar();


void Delay(int mt,long msec) {
  prima[mt] = adesso;
  while (adesso<(msec+prima[mt])) {
     gettimeofday(&tv,&tz);
     adesso=tv.tv_sec*1000+(tv.tv_usec/1000);
  }
}



u_char ogni(int mt,long msec) {
  gettimeofday(&tv,&tz);
  adesso=tv.tv_sec*1000+(tv.tv_usec/1000);
  if (prima[mt] == 0) prima[mt]=adesso;
  if (adesso>=(msec+prima[mt])) {
    prima[mt]=adesso;
    return 1;
  }
  return 0;
}


void termina() {};

unsigned char leggichar() {
 char ch;
 if (!kbhit()) {
   if ((ch=getchar())==27)
   if (((ch=getchar())=='O')||(ch=='[')) 
     switch (ch=getchar()) {
      case 'A': return 200;break;
      case 'B': return 208;break;
      case 'C': return 205;break;
      case 'D': return 203;break;
     }
  return ch;
 } else return '#';
}
void svuota_buff(u_char nnn) {
 while (!ogni(0,100)) leggichar();
 while (!ogni(0,1000)) leggichar();
}


void terminale() {
  u_char buf[10];
  bzero(buf,10);
  ioctl(1,0x5413,buf);
  printf("Rilevato un terminale con %u righe e %u colonne.\n",buf[0],buf[2]);  
  if ((buf[0]<25)||(buf[2]<80)) {printf("Deve essere minimo 25 righe e 80 colonne.\n"); exit(-1);}
}
