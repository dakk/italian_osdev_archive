#include <stdio.h>
#include <stdlib.h>
#include <fat12.h>
#define f_cercafile
#define f_file
#include "fat12.inc"

u_char path[300];
int npat = 0;

int main(int argc,char **argv) {
  FILE *f1;
  char ss1[100];
  int t=0;
  if (argc != 2) return -2;  
  if (cercafile(ss1,"/aiuto",argv[1]) != 0) return -1;
  f1 = fopen(ss1,"r+");
  while (t<40) {
    for (t=0;(t<20) && (fgets(ss1,100,f1) != NULL);t++) {
      printf("%s\n",ss1);
    }
    if (t<20) break;
    printf("---- premi un tasto per continuare ----\r");
    getchar();
  }  
  fclose(f1);
  return 0;
}

