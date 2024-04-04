#include <stdio.h>
#include <stdlib.h>
#include <fat12.h>
#define f_leggifile
#define f_file
#include "fat12.inc"


int main(int argc,char **argv) {
  FILE *f1;
  char ss[100],*ss1;
  setta_disk();
  dircorrente = path_to_num(ss1=getenv("PATH"));
  free(ss1);
  if (argc != 2) return -1;
  if ((ss1 = argv[1]) == NULL) return -1;
  if ((f1 = fopen(ss1,"r")) == NULL) return -1;
  while (fread(ss,100,1,f1) != NULL) {
     printf("%s\n",ss);
  }
  fclose(f1);
  return 1;
}

