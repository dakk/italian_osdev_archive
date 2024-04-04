#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3

#define FILE struct tFile
struct tFile {
  char nome[13];
  u_short time,date;
  u_char attr;
  int flag;   /* 1=lettura 2=scrittura 4=append */
  int dir;   /* posizione della dir dove si trova il file */
  int size;   /* dimensione del file */
  int posiz;   /* posizione attuale */
  u_short clust;  /* cluster di partenza */
  u_short clust_corr;  /* cluster corrente */
};


int setta_disk() {
  u_char buf[512];

/* f0ffff 034000 056000 078000 */
/*  printf("buf24=%u\n",*(u_short *)(buf+24)); */
  if (!read_block(0,buf,1)) { printf("Errore"); return -1; }/* boot sect */
/* 03 40 00 00000011 0100000 00000000 */  
/*  printf("buf24=%u\n",*(u_short *)(buf+24)); */
  if ((buf[0] != 0xeb) || (buf[2] != 0x90)) return -2;
  if (*(u_short *)(buf+11) != 512) return -3;
  if (*(u_short *)(buf+24) != 18) return -4;
  if (*(u_short *)(buf+26) != 2) return -5;
  psr=1+(1+ (4320/(*(u_short *)(buf+11))/buf[13]) )*buf[16];
  nfat=psr+*(u_short *)(buf+17)/16;
/* ((2880/2*3/(byte per settore)/(settori per cluster)+1)*numero fat)+1   */
  if (!read_block(1,buf,1)) printf("Errore"); /* fat */
  if (buf[0] == 0xf8) {printf("Fat a 16bit\n");return -6;}
  if (buf[0] != 0xf0) return -7;
  return psr;
}



int intfat12(int nn) {
  int knn,znn;
  u_char nnn[3];
  u_char buf[512];
  if (nn < nfat) {
     if (nn == (nfat -1)) return -1; 
     if (nn < psr) return -4;
     return nn+1;
  } else nn -= (nfat-2);
  if ((nn<2) && (nn>=(2880/512))) return 4;
/* nn/2*3 -> progressivo byte della fat */
/* nn/2*3/512 -> settore della fat interessato */  
/*  if (!read_block(buf,0,0,2) == NULL) return -1; */
  knn = nn+1+((nn-2)/2); /* primo byte da considerare */
  znn = knn % 512;
/*  printf("knn=%u znn=%u",knn,znn); */
  if (!read_block(1+knn/512,buf,1)) return -3;
  nnn[0] = buf[znn++];if (znn>511) {
     if (!read_block(2+knn/512,buf,1)) return -3;
     znn = 0;
  }
  nnn[1] = buf[znn++];if (znn>511) {
     if (!read_block(2+knn/512,buf,1)) return -3;
     znn = 0;
  }
  nnn[2] = buf[znn];
/*  printf(" nnn=%02x%02x%02x]",nnn[0],nnn[1],nnn[2]); */
  nn = ((*(u_short *)(nnn)>>4*(nn%2))&0xfff);
  if (nn == 0) return 0; /* vuoto */
  if (nn == 4095) return -1;/* ultimo */
  if (nn == 0xff7) return -2;/* guasto */
  nn += (nfat-2);
  return nn;
}

int setfat12(int nn,u_short nnk) {
  int knn,znn;
  u_char nnn[2];
  u_char buf[512];
  if (nn < nfat) return -1; else nn -= (nfat-2);
  if ((nnk >= nfat) && (nnk < 0xf00)) nnk -= (nfat-2);
  if ((nn<2) && (nn>=(2880/512))) return -1;
/* nn/2*3 -> progressivo byte della fat */
/* nn/2*3/512 -> settore della fat interessato */  
/*  if (!read_block(buf,0,0,2) == NULL) return -1; */
  knn = nn+1+((nn-2)/2); /* primo byte da considerare */
  znn = knn % 512;
/*  printf("knn=%u znn=%u",knn,znn); */
  if (!read_block(1+knn/512,buf,1)) return -3;
  nnn[0] = buf[znn++];if (znn>511) {
     if (!read_block(2+knn/512,buf,1)) return -3;
     znn = 0;
  }
  nnn[1] = buf[znn++];
/*  printf(" nnn=%02x%02x%02x]",nnn[0],nnn[1],nnn[2]); */
#define snnn (*(u_short *)nnn)  
  nnk &= 0xfff;
  if ((nn%2)) snnn = (snnn & 0x000f) + (nnk << 4);
          else snnn = (snnn & 0xf000) + nnk;
            
  knn = nn+1+((nn-2)/2); /* primo byte da considerare */
  znn = knn % 512;
  if (!read_block(1+knn/512,buf,1)) return -3;
  buf[znn++] = nnn[0];if (znn>511) {
     write_block(1+knn/512,buf,1);knn += 512;
     if (!read_block(1+knn/512,buf,1)) return -3;
     znn = 0;
  }
  buf[znn++]= nnn[1];
  write_block(1+knn/512,buf,1);
  return 0;
}


