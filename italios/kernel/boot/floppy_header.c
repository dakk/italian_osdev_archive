#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define byte unsigned char
#define word unsigned short int
#define dword unsigned int

struct fat_boot_sector {
	char	ignored[3];
	char	system_id[8];	/* Nome */
	byte	sector_size[2];	/* bytes per logical sector */
	byte	cluster_size;	/* sectors/cluster */
	word	reserved;	/* reserved sectors */
	byte	fats;		/* numero di fat */
	byte	dir_entries[2];	/* root directory entries */
	byte	sectors[2];	/* number of sectors */
	byte	media;		/* media code (unused) */
	word	fat_length;	/* sectors/FAT */
	word	secs_track;	/* sectors per track */
	word	heads;		/* number of heads */
	dword	hidden;		/* settori nascosti */
	dword	total_sect;	/* number of sectors (if sectors == 0) */
	byte drive_num;
	byte reserved1;
	byte signature;
	dword vol_id;
	byte label[11];
	dword fstype[8];
};

int main(size_t num_par, char** par)
{
int fd;				// File Descriptor per il dispositivo
struct fat_boot_sector boot;	// Record nel quale leggere il primo settore
  char nome_floppy[20];
  int i;
  
	if(num_par != 2){
		printf("\nUso: %s nomefile\n", par[0]);
		return -1;		
	}

  if((fd=open(par[1],O_RDONLY))==-1)	// Apre il dispositivo
  {
    printf("%s: Errore di apertura! Forse mancano i diritti sul dispositivo\n", par[1]);
    return 1;
  }

  read(fd,&boot,sizeof(boot));		// Legge il boot sector
  
  for(i = 0; i < 8; i++){
	  	if(!boot.system_id[i]){
			nome_floppy[i] = ' ';
		}else		nome_floppy[i] = boot.system_id[i];
  }
   nome_floppy[8] = 0;

  // Stampa le caratteristiche interessanti
  printf("\nBS_OEMName db '%s'", nome_floppy);
  printf("\nBPB_BytsPerSec dw %d",*(word *)boot.sector_size);
  printf("\nBPB_SecPerClus db %d",boot.cluster_size); //settori per cluster
  printf("\nBPB_RsvdSecCnt dw %d",boot.reserved);
  printf("\nBPB_NumFATs db %d",boot.fats);
  printf("\nBPB_RootEntCnt dw %d",*(word *)boot.dir_entries);
  printf("\nBPB_TotSec16 dw %d",*(word*)boot.sectors);
  printf("\nBPB_Media db %d",boot.media);
  printf("\nBPB_FATSz16 dw  %d",boot.fat_length);
  printf("\nBPB_SecPerTrk dw %d",(word)boot.secs_track);
  printf("\nBPB_NumHeads  dw %d",boot.heads);
  printf("\nBPB_HiddSec dd %d",boot.hidden);
  printf("\nBPB_TotSec32 dd %d",boot.total_sect);
  printf("\nBS_DrvNum db %d",boot.drive_num);
  printf("\nBS_Reserved1 db %d",boot.reserved1);
  printf("\nBS_BootSig db %d",boot.signature);
  printf("\nBS_VolId dd %d",boot.vol_id);
  for(i = 0; i < 11; i++)
		nome_floppy[i] = boot.label[i];
   nome_floppy[11] = 0;
  printf("\nBS_VolLab db '%s'",nome_floppy);
    for(i = 0; i < 8; i++)
		nome_floppy[i] = boot.fstype[i];
   nome_floppy[8] = 0;
  printf("\nBS_FileSysType db '%s'\n",nome_floppy);
   return 0;
}
