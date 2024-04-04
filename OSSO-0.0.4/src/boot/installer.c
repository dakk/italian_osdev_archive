/*
 * OSSO - A Micro Kernel OS
 * Copyright (c) 2000 Alessandro Iurlano.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */


/* Boot Installer */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
//#include <errno.h>


#define VERBOSE
#define DEBUG_CHECK


#define SYSCALL(r,c,e) if ((r=c)==-1) { perror(e); exit(-1); }
#define SYSCALLN(c,e)  if (c==-1) { perror(e); exit(-1); }

typedef unsigned char      byte;
typedef unsigned short int word;
typedef unsigned int       dword;

#define BOOTLOADER_NAME "BOOTLOADER"

const char *SYSTEM_NAME="OSSO    ";

#define STRUCTURE_SIZE_BOUNDARY 0

struct BootSectorTypes {
  byte  Jmp[3];
  char  SystemName[8];
  word  bytesXsector;
  byte  sectorsXcluster;
  word  reservedSectors;
  byte  numFAT;
  word  numDirEntries;
  word  totSectorsU;
  byte  mediaDescriptor;
  word  sectorsXFAT;
  word  sectorsXtrack;
  word  numHeads;
  dword numHiddenSectors;
  dword totSectors;
  byte  physicalDriveNum;
  byte  RESERVED;
  byte  sign4extBR;
  dword serialNum;
  char  label[11];
  char  FATtype[8];
  char  BOOTSTRAPCODE[448];
  word  SectorSignature;
} __attribute__ ((packed));

typedef struct BootSectorTypes BootSectorType;


void WriteString(char *s, char lung) {
  for (; lung; lung--, s++)
    printf("%c",*s);
}


int main(int argc, char * argv[])
{
  BootSectorType nbs, bs;
  char Comando[1024];
  int bootsector_fd, bootloader_fd;
  int ret;

#ifdef DEBUG_CHECK
  if (sizeof(BootSectorType)!=512) {
    printf("\nSIZE = %u\n\n",sizeof(BootSectorType));
    printf("Errore nella definizione della struttura!!!\n\n");
    return -100;
  }
#endif

  if (argc!=2) {
    printf("Format: %s device\n",argv[0]);
    return -1;
  }

  /* Reading of boot-sector data. */
  printf("Reading bootsector... ");
  SYSCALL(bootsector_fd,open(argv[1],O_RDONLY,0),"\nBootsector open");
  SYSCALLN(read(bootsector_fd,&bs,512),"\nBootsector read");
  SYSCALLN(close(bootsector_fd),"\nBootsector close");
  printf("ok.\n");

#ifdef VERBOSE
  printf("         System name: "); WriteString(bs.SystemName,8); printf("\n");
  printf("    Bytes per sector: %6u\n",bs.bytesXsector);
  printf(" Sectors per cluster: %6u\n",bs.sectorsXcluster);
  printf("    Reserved sectors: %6u\n",bs.reservedSectors);
  printf("         Num of FATs: %6u\n",bs.numFAT);
  printf("    Root dir entries: %6u\n",bs.numDirEntries);
  printf("    Total sectors u.: %6u\n",bs.totSectorsU);
  printf("    Media descriptor:   0x%02X\n",bs.mediaDescriptor);
  printf("     Sectors per FAT: %6u\n",bs.sectorsXFAT);
  printf("   Sectors per track: %6u\n",bs.sectorsXtrack);
  printf("Num of logical heads: %6u\n",bs.numHeads);
  printf("      Hidden sectors: %6u\n",bs.numHiddenSectors);
  printf("       Total sectors: %6u\n",bs.totSectors);
  printf("  Physical drive num:   0x%02X\n",bs.physicalDriveNum);
  printf("Signature for ext BR:   0x%02X\n",bs.sign4extBR);
  printf("       Serial number: 0x%08X\n",bs.serialNum);
  printf("               Label: "); WriteString(bs.label,11); printf("\n");
  printf("            FAT type: "); WriteString(bs.FATtype,8); printf("\n");
  printf("    Sector signature: 0x%04X\n\n",bs.SectorSignature);
#endif

  /* Init bootloader recompilation command. */
  sprintf(Comando,"nasm bootloader.nasm -f bin -o %s",BOOTLOADER_NAME);

  /* Checking for filesystem type. */
  {
    int c=8;
    char *p=bs.FATtype;

    printf("Filesystem: ");
    for (; c; c--, p++) putchar(*p);
    printf("\n");
  }
  if (!strncmp(bs.FATtype,"FAT12   ",8))
    strcat(Comando," -DFAT12");
  else
    if (!strncmp(bs.FATtype,"FAT16   ",8))
      strcat(Comando," -DFAT16");
    else {
      fprintf(stderr,"\nError: Filesystem not implemented (unformatted ?)!\n");
      exit(-1);
    }

  /* Checking for number of bytes per sector. */
  printf("Byte per sector: %d\n\n",bs.bytesXsector);
  if (bs.bytesXsector!=512)
    strcat(Comando," -DGENERIC_BxS");

  /* Recompilation of boot-loader. */
  printf("Compiling bootloader...\n\n");
  printf("%s\n",Comando);
  ret=system(Comando);
  if (ret==-1 && ret==127) {
    perror("\nSystem()");
    exit(-1);
  }
  if (ret) {
    fprintf(stderr,"Error in compiling bootloader!!!\n");
    exit(-1);
  }
  printf("\nok: compilation done\n\n");

  /* Reading just-compiled boot-loader. */
  SYSCALL(bootloader_fd,open(BOOTLOADER_NAME,O_RDONLY,0),"Bootloader open");
  SYSCALLN(read(bootloader_fd,&nbs,512),"Bootloader read");
  SYSCALLN(close(bootloader_fd),"Bootloader close");

  /* Updating of boot-sector data. */
  printf("Updating bootloader... ");
  memcpy(((char *)&nbs)+3,((char *)&bs)+3,59);  /* Copying data.     */
  memcpy(nbs.SystemName,SYSTEM_NAME,8);         /* Updating SysName. */
  printf("ok.\n");

  /* Writing of new boot-sector. */
  printf("Writing bootloader... ");
  SYSCALL(bootsector_fd,open(argv[1],O_RDWR,0),"\nBootsector open");
  SYSCALLN(write(bootsector_fd,&nbs,512),"\nBootsector write");
  SYSCALLN(close(bootsector_fd),"\nBootsector close");
  printf("ok.\n");

  return 0;
}
