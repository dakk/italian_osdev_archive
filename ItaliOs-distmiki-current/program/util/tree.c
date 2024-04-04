#include <stdio.h>
#include <stdlib.h>
#include <fat12.h>
#define f_leggifile
#define f_file
#include "fat12.inc"


u_char path[300];
int npat = 0;


int main() {
  FILE f1;
  char *ss1;
  int tt[10],ntt,t;
  if ((tt[0] = path_to_num(ss1=getenv("PATH"))) <0) {
    free(ss1);
    return 0;
  }  
  free(ss1);
  ntt=0;
  f1.nome[0] = 0;
  while (ntt>=0) {
      tt[ntt] = leggifile(tt[ntt],&f1);
      if (tt[ntt] < 0 ) { f1.nome[0] = 1;ntt--; continue; }
      if (f1.nome[0] == '.') continue;
      for (t=0;t<ntt;t++) printf("   ");
      printf("%s",f1.nome);
      if (f1.attr & 0x10) {
         printf("/\n"); 
         ntt++;
         tt[ntt] = (f1.clust+nfat-2)*512;
      } else printf("\n");
      
  }
  return 0;
}

