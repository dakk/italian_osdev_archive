#include <cstddef>
#include <floppy.h>
#include <cstring>
#include <video.h>

#define	toupper(c)	(((c)>='a'&&(c)<='z') ? (c)^0x20 : (c))
#define	tolower(c)	(((c)>='A'&&(c)<='Z') ? (c)^0x20 : (c))

//#define _DEBUG_

int strcasecmp(char *s,char *d){
   for(;;)
   {
      if( *s != *d )
      {
	 if( tolower(*s) != tolower(*d) )
	    return *s - *d;
      }
      else if( *s == '\0' ) break;
      s++; d++;
   }
   return 0;
}

void leggi_floppy(dword vpos, size_t lung,byte* buffer) {
	byte mybuffer[600];
	 byte *pbuffer = mybuffer;
	dword pos;
	 int num_set;
	int i;
	 int offsetbasso, offsetalto;
	 num_set = lung/512;
	 num_set++;
	 pos = vpos / 512;
	 //printf("\ndevo leggere la posizione %d", pos);
	 //pos /=512;	determino il settore effettivo
	 if((pos*512)==vpos){
		 offsetbasso = 0;
	 }else{
		 offsetbasso = vpos - (pos*512);
		 num_set++;
	 }
	 //printf("\nl'offset basso e' %d", offsetbasso);
	 if((vpos+num_set*512)<(vpos+lung)){
		 offsetalto = (vpos+lung)-(vpos+num_set*512);
		 	 num_set++;
	 }else{
		 offsetalto = 0;
	 }
	 //printf("\nl'offset alto e' %d", offsetalto);
	for(i=0; i < num_set; i++){
		read_block(pos,  mybuffer, 1);
		if(offsetbasso){
			pbuffer += offsetbasso;
			offsetbasso = 0;
		}
		if(lung <= 512){
			memcpy(buffer, pbuffer,lung);
			return;
		}
		memcpy(buffer, mybuffer, 512);
		lung -= 512;
		buffer += 512;
		pos += 512;
	}
}

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


// Descrittore del disco
struct disco {
    int drive;			// drive sul quale lavorare
    int rootDir;		// posizione in byte della root Directory
    int rootDirSize;		// Dimensione in 'entries' della root directory
    int clusterBytes;		// Dimensione di un cluster in byte
    int firstDataByte;		// Primo byte dell'area dati
    int FATPos;			// Posizione della FAT (in byte)
};

struct msdos_dir_entry {
    char name[8], ext[3];	/* nome e estenzione */
    byte attr;		/* bit dell'attributo */
    byte lcase;	/* case della base e dell'estanzione */
    byte ctime_ms;	/* millisecondi dalla creazione */
    word ctime;	/* tempo della creazione */
    word cdate;	/* data della creazione */
    word adate;	/* data dell'ultimo accesso */
    word starthi;	/* 16 bit altri del cluster in FAT32 */
    word time, date, start;	/* tempo, data e primo cluster*/
    dword size;		/* dimensione in byte */
};


#define ATTR_RO      1		/* sola lettura */
#define ATTR_HIDDEN  2		/* nascosto */
#define ATTR_SYS     4		/* di sistema */
#define ATTR_VOLUME  8		/* etichetta di volume */
#define ATTR_DIR     16		/* directory */
#define ATTR_ARCH    32		/* archivio */


/* nextCluster
 * Ritorna il prossimo cluster letto dalla FAT nell'entry relativa al cluster 'cluster'
 * pasato per paramentro, nel dispositivo descritto da 'd'
 * Se il cluster e' l'ultimo del file (il prossimo cluster e' 0xfff) ritorna 0.
 *
 * FAT a 12 bit:
 * 2 cluster ogni 3 byte.
 * Indico con 'P' il cluster pari e con D il cluster dispari:
 * I tre byte sono codificati cosi':
 * PP DP DD
 */