/* #define fat12(nn) ((*(u_short *)(FAT+nn+1+((nn-2)/2))>>4*(nn%2))&0xfff) */

void format_S(char *nome1,char *nome2) {
  int t1,t2;
  for (t1 = t2 = 0; t2 < 11; t2++) {
    if ((nome2[t2]>64) && (nome2[t2]<91)) nome1[t1++] = nome2[t2]+ 32; else
    if ((nome2[t2] == ' ') && (t2 < 8)) {
          if (nome1[9] != ' ') { nome1[t1++] = '.'; t2 = 7; } else
          { nome1[t1] = 0; return; }
    } else 
    if (nome2[t2] == ' ') {
      if (nome1[t1-1] == '.') nome1[t1-1] = 0;
      nome1[t1] = 0; return;
    } else {
      nome1[t1++] = nome2[t2];
      if (t2 == 7) nome1[t1++] = '.';
    }  
  }
  nome1[t1] = 0;  
  return;
}
void format_L(char *nome1,char *nome2) {/* buff, filee.nome */
  int t1;
  for (t1=0;*nome2 && (t1 <11);nome2++) {
    if ((*nome2>64) && (*nome2<91)) nome1[t1++] = *nome2 + 32; else
    if ((*nome2 == '.') && (t1 == 8)) ; else
    if ((*nome2 == '.') && (t1<8)) {
      while (t1<8) nome1[t1++] = ' ';
    } else nome1[t1++] = *nome2;
  }
  while (t1 < 11) nome1[t1++] = ' ';
}

int leggifile(int curpos,FILE *filee) {
  int t;
  int clustcorr=0;
  u_char buf[512];
  setta_disk();
  if (psr<1) { printf("disco floppy errato\n"); return -3; }
  if (!curpos) curpos = psr*512;
  clustcorr = curpos & 0x7ffffe00; /* setta il cluster caricato in memoria */
  if (!read_block(curpos/512,buf,1)) return -3;
  while ((buf[curpos & 0x1ff] == 0xe5) || 
      ((buf[curpos & 0x1ff] != 0) && (buf[(curpos & 0x1ff)+2] == 0))) {
    if (!buf[curpos & 0x1ff]) return -1;
    curpos += 32;
    if (clustcorr != (curpos & 0x7ffffe00)) /* se il settore e' un'altro */ {
       if (!read_block(curpos/512,buf,1)) return -3;
    }
  }
  if (!buf[curpos & 0x1ff]) return -5;
  t = curpos & 0x1ff;
  format_S(filee->nome,buf+t);
  filee->attr = buf[t+11]; 
  filee->time = *(u_short *)(buf+t+22); 
  filee->date = *(u_short *)(buf+t+24); 
  filee->clust = *(u_short *)(buf+t+26); 
  filee->size = *(u_int *)(buf+t+28); 
  return curpos+32;
}


int path_to_num(char *pp1) {
  FILE filee1;
  u_char buf[512];
  int t,t1=0,pc=0,pc1=0;
  memcpy(buf,pp1,strlen(pp1));
  buf[strlen(pp1)] = 0;
  for (t=0;buf[t];t++)
     if (buf[t] == '/'){ buf[t] = 0;}

  buf[t] = 0;   
  if ((buf[0] == 0) && t) {pc1 = pc = psr*512; t1=1; } else pc1 = pc = dircorrente;

  for (;t1<t;) {
     pc = leggifile(pc,&filee1);
     if (pc < 0) return -6;
     if (!(strcmp(buf+t1,filee1.nome))) {/* se ha trovato il nome */
        if (filee1.attr & 0x10) {     /* se e' una dir */
            if (!filee1.clust) pc1= pc= psr*512; else
                 pc1 = pc = (filee1.clust+nfat-2)*512;
            while (buf[t1]) t1++; t1++;
        } else {                      /* se e' un file */
            while (buf[t1]) t1++;
            if (t1 >= t) return -5;   /* file trovato */
                    else return -6;   /* file non trovato */   
        }
     }
  }   
  return pc1;
}

