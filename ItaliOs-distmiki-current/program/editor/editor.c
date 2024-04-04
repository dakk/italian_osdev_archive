#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fat12.h>
#define f_leggifile
#define f_file
#define f_setta_disk
#include "fat12.inc"

 #define NOBG 100
 #define ASPETTA 0
 #define NON_ASPETTA 1
#ifndef SU
 #define SU 193
 #define GIU 194
 #define DX 195
 #define SX 196
 #define PF1 208
 #define PF2 209
 #define PF3 210
 #define PF4 211
 #define DEL 127
 #define HOME 177
 #define INS 178
 #define CANC 179
 #define FINE 180
 #define PGSU 181
 #define PGGIU 182
#endif

#ifndef u_char
 #define u_char unsigned char
#endif
#define PASSI 16384
#define SIZE_BUFF 32768
#define SIZE_LIN 16384
#define SIZE_PASSI 100
#define POS (linee[nll+y]+x)

  FILE *f1,*f2;
  int t,NLINEE,nlin=0,pnt1=0,pnt2=0,lf;
  u_char *buf = 2,ch;
  char *nomefile;
  unsigned int *passi = 0;
  unsigned short *linee = 1; 
  unsigned char x=0,x1,y=0;
  char *HeLP="PF1=esci PF2=annulla PF4=salva";
/*  struct winsize Win; */
  int nll=0,oldy=0;



unsigned char leggichar(unsigned char dl) {
 char ch;
 if (kbhit) return '#'; 
 if ((ch=getchar())==27)
  if (((ch=getchar())=='O')||(ch=='[')) 
    {
     if ((ch=getchar())<64) getchar();
     if (ch=='[') return (getchar()+143);
     return (ch+128);
    } 
 return ch;
}



void marca() {
  int mr1;
  nlin=0;
  linee[nlin++]=0;
  for (mr1=1;mr1<lf;mr1++)
    if (buf[mr1-1]==0x0a) linee[nlin++]=mr1;    
}

void visual() {
  int vt,vt1=0,vt2,vt3=0;
  printf("\033[2J\033[1;1H");
  if (nlin>nll) vt1=linee[nll];
  if (nlin>(nll+NLINEE+1)) vt2=linee[nll+NLINEE+1]; else vt2=lf;

  for (vt=vt1;vt<vt2;vt++) {
    if (vt3) {
     if (buf[t]=='{') vt3++;
     if (buf[t]=='}') vt3--;
     if (!vt3) {vt3=-1;printf("\033[1;33m");}
    }
    if ((vt==POS)&&(buf[vt]=='{')) vt3=1;
    if ((buf[vt]>31)||(buf[vt]==10)||(buf[vt]==13)||(buf[vt]==9)) printf("%c",buf[vt]); else return;
    if (vt3==-1) {vt3=0;printf("\033[m");}
  }
/*  fflush(stdout);   */
}

void salvabuf() {
  char *ss1;
  dircorrente = path_to_num(ss1=getenv("PATH"));
  free(ss1);
  cancfile(dircorrente,nomefile);
  f1=fopen(nomefile,"a+");
  fseek(f1,0,SEEK_SET);
  fwrite(buf,1,lf,f1);
  fclose(f1);
}
void caricabuf() {
  f1=fopen(nomefile,"a+");
  fseek(f1,0,SEEK_SET);
  lf=fread(buf,1,PASSI,f1);
  fclose(f1);
}


void edita() {
#define FINELINEA (linee[y+1+nll]-linee[y+nll]-1)
  NLINEE=23;
  marca(); visual();
  ch='#';
  while (ch!=PF1) {
/*     ioctl(1,(int)0x5413,(char *)&Win);
     NLINEE=Win.ws_row-2;*/

     if (lf>(SIZE_BUFF-10)) salvabuf();
     ch=leggichar(ASPETTA);
     oldy=y+nll;
     switch(ch) {
       case PGSU : if (y<9) { y=0; nll-=10*(nll>9); visual(); } else y-=10;/*if (x>FINELINEA) x=FINELINEA;*/break;
       case PGGIU : if (y>=(NLINEE-9)) { y=NLINEE; nll+=10; visual(); } else y+=10;/*if (x>FINELINEA) x=FINELINEA;*/break;
       case SU : if (y==0) { y=0; nll-=(nll>0); visual(); } else y--;break;
       case GIU : if (y>=NLINEE) { y=NLINEE; nll++; visual(); } else y++;/*if (x>FINELINEA) x=FINELINEA;*/break;
       case DX : 
           x+=(x<FINELINEA);break;
       case SX : x-=(x>0);break;
       }
     switch(ch) {
       case FINE : x=FINELINEA;break;
       case DEL : if ((x>0)||(y>0)) {
                   for (t=linee[y+nll]+x;t<lf;t++) buf[t-1]=buf[t];
                   if (x>0) x--; else { y--; x=FINELINEA; }
                   lf--;marca();visual();
                  } 
                break;
       case CANC : for (t=linee[y+nll]+x;t<lf;t++) buf[t]=buf[t+1];
                   lf--;marca();visual();
                break;
       case PF3 : caricabuf();break;
       case PF4 : salvabuf();break;
       default: if (lf<SIZE_BUFF) {
                   for (t=lf;t>(linee[y+nll]+x);t--) buf[t]=buf[t-1];
                   buf[linee[y+nll]+x]=ch;
                   lf++;marca();visual();
                   x++;
                  } 
     }
     while (linee[y+nll]>lf) y--;
     if (oldy!=(y+nll)) if (x>FINELINEA) x=FINELINEA;
     if (buf[POS]=='{') visual();
     x1=0;for (t=0;t<x;t++) 
       if (buf[linee[y+nll]+t]>31) x1++; else
       if (buf[linee[y+nll]+t]==9) x1=(++x1+8)&0xf8;         /* carattere tab */
  printf("\033[44;37m\033[%i;1Hsizebuff=%i tasto=% 3i\033[44;39m%s\033[K\033[m\033[%i;%iH"
  ,NLINEE+2,lf,ch,HeLP,y+1,x1+1);
  }
}



int main(int argc,char **argv) {
  setta_disk();
  printf("setta_disk() OK\n");
  if (PASSI>=SIZE_BUFF) { printf("var PASSI > SIZE_BUFF\nerrore!!\n");return -1;}
  buf = linee = passi = 0;
  printf("valori = %x %x %x\n",buf,linee,passi);
  buf = malloc(SIZE_BUFF);
  printf("valori = %x %x %x\n",buf,linee,passi);
  linee = malloc(SIZE_LIN*2); 
  printf("valori = %x %x %x\n",buf,linee,passi);
  passi = malloc(SIZE_PASSI*4);
  printf("valori = %x %x %x\n",buf,linee,passi);
/* da qua va in int14 */
  memset(buf,0,SIZE_BUFF);
  memset(linee,0,SIZE_LIN*sizeof(unsigned short));
  memset(passi,0,SIZE_PASSI*sizeof(unsigned int));
  printf("Malloc varie OK\n");
  if (argc!=2) {
    printf("Editor di testi.\nSintassi: %s <nomefile>\n\n",argv[0]);
    return 0;
  }
  nomefile=argv[1];
  printf("1\n");
  caricabuf();   
  printf("2\n");
  edita();
  
  free(buf);
  free(linee);
  free(passi);
  return 0;
}