int nextCluster(struct disco *d,int cluster)
{
    byte dCluster[3];	// Buffer per la coppia di elementi
    dword pos = d->FATPos + (cluster / 2) * 3;	// Posizione in byte della coppia di elementi
    int nuovoCluster;		// Cluster letto nell'elemento

    // Legge la coppia
    leggi_floppy(pos, 3, dCluster);
    // Ricompone il numero di cluster in base alla formula esposta all'inizio
    if (cluster % 2)		// Cluster pari
    {
	nuovoCluster = dCluster[2];
	nuovoCluster =
	    ((dCluster[1] >> 4) & 0xf) | ((nuovoCluster << 4) & 0xff0);
    } else {			// Cluster dispari
	nuovoCluster = dCluster[1];
	nuovoCluster =
	    ((nuovoCluster << 8) & 0xf00) | (dCluster[0] & 0xff);
    }

    // Se il nuovo cluster e' 0xfff, ritorna 0, se no ritorna il nuovo cluster
    return (nuovoCluster == 0xfff) ? 0 : nuovoCluster;
}


/* clusterByte
 * Converte in byte dall'inizio del disco un numero di cluster.
 * Utilizza le informazioni del descrittore per il calcolo
 * Le prime due entry nella FAT sono riservate, quindi il primo cluster e' il 2
 */
dword clusterByte(struct disco *d, int cluster){
	dword acluster = cluster;
    if (acluster < 2)		// Se il cluster e' illegale
    {
	return 0;		// ritorno 0 (errore)
    }
    acluster -= 2;		// tolgo i cluster riservati
    acluster *= d->clusterBytes;	// moltiplico per la dimensione del cluster
    return acluster + d->firstDataByte;	// aggiungo l'inizio dell'area dati
}


struct fat_boot_sector  boot;
	struct disco d;
		
void init_fat(){
		 char nome_floppy[15];
	   int i;
//	 printf("\nlunghezza nominale %d\n", sizeof(struct fat_boot_sector));
	 //boot =(struct fat_boot_sector *) mybuffer;
	//disk_read(0, 0, 0,1, 1,boot);
	 leggi_floppy((dword)0,sizeof(struct fat_boot_sector), (byte*)&boot);
  for(i = 0; i < 8; i++){
	  	if(!boot.system_id[i]){
			nome_floppy[i] = ' ';
		}else		nome_floppy[i] = boot.system_id[i];
  }
   nome_floppy[8] = 0;
  
  #ifdef _DEBUG_
  // Stampa le caratteristiche interessanti
  kout << "\nNome: '"<< nome_floppy << "'";
  kout << "\nByte per settore " << *(word *)boot.sector_size ;
  kout << "\nSettore per cluster " << boot.cluster_size;
  kout << "\nSettori riservati " << boot.reserved;
  kout << "\nNumero di FAT " << boot.fats;
  kout << "\nNumero di entry delle dir " << *(word *)boot.dir_entries;
  kout << "\nTotale settori " << *(word*)boot.sectors;
  kout << "\nDispositivo " << boot.media;
  kout << "\nLunghezza della FAT  " << boot.fat_length;
  kout << "\nSettori per tracce " << (word)boot.secs_track;
  kout << "\nTestine " << boot.heads;
  kout << "\nSettori nascosti " << boot.hidden;
  kout << "\nNumero driver " << boot.drive_num;  
  #endif
      d.rootDir = (boot.reserved + boot.fats * boot.fat_length) *
	*(word *) boot.sector_size;

    // Dimensione in 'entries' della root directory
    d.rootDirSize = *(word *) boot.dir_entries;

    // Dimensione di un cluster in byte
    d.clusterBytes =	boot.cluster_size * *(word *) boot.sector_size;

    // Primo byte dell'area dati
    d.firstDataByte =	d.rootDir + (d.rootDirSize * sizeof(struct msdos_dir_entry));

    // posizione in byte della FAT
    d.FATPos = boot.reserved * *(word *) boot.sector_size;	
}

