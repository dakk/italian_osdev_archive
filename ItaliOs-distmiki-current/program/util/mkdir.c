#include <stdio.h>
#include <stdlib.h>
#include <fat12.h>
#define f_setta_disk
#define f_mkdir
#define f_path_to_num
#include "fat12.inc"


int main(int argc,char **argv) {
  int curpos;
  u_char *nomedir;
  setta_disk();
  if (psr<1) { printf("disco floppy errato\n"); return -1;}
  if (argc != 2) return -1;
  curpos = path_to_num(nomedir=getenv("PATH"));
  free(nomedir);
  nomedir = argv[1];
  mkdir(curpos,nomedir);
  return 0;
}

