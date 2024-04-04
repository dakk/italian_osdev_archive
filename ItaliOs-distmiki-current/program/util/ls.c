
#include <stdio.h>
#include <stdlib.h>
#include <fat12.h>
#define f_leggifile
#include "fat12.inc"


u_char path[300];
int npat = 0;


int main() {
  FILE filee1;
  int t;
  char *path=getenv("PATH");
/*  if (path == NULL) {
     path[0] = '/'; path[1] = 0;
     setenv("PATH",path,1);
     path="/";
  }*/
  printf("path=%s\n",path);
  t = path_to_num(path);
  free(path);
  filee1.nome[0] = 0;
  t=leggifile(t,&filee1);
  while (filee1.nome[0]) {
    if (t<0) break;
    printf("%02i:%02i:%04i  %02u:%02i ",
    filee1.date & 0x1f,(filee1.date >> 5) & 0xf,((filee1.date >> 9) & 0x3f)+1980, 
    filee1.time >> 11,(filee1.time >> 5) & 0x3f);
    if (filee1.attr & 0x10) printf("  <DIR> %s/\n",filee1.nome); else printf(" %i %s\n",filee1.size,filee1.nome);
    t=leggifile(t,&filee1);
  }
/*  printf("%i\n",_errore_(t)); */
  return 0;
}