/*Cerca se un file è presente sul floppy. Il paramentro pos è l'entry dir dove cercare un file. SI possono mettere tranquillamente anche path complicati. Ovviamente tutto deve essere assoluto rispetto alla dir puntata da pos. Restituisce 0 se il fuile non esiste. Restituisce  il cluster se il file esiste*/
int trova_file(char* nome, dword pos){
	char pezzo[100];
	char analizzare[100];
	 struct msdos_dir_entry dir;
	int i, j, mylun;
	 	strcpy(analizzare, nome);
	strcpy(pezzo, nome);
	i= 0;
	j = 0;
//	 printf("-----> '%s'  <---------", nome);
	while((pezzo[i] != 0) && (pezzo[i] != '/')) i++;
	if(pezzo[i] == 0){
		char estenzione[3];
		for(i=0; i < 8; i++)
			if(analizzare[i] == '.')
				break;
		if(i < 8){
			int j;
			for(j = 0; j < 3; j++){
				estenzione[j] = analizzare[i+j+1];
			}
			for(; i < 8; i++)
				analizzare[i] = ' ';
			analizzare[i] = '.';
			for(j=0; j < 3; j++)
				analizzare[i+j+1] = estenzione[j];
			analizzare[i+j+1] = 0;
		}
		
		
		//printf("\nanalizzare file = '%s'", analizzare);
		while(1){
			int j;
			char nnome[20];
			leggi_floppy(pos, sizeof(dir),(byte*)&dir);
			pos +=sizeof(dir);
			if (dir.name[0] == 0) {
//	    		printf("\n%s non trovato", analizzare);
	    		return 0;
			}
			for(j=0; j < 8; j++){
				nnome[j] = dir.name[j];
			}
			nnome[j] ='.';
			for(j=0; j < 3; j++)
				nnome[j+9] = dir.ext[j];
			nnome[j+9] = 0;
		//	printf("\nlavoro con '%s'  e '%s' ", nnome, analizzare);
			
			if(!strcasecmp(nnome, analizzare)){
//				printf("\n%s trovato", nnome);
				return dir.start;
			}
		}

		
		return 1;
	}
	mylun = i;
	for(; i < 8; i++)
		analizzare[i] = ' ';	
	analizzare[i] = 0; 
	//printf("\nanalizzare = '%s'", analizzare);
	while(1){
		int j;
		char nnome[20];
		leggi_floppy(pos, sizeof(dir),(byte*)&dir);
		pos +=sizeof(dir);
		if (dir.name[0] == 0) {
//	    	printf("\n%s non trovato", analizzare);
	    	return 0;
		}
		for(j=0; j < 8; j++){
			nnome[j] = dir.name[j];
		}
		nnome[j]=0;
//		printf("\nlavoro con '%s'", nnome);
		if (dir.attr & ATTR_DIR) {
			if(!strcasecmp(nnome, analizzare)){
//				printf("\n%s trovato", nnome);
				return trova_file(&pezzo[mylun+1],clusterByte(&d, dir.start) );
			}
		}
	}
}

struct cmd_read{
	dword pos;
	size_t set;
};

/*legge un file e lo memorizza nella memoria puntata da buff, restituisce la lunghezza in byte del file*/
dword leggi_file(int cluster, void* buff){
	byte * buffer = (byte*) buff;
	dword lung = 0;
	dword npos;
	do{
		//kout <<"\033[S" << lung<< "\033[u";
		npos = clusterByte(&d, cluster);
		leggi_floppy(npos, 512, buffer);
		buffer += 512;
		lung += 512;
	}while((cluster = nextCluster(&d,cluster)) != 0);	
	return lung;
}

size_t dim_file(char* nome){
	int cluster;
	dword dir = d.rootDir;
	dword dim;
	//kout << "\nricerca di " << nome;
	if((cluster = trova_file(nome, dir))){
		dword lung = 0;
		dword npos;
		do{
			npos = clusterByte(&d, cluster);
			lung += 512;
		}while((cluster = nextCluster(&d,cluster)) != 0);	
		return lung;
	}
	return 0;
}


size_t carica_file(char* nome, void* buffer){
	int inizio;
	dword dir = d.rootDir;
	dword dim;
	//kout << "\nricerca di " << nome;
	if((inizio = trova_file(nome, dir))){
	//	kout << "\t\t\t[OK]";
	//	kout << "\n* Caricamento di " << nome <<  " in " << (dword) buffer << " in corso...";
		dim = leggi_file(inizio, buffer);
	//	kout << "Dimensione " << dim << " [OK]";
	}else{
	  //kout << "\t\t\t[NON TROVATO]";
		return 0;
	}
	return dim;
}