int num_to_path(int nn,char *pp1) {
/* 
       pc puntatore alla dir corrente 
       pc1 puntatore alla dir superiore
       nn posizione corrente 
*/
  FILE filee1;
  u_char buf[512];
  short nbuf=511;
  int t,pc=0,pc1=0,pc2=0;/* pc punta*/
  if (!nn) return -6;
  pc=nn;
  buf[nbuf--] = 0;    
  while (nn) {
     nn = leggifile(nn,&filee1);      /* leggi l'entry */
     if (nn < 0) { 
        if (pc1 == 0) { /* dir root */
            break;
        } else pc2 = (nn = pc1); } else
     if (!strcmp("..",filee1.nome)) { /* se e' .. */
       if (filee1.clust) pc1 = (filee1.clust+nfat-2)*512;          /* conserva il posizore */  
                  else pc1 = psr*512;
     } else
     if (!strcmp(".",filee1.nome)) ; else
     if (((filee1.clust+nfat-2)*512) == pc) {
         for (t=strlen(filee1.nome)-1;t>-1;t--)
             buf[nbuf--] = filee1.nome[t];
         buf[nbuf--] = '/';    
         pc = pc2; pc2 = (nn = pc1);pc1 = 0;
     }
  }
  pc=0;
  if (nbuf<510) 
    for (t=nbuf+1;t<512;t++) {
       pp1[pc++] = buf[t];
    }
  pp1[pc++] = '/';
  pp1[pc] = 0;
  return 0;
/*        if (filee1.attr & 0x10) {     se e' una dir */
  
}



void mkdosfs() {
  int t;
  u_char buf[512];
  bzero(buf,512);
  memcpy(buf,"\xeb\x3c\x90ItaliOs \x00\x02\x01\x01\x00\x02\xe0\x00\x40\x0b\xf0\x09\x00\x12\x00\x02\x00\x00\x00",0x1e);
  write_block(0,buf,1);
  bzero(buf,512);
  memcpy(buf,"\xf0\xff\xff",3);
  write_block(1,buf,1);
  write_block(9,buf,1);
  bzero(buf,512);
  for (t=3;t<10;t++) {
    write_block(t-1,buf,1);
    write_block(t+8,buf,1);
  }  
  write_block(19,buf,1);
  printf("filesystem msdos fat12 inserito\n");
  return;
}

void format() {
  int t;
  u_char buf[512];
  bzero(buf,512);
  for (t=0;t<2880;t++) {
    printf("% 4i settori\r",t);
    write_block(t,buf,1);
  }  
  printf("formattazione completata\n");
  return;
}




int creaentry(int curpos,char *nomedir,u_char attr) {
  u_char nomee[12];
  int clustcorr=0;
  int t=0,t1;
  u_char buf[512]; 
  setta_disk();
  if (psr<1) { printf("disco floppy errato\n"); return -3; }
  if (!curpos) curpos = psr*512;
  clustcorr = curpos & 0x7ffffe00; /* setta il cluster caricato in memoria */
  if (!read_block(curpos/512,buf,1)) return -3;
  format_L(nomee,nomedir);
  while ((buf[curpos & 0x1ff] != 0) && (buf[curpos & 0x1ff] != 0xe5)) {
    if (!strncmp(nomee,buf+(curpos & 0x1ff),11)) return -5;
    if (clustcorr != (curpos & 0x7ffffe00)) /* se il settore e' un'altro */ {
       if (!read_block(curpos/512,buf,1)) return -3;
    }
    curpos += 32;
  }
  t = curpos & 0x1ff;
  bzero(buf+t,32);
  for (t1=0;t1<11;t1++) buf[t1+t] = ' ';
  memcpy(buf+t,nomee,11);
  buf[t+11] = attr; /* attributo  */
  *(u_short *)(buf+t+26) = 0; /* start cluster */
  *(u_int *)(buf+t+28) = 0;   /* size */

  write_block(curpos/512,buf,1);
  return (curpos & 0x7ffffe00)+t;
}

