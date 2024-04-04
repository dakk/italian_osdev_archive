#include <stdio.h>
#include <stdlib.h>
#include <fat12.h>
#define f_setta_disk
#define f_path_to_num
#define f_num_to_path
#define f_leggifile
#include "fat12.inc"


u_char path[300];
int npat = 0;


int main(int argc,char **argv) {
  int t;
  char *ss1;
  setta_disk();
  dircorrente = path_to_num(ss1=getenv("PATH"));
  free(ss1);
  if (dircorrente < 0) return -1;
  t=path_to_num(argv[1]);
  num_to_path(t,path);
  setenv("PATH",path,1);
  return 1;
}

