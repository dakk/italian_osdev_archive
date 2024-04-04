#include <stdio.h>
#include <stdlib.h>
#include <fat12.h>
#define f_leggifile
#define f_file
#include "fat12.inc"


void cp(char *nome,char *nome1) {
  FILE *f1,*f2;
  int t,t1=0,t2=0;
  u_char buf[200],*ss1;
/*  printf("%s -> %s\n",nome,nome1); */
  for (t=0;nome[t];t++) {
    if (nome[t] == '/') t1 = t+1;
  } 
  dircorrente = path_to_num(ss1=getenv("PATH")); 
  free(ss1); 
  if (path_to_num(nome) == -5) {  /* nome del file esiste */
/*    printf("arg1 %s esiste\n",nome); */
    if ((t = path_to_num(nome1)) == -6) {  /* 2^ arg non esiste */
/*      printf("arg2 %s non esiste\n",nome1); */
       f1 = fopen(nome,"r"); t2=f1->dir;
       f2 = fopen(nome1,"w");
    } else
    if (t>0) {                             /* 2^arg e' una dir */
/*       printf("arg2 %s e' una dir\n",nome1); */
       f1 = fopen(nome,"r");
/*       printf("-->%s\n",nome); */
       t2 = f1->dir;
       nome += t1;
       strncpy(buf,nome1,t=strlen(nome1));
       buf[t] = '/';
       strncpy(buf+t+1,nome,strlen(nome)+1);
/*       printf("-->%s\n",buf); */
       f2 = fopen(buf,"w");
    }
/*   printf("Open ok\n"); */
   while ((t=fread(buf,200,1,f1)) > 0) fwrite(buf,t,1,f2);
/*   printf("Fr Fw ok\n"); */
   fclose(f1);
   fclose(f2);
   printf(" File copiato.\n");

  }
   
}



int main(int argc,char **argv) {
    if (argc != 3) return -1;
    cp(argv[1],argv[2]);
    printf("OK\n");
    return 0;
}