int cancfile(int curpos,char *nomedir) {
  int t;
  int t1;
  FILE filee1;
  u_char buf[512]; 
  setta_disk();
  if (psr<1) { printf("disco floppy errato\n"); return -3; }
  if (!curpos) curpos = psr*512;
  t=leggifile(curpos,&filee1); /* cerca il file */
  while (filee1.nome[0]) {
    if (!strcmp(filee1.nome,nomedir)) break;
    t=leggifile(t,&filee1);
  }
  if (!(filee1.nome[0])) return -5;
  t -= 32;
  if (!read_block(t/512,buf,1)) return -3;
  buf[t & 0x1ff] = 0xe5;
  write_block(t/512,buf,1);
  if (!(t1 = t = (filee1.clust+nfat-2))) return 0;
  if (t < nfat) return -4;
  while ((t>= nfat) && (t<2079)) {
    t=intfat12(t1);
    setfat12(t1,0);
    t1=t;
  }  
  if (t == -1) setfat12(t1,0);
  return 0;
}





FILE *Fopen(char *nomefile,char *modo) {
  FILE file1;
  FILE *file2;
  int t,t1=0;
  file2 = malloc(sizeof(FILE));
  file2->dir= dircorrente;
  for (t=0;nomefile[t];t++)
     if (nomefile[t] == '/') t1 = t;  
  if (t1) {
    if (t == t1) { free(file2); return NULL; }
    nomefile[t1] = 0;
    file2->dir = path_to_num(nomefile);
    if (file2->dir < 0) { free(file2); return NULL; }
    nomefile +=t1+1;
  }  
  memcpy(file2->nome,nomefile,12);
  if (!strcmp(modo,"r+")) file2->flag = 0x02; else /* R pos=0 */
  if (!strcmp(modo,"r")) file2->flag = 0x01; else  /* RW pos=0 */
  if (!strcmp(modo,"w+")) file2->flag = 0x08; else /* W filecanc pos=0*/
  if (!strcmp(modo,"w")) file2->flag = 0x04; else  /* RW filecanc pos=0*/
  if (!strcmp(modo,"a+")) file2->flag = 0x20; else /* RW creasenones pos=end */
  if (!strcmp(modo,"a")) file2->flag = 0x10;  /* W creasenones pos=end */	

  t=leggifile(file2->dir,&file1); /*  cerca il puntatore al file */
  while(file1.nome[0]) {
     if (!strcmp(file1.nome,nomefile)) break;
     t=leggifile(t,&file1);
     if (t < 0) break;
  }
  t -= 32;
  
/* gestione dei flags */
  if (t < 0) {
    if (file2->flag & 0xFC) {
      creaentry(file2->dir,nomefile,0x20);
      file1.clust=0;
      file1.size=0;
    } else { free(file2);return NULL; }
  } else {
    if (file2->flag & 0xc0) {
      cancfile(file2->dir,nomefile);
      creaentry(file2->dir,nomefile,0x20);
      file1.clust=0;
      file1.size=0;
    }  
  }
/* per il momento file solo in lettura */
  file2->posiz = 0;
  if (file1.clust) file2->clust_corr = file2->clust = file1.clust+nfat-2; else
                     file2->clust_corr = file2->clust = 0;  
  file2->size = file1.size;
  if (file2->flag & 0x30) file2->posiz = file2->size;
  return file2;
   
}


