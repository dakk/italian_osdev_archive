#include <stdio.h>


int main(int argc,char ** argv) {
  FILE *f1,*f2;
  int t;
  unsigned char buf[512];
  memset(buf,0,512);
  if ((argc != 2) && (argc != 3)) {
	printf("Sintassi: mkfatfs <immaginefloppy> <eventuale bootsector>\n");
	return -1;
  }
  f1 = fopen(argv[1],"r+");
  if (argc == 3) {
	f2 = fopen(argv[2],"r+");
	fread(buf,512,1,f2);
	fclose(f2);
  }
  
  memcpy(buf,"\xeb\x3c\x90ItaliOs \x00\x02\x01\x01\x00\x02\xe0\x00\x40\x0b\xf0\x09\x00\x12\x00\x02\x00\x00\x00",0x1e);    fwrite(buf,512,1,f1);

  memset(buf,0,512);
  memcpy(buf,"\xf0\xff\xff",3);

  fseek(f1,512,SEEK_SET);
  fwrite(buf,512,1,f1);

  fseek(f1,512*9,SEEK_SET);
  fwrite(buf,512,1,f1);

  memset(buf,0,512);
  for (t=3;t<10;t++) {
	fseek(f1,512*(t-1),SEEK_SET);
	fwrite(buf,512,1,f1);
	fseek(f1,512*(t+8),SEEK_SET);
	fwrite(buf,512,1,f1);
  }  
  fseek(f1,512*(19),SEEK_SET);
  fwrite(buf,512,1,f1);
  printf("filesystem msdos fat12 inserito\n");
  fclose(f1);
  return 0;
}
