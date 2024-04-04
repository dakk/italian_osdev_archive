#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

 FILE *f1;
 char *nomefile,*opz,Opz;
 unsigned char buffer[1000];
 char caratt[256],lastcarat=0,OK=0;
 int t,t1;
 unsigned char lastword[1000],commento=0,livello=0;
 unsigned short nword=0;
/*
    1:lettere
    2:numeri
    4:stampabili
    8:parentesi tonda aperta

*/
int main ( int argc,char **argv) {
  memset (caratt,4,256);
  for (t=0;t<33;t++) caratt[t]=0;
  for (t=65;t<91;t++) caratt[t]=1;
  for (t=97;t<123;t++) caratt[t]=1;
  for (t=48;t<58;t++) caratt[t]=2;
  caratt['(']=8;
  caratt['_']=1;
  if (argc!=3) {printf("Sintassi: %s <nomefile> opz\n",argv[0]);exit(0);}
  opz=argv[2];
  Opz=0;
  for (;*opz;opz++) {
    if (*opz=='0') Opz|=1;
  }
  f1=fopen(nomefile=argv[1],"r");
  lastcarat=0;
  while (fgets(buffer,1000,f1)) {
     OK=0;
     if (strstr(buffer,"#define")==NULL)
      for (t=0;t<(strlen(buffer)-1);t++) {
        if ((buffer[t]=='/')&&(buffer[t+1]=='*')) commento=1;
        if ((buffer[t+1]=='/')&&(buffer[t]=='*')) commento=0;
        if (!commento) {
           if (buffer[t]=='{') livello++;
           if (buffer[t]=='}') livello--;
           if (caratt[buffer[t]]&3) 
              { lastword[nword]=buffer[t];lastword[nword+1]=0;nword++;} 
           else nword=0;
           if ((lastcarat&3)&&(caratt[buffer[t]]&8)) {
             if (strcmp(lastword,"switch")) 
             if (strcmp(lastword,"free")) 
             if (strcmp(lastword,"printf")) 
             if (strcmp(lastword,"malloc")) 
             if (strcmp(lastword,"if")) 
             if (strcmp(lastword,"while")) 
             if (strcmp(lastword,"for")) 
                 OK=1;
           }
           if (OK) { 
               if (Opz&1) {
                 if (livello==0) printf("%s()\n",lastword);	
               } else {
                 for (t1=0;t1<livello;t1++) printf(" "); 
                 printf("%s()\n",lastword); 
               }
               OK=0;
           }
           if (caratt[buffer[t]]) lastcarat=caratt[buffer[t]];
        }
     }
  }
  fclose(f1);
  return 1;
}