char *Fgets(char *ss,int size,FILE *file1) {
  int t;
  int t1=0;
  u_char buf[512];
  t = file1->posiz % 512;
  if (!read_block(file1->clust_corr,buf,1)) return NULL;
  if (file1->posiz == file1->size) return NULL;
  for (t1=0; (t1<size) && (file1->posiz < file1->size) ;t1++) {
     if (buf[t] == '\x0a') {
        ss[t1] = 0;
        ++file1->posiz;
        break;
     } else
     if (buf[t] == 0) {
        ss[t1] = 0;
        ++file1->posiz;
        break;
     } else
     ss[t1] = buf[t];
     t = (++file1->posiz)%512;
     if (!t) {
       file1->clust_corr = intfat12(file1->clust_corr);
       if (!read_block(file1->clust_corr,buf,1)) return NULL;
     }
  }
  return ss;

}
int Feof(FILE *file1) {
  return (file1->posiz == file1->size );
}
int Fread(char *ss,int size,int nmemb,FILE *file1) {
  int t;
  int t1=0;
  u_char buf[512];
  t = file1->posiz % 512;
  size *= nmemb;
  if (!read_block(file1->clust_corr,buf,1)) return 0;
  if (file1->posiz == file1->size) return 0;
  for (t1=0; (t1<size) && (file1->posiz < file1->size) ;t1++) {
     ss[t1] = buf[t];
     t = (++file1->posiz)%512;
     if (!t) {
       file1->clust_corr = intfat12(file1->clust_corr);
       if (!read_block(file1->clust_corr,buf,1)) return 0;
     }
  }
  return t1;

}

int cercaclust(int nn) {
 int t=0,t1;
    if (!nn) nn = nfat;
    for (t1=nn;t != -4;t1++)
         if ((t = intfat12(t1)) == 0) break;/* trovato un clust libero */
    if (t == -4) return -4;
    return t1;
}


int Fwrite(char *ss,int size,int nmemb,FILE *file1) {
  int t;
  int t1=0;
  u_char buf[512];
  size *= nmemb;
  if (!file1->clust) { /* se il file e' vuoto */
    if ((t1=cercaclust(nfat)) > 0) {
       file1->clust = file1->clust_corr = (short)t1;
       setfat12(t1,4095); /* occupiamo il cluster trovato */
    } else return t1;   
  }  
  t = file1->posiz % 512;
  if (!read_block(file1->clust_corr,buf,1)) return -1;
/*  if (file1->posiz == file1->size) return NULL; */
  for (t1=0; t1 < size;t1++) {
     buf[t] = ss[t1];
     t = (++file1->posiz)%512;
     if (!t) {
       write_block(file1->clust_corr,buf,1);
       t = (int)intfat12(file1->clust_corr);
       if ((t < 2) || (t >2879)) {
          t = (int)cercaclust(file1->clust_corr);
          if (t) { 
            setfat12(file1->clust_corr,(u_short)t);
            setfat12((u_short)t,4095);
            file1->clust_corr = t;
            }
            else return -2;
          
       } else return -3;
       if (file1->posiz > file1->size) file1->size = file1->posiz;
       if (!read_block(file1->clust_corr,buf,1)) return -1;
       t=0;
     }
  }
  write_block(file1->clust_corr,buf,1);
  if (file1->posiz > file1->size) file1->size = file1->posiz;
  return t1;

}

int Fseek(FILE *file1,int posiz,u_char origine) {
   int t,tcur;

   tcur = file1->posiz;   
   if (origine == SEEK_SET) tcur = posiz; else
   if (origine == SEEK_CUR) tcur += posiz; else
   if (origine == SEEK_END) tcur = file1->size+posiz;else
      return -1;
   
   if (tcur > file1->size) tcur = file1->size;
   if (tcur < 0) tcur = 0;
   if (tcur >= file1->posiz) {
       t= tcur/512-file1->posiz/512;
   } else {
       t=tcur/512; file1->clust_corr = file1->clust;
   }
   while (t--) file1->clust_corr = intfat12(file1->clust_corr);
   file1->posiz = tcur;
   return 0;
}

int Ftell(FILE *file1) {
  if ((file1) && (file1->flag))
    return file1->posiz;
  else return -1;  
}



int Fclose(FILE *file2) {
  int tz=0;
  u_char buf[512]; 
  FILE file1;
  setta_disk();
  if (psr<1) { printf("disco floppy errato\n"); return -3; }
/*  tz=dircorrente;
  if (!tz) tz = psr*512;*/
  tz=leggifile(file2->dir,&file1); /* cerca il file */
  while (file1.nome[0]) {
    if (!strcmp(file1.nome,file2->nome)) break;
    tz=leggifile(tz,&file1);
  }
  if (!(file1.nome[0])) return -5;
  tz -= 32;
  if (!read_block(tz/512,buf,1)) return -3;
  if (file2->clust) *(u_short *)(buf+(tz & 0x1ff)+26) = file2->clust-nfat+2;   /* size */
  *(u_int *)(buf+(tz & 0x1ff)+28) = file2->size;   /* size */
  write_block(tz/512,buf,1);
  free(file2);
  return 0;
}


int mkdir(int curpos,char *nomedir) {
  int t;
  int t1,t2;
  FILE filee1;
  u_char buf[512]; 
  setta_disk();
  if (psr<1) { printf("disco floppy errato\n"); return -1; }
  if (!curpos) curpos = psr*512;
  t1=0;
  for (t=0;nomedir[t];t++)
     if (nomedir[t] == '/') t1 = t;  
  if (t1) {
    if (t == t1) return -6;
    nomedir[t1] = 0;
    curpos = path_to_num(nomedir);
    if (curpos < 0) return -6;
    nomedir +=t1+1;
  }  


  t=leggifile(curpos,&filee1);
  t1 = 0;
  while (filee1.nome[0]) {
    if (!strcmp(filee1.nome,nomedir)) return -5;
    if (t < 0) break;
    t1 = t;
    t=leggifile(t,&filee1);
  }
  t = (curpos = t1) & 0x1ff;
  if (curpos<=0) curpos = psr*512;
  t2 = cercaclust(nfat);
  if (t2 < 0) return -4;
  setfat12(t2,0xfff);
  
  if (!read_block(curpos/512,buf,1)) return -1;
/*  cls[1]=t; */
/*  bzero(buf+t,32); */
  for (t1=0;t1<11;t1++) buf[t1+t] = ' ';
  memcpy(buf+t,nomedir,strlen(nomedir));
  buf[t+11] = 0x10; /* attributo directory  */
  *(u_short *)(buf+t+26) = (u_short)t2-nfat+2;
  *(u_int *)(buf+t+28) = 0;

  write_block(curpos/512,buf,1);
  if (!read_block(t2,buf,1)) return -1;

  bzero(buf,512);
  buf[0] = '.';
  for (t1=1;t1<11;t1++) buf[t1] = ' ';
  buf[11] = 0x10; /* attributo directory  */
  *(u_short *)(buf+26) = (u_short)t2-nfat+2;
  *(u_int *)(buf+28) = 0;

  buf[32] = '.';buf[33] = '.';
  for (t1=2;t1<11;t1++) buf[t1+32] = ' ';
  buf[32+11] = 0x10; /* attributo directory  */
  if (((curpos/512) >= psr) && ((curpos/512) < nfat))
    *(u_short *)(buf+32+26) = 0; else
  *(u_short *)(buf+32+26) = (u_short)(curpos/512-nfat+2);
  *(u_int *)(buf+32+28) = 0;
  write_block(t2,buf,1);
  return 0;
}


int df() {
  int t,t1,t2=0,t3=0;
  setta_disk();
  for (t=nfat;t<2880;t++) {
    t1=intfat12(t);
    if (t1 == 0) t2++;
    if (t1 == 0xff7) t3++;
  }
  if (t3) printf("   %u byte danneggiati\n",t3*512);
  printf("   %u byte liberi\n",t2*512);
  return t2*512;
}



int cercafile(char *pathcompleto,char *dir,char *nome) {
  FILE f1;
  int tt[10],btt[10],ntt,t;
  char nome1[20];
  if ((tt[0] = path_to_num(dir)) <0) return -7;
  btt[0] = tt[0];
  ntt=0;
  while (ntt>=0) {
      tt[ntt] = leggifile(tt[ntt],&f1);
      if (tt[ntt] < 0 ) { ntt--; continue; }
      if (f1.nome[0] == '.') continue;
      if (f1.attr & 0x10) {
         ntt++;
         tt[ntt] = (f1.clust+nfat-2)*512;
         btt[ntt] = tt[ntt];
      } else {
         if (!strcmp(nome,f1.nome)) break;
      }
      
  }
  if (ntt < 0) return -5;
  strncpy(nome1,nome,20);
  ntt = num_to_path(btt[ntt],pathcompleto);
  t= strlen(pathcompleto);
  t += sprintf(pathcompleto+t,"/");
  t += sprintf(pathcompleto+t,nome1);
  return ntt;
  
  
}



