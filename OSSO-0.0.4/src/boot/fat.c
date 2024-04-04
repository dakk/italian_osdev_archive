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


/****************************** O S S O  ***********************************
 * file : $Source: /cvsroot/osso/OSSO/src/boot/fat.c,v $
 * Description: 16-bit driver for boot process: fat.
 ***************************************************************************

 ***************************************************************************
 * $Id: fat.c,v 1.1 2001/05/14 05:47:39 steros Exp $
 ***************************************************************************
 *
 * $Log: fat.c,v $
 * Revision 1.1  2001/05/14 05:47:39  steros
 * Kernel loader now should be fs indipendent.
 *
 *
 ***************************************************************************/


/* Boot block data. */
#define BytesPerSector    (*((word *)(bpb+0x0b)))
#define SectorsPerCluster (*((byte *)(bpb+0x0d)))
#define ReservedSectors   (*((word *)(bpb+0x0e)))
#define NumberOfFATs      (*((byte *)(bpb+0x10)))
#define NumRootDirEntries (*((word *)(bpb+0x11)))
#define TotalSectors2     (*((word *)(bpb+0x13)))
#define MediaDescriptor   (*((byte *)(bpb+0x15)))
#define SectorsPerFAT     (*((word *)(bpb+0x16)))
#define SectorsPerTrack   (*((word *)(bpb+0x18)))
#define NumHeads          (*((word *)(bpb+0x1A)))
#define HiddenSectors     (*((ulong *)(bpb+0x1C)))
#define TotalSectors4     (*((ulong *)(bpb+0x20)))
#define PhysicalDriveNum  (*((byte *)(bpb+0x24)))
#define SignatureForExtBR (*((byte *)(bpb+0x26)))
#define SerialNumber      (*((ulong *)(bpb+0x27)))
#define DiskLabel         ((char *)(bpb+0x2b))
#define FATType           ((char *)(bpb+0x36))

/* BIOS data. */
#define boot_drive        (*((byte *)(bpb+62)))
#define number_of_sectors (*((word *)(bpb+63)))
#define number_of_heads   (*((word *)(bpb+67)))

/* Size of data block inherited from bootloader. */
#define BPB_size 72


/*
 Directory entry structure.
*/
typedef
struct __Dir_Entry {
  char filename[8];
  char ext[3];
  byte attributes;
  byte reserved[10];
  word time_of_creation;
  word date_of_creation;
  word starting_cluster;
  ulong file_size;
} Dir_Entry;


/*
 Global variables.
*/
char bpb[BPB_size];
char BootDrive;
Dir_Entry *rootdir;
int fat_start;
int root_start;
int data_start;
int bytes_per_cluster;
int root_entries_per_sector;
int dir_entries_per_cluster;
int root_sectors;
int fat_type=-1;     /* 0 = FAT12, 1 = FAT16 */
int FAT_cached=-1;
char *FAT_cache;





/*
 If segment is 0, it will be ignored.
*/
int read_sector(int logical_sec, int numsectors, uint segment, void *buffer)
{
  long hsector=HiddenSectors+logical_sec;
  long sec_head;
  int c, h, s;

  s=(hsector%number_of_sectors)+1;
  sec_head=hsector/number_of_sectors;
  h=sec_head%number_of_heads;
  c=sec_head/number_of_heads;
  read_sector_CHS(c,h,s,BootDrive,numsectors,segment,buffer);
  if (read_error) {
    OPERATION_FAIL;
    ERROR("\r\n  Sorry, one or more sectors of boot disk are unreadable!!!\n\r\n\r");
    COMMENT("\r\n  Try repairing or reformatting the disk...\n\r\n\r");
    PANIC;
  }
}


int read_cluster(int cluster, uint segment, void *buffer) {
  return
    read_sector((cluster-2)*SectorsPerCluster+data_start,
      SectorsPerCluster,segment,buffer);
}


/*
 Calculate disk and file system parameters.
*/
void GetBootDiskData() {
#asm
    pusha
    push es
    push ds
    mov ax,#$7c0
    mov ds,ax
    pop es
    mov si,#0
    lea di,_bpb
    mov cx,#BPB_size
    rep
    movsb
    push es
    pop ds
    pop es
    popa
#endasm

  BootDrive=boot_drive;
  if (!strncmp(FATType,"FAT12   ",8))
    fat_type=0;
  else
    if (!strncmp(FATType,"FAT16   ",8))
      fat_type=1;
    else {
      OPERATION_FAIL;
      COMMENT("\r\n  Sorry, FAT type not recognized!!!\n\r\n\r");
      PANIC;
    }

#ifdef XVERBOSE
  color_prints(Cyan,"Boot sector data\n\r");
  _printf("     BytesPerSector: %d\n\r",BytesPerSector);
  _printf("  SectorsPerCluster: %d\n\r",SectorsPerCluster);
  _printf("    ReservedSectors: %d\n\r",ReservedSectors);
  _printf("       NumberOfFATs: %d\n\r",NumberOfFATs);
  _printf("  NumRootDirEntries: %d\n\r",NumRootDirEntries);
  _printf("      TotalSectors2: %d\n\r",TotalSectors2);
  _printf("    MediaDescriptor: %d\n\r",MediaDescriptor);
  _printf("      SectorsPerFAT: %d\n\r",SectorsPerFAT);
  _printf("    SectorsPerTrack: %d\n\r",SectorsPerTrack);
  _printf("           NumHeads: %d\n\r",NumHeads);
  _printf("      HiddenSectors: %d\n\r",HiddenSectors);
  _printf("      TotalSectors4: %ld\n\r",TotalSectors4);
  _printf("   PhysicalDriveNum: %d\n\r",PhysicalDriveNum);
  _printf("  SignatureForExtBR: %d\n\r",SignatureForExtBR);
  _printf("       SerialNumber: %ld\n\r",SerialNumber);
  _printf("          DiskLabel: \"");
  {
    char *p=DiskLabel, c=11;
    for (; c; c--, p++) printc(*p);
  }
  _printf("\"\n\r");
  _printf("            FATType: \"");
  {
    char *p=FATType, c=8;
    for (; c; c--, p++) printc(*p);
  }
  _printf("\"\n\r");
  color_prints(Cyan,"BIOS data\n\r");
  _printf("         boot_drive: %d\n\r",boot_drive);
  _printf("  number_of_sectors: %d\n\r",number_of_sectors);
  _printf("    number_of_heads: %d\n\r",number_of_heads);
  _printf("           fat_type: %d\n\r",fat_type);
#endif

  fat_start=ReservedSectors;
  root_start=NumberOfFATs*SectorsPerFAT+fat_start;
  bytes_per_cluster=BytesPerSector*SectorsPerCluster;
  root_entries_per_sector=BytesPerSector/sizeof(Dir_Entry);
  dir_entries_per_cluster=bytes_per_cluster/sizeof(Dir_Entry);
  root_sectors=NumRootDirEntries/root_entries_per_sector;
  data_start=root_start+root_sectors;

#ifdef XVERBOSE
  color_prints(Cyan,"Calculated data\n\r");
  _printf("                fat_start: %d\n\r",fat_start);
  _printf("               root_start: %d\n\r",root_start);
  _printf("        bytes_per_cluster: %d\n\r",bytes_per_cluster);
  _printf("  root_entries_per_sector: %d\n\r",root_entries_per_sector);
  _printf("  dir_entries_per_cluster: %d\n\r",dir_entries_per_cluster);
  _printf("             root_sectors: %d\n\r",root_sectors);
  _printf("               data_start: %d\n\r",data_start);
  prints("\n\r");
#endif
}


/*
 Return 1 if cluster is a termination code.
*/
int FAT_EOF(int cluster) {
  switch (fat_type) {
    case 0:
        if (cluster>=0xFF8) return 1;
        return 0;
    case 1:
        if (cluster>=0xFFF8) return 1;
        return 0;
    default:
        color_prints(White,"\n\r\n\rINTERNAL PROGRAM ERROR (FAT_EOF())!!!");
        while (1);
  }
  return 0;
}


/*
 Return next cluster, reading it from FAT.
*/
int NextCluster(int cluster) {
  int sector, offset, hb;

  if (!fat_type) {
    sector=(cluster*3/2)/512;
    offset=(cluster*3/2)%512;
    hb=(cluster*3)%2;
  } else {
    sector=(cluster*2)/512;
    offset=(cluster*2)%512;
  }
  sector+=ReservedSectors;
  if (FAT_cached!=sector) {
    if (FAT_cached==-1) {
      FAT_cache=Alloca(BytesPerSector*2);
    }
    read_sector(sector,sector<root_start-1 ? 2 : 1,0,FAT_cache);
    FAT_cached=sector;
  }
  cluster=*(int *)(FAT_cache+offset);
  if (!fat_type) {
    if (hb)
      cluster=cluster >> 4;
    cluster=cluster & 0x0FFF;
  }
  return cluster;
}


/*
 Convert a file name (src) in a "readable" format (dest).
*
void ReadableFileName(char *src, char *dest) {
  char *p=src+7, *e=src+8;

  * Find effective end of the name. *
  while (*p==' ' && p>src) p--;
  * Copy the name. *
  for (; src<=p; src++, dest++) *dest=*src;
  * Check for the extension. *
  for (p=e+2; *p==' ' && p>e; p--);
  if (p>e || *e!=' ') {
    *dest++='.';
    for (; e<=p; e++, dest++) *dest=*e;
  }
  *dest='\0';
}
*/


/*
 Returns directory entry relative to a file in root directory.
*/
Dir_Entry *GetRootEntry(char *name) {
  char *dir;
  Dir_Entry *dirent;
  static Dir_Entry de;
  int dirsec, dirnum, notyetfound=1;

  dir=(char *)Alloca(BytesPerSector);
  for (dirsec=root_start; dirsec<data_start && notyetfound; dirsec++) {
    read_sector(dirsec,1,0,dir);
    dirnum=root_entries_per_sector;
    dirent=(Dir_Entry *)dir;
    for (; dirnum && notyetfound; dirnum--, dirent++) {
      if (!strncasecmp(name,dirent->filename,11)) {
        memcpy(&de,dirent,sizeof(Dir_Entry));
        notyetfound--;
      }
    }
  }
  free(dir);
  if (notyetfound)
    return 0;
  else
    return &de;
}


/*
 Returns directory entry relative to a file in a specific subdirectory
 pointed by cluster.
*/
Dir_Entry *GetDirEntry(char *name, word cluster) {
  char *dir;
  Dir_Entry *dirent;
  static Dir_Entry de;
  int dirnum, notyetfound=1;

  dir=(char *)Alloca(bytes_per_cluster);
  for (; !FAT_EOF(cluster); cluster=NextCluster(cluster)) {
    read_cluster(cluster,0,dir);
    dirnum=dir_entries_per_cluster;
    dirent=(Dir_Entry *)dir;
    for (; dirnum && notyetfound; dirnum--, dirent++) {
      if (!strncasecmp(&(dirent->filename[0]),name,11)) {
        memcpy(&de,dirent,sizeof(Dir_Entry));
        notyetfound--;
      }
    }
  }
  free(dir);
  if (notyetfound)
    return 0;
  else
    return &de;
}


/*
 Returns the formatted version for the name. Useful for name comparing.
*/
char *FormatName(char *name) {
  static char fname[12];
  char *n=name;
  int c=0;

  for (; c<11; ) {
    switch (*n) {
      case '\0':
        for (; c<11; c++) fname[c]=' ';
        break;
      case '.':
        if (c>8) {
          OPERATION_FAIL;
          ERRORP("\n\r  \"%s\" is a bad file name!\n\r",name);
          COMMENT("\n\r  Check configuration file and disk for errors!!!\n\r\n\r");
          PANIC;
        }
        for (; c<8; c++) fname[c]=' ';
        n++;
        break;
      default:
        fname[c]=toupper(*n);
        n++;
        c++;
    }
  }
  fname[11]='\0';
  return fname;
}





/*---------------------------------------------------------------------*/
/*                         Published functions.                        */
/*---------------------------------------------------------------------*/


/*
 Returns the initial cluster of the file or directory specified (name).
 It also check if it is a directory (isdir) and returns the size (dim).
 ATTENTION: the path must be always absolute.
*/
word GetStartCluster(char *name, int isdir, long *dim) {
  Dir_Entry *d;
  char *start=name, *end=name, *p;

  for (; *end; end++);   /* Search for the end of string. */
  if (*start!='/' || !*(start+1)) {
    OPERATION_FAIL;
    ERRORP("\n\r  \"%s\" is a bad file name!\n\r",name);
    COMMENT("\n\r  Check configuration file and disk for errors!!!\n\r\n\r");
    PANIC;
  }
  /* Scanning root directory. */
  start++;
  for (p=start; *p!='\0' && *p!='/'; p++);
  *p='\0';
  d=GetRootEntry(FormatName(start));
  if (!d || (p<end && !(d->attributes & 0x10))) {
    OPERATION_FAIL;
    if (!d) {
      ERRORP("\n\r  \"%s\" does not exists!\n\r",name);
    } else {
      ERRORP("\n\r  \"%s\" is not a directory!\n\r",name);
    }
    COMMENT("\n\r  Check configuration file and disk for errors!!!\n\r\n\r");
    PANIC;
  }
  if (p!=end) *p='/';
  p++;
  start=p;
  for (; p<end; ) {
    /* Scanning for subdirectories. */
    for (; *p!='\0' && *p!='/'; p++);
    *p='\0';
    d=GetDirEntry(FormatName(start),d->starting_cluster);
    if (!d || (p!=end && !(d->attributes & 0x10))) {
      OPERATION_FAIL;
      if (!d) {
        ERRORP("\n\r  \"%s\" does not exists!\n\r",name);
      } else {
        ERRORP("\n\r  \"%s\" is not a directory!\n\r",name);
      }
      COMMENT("\n\r  Check configuration file and disk for errors!!!\n\r\n\r");
      PANIC;
    }
    if (p!=end) {
      *p='/';
      p++;
      start=p;
    }
  }
  isdir=isdir ? 0x10 : 0;
  if (isdir!=(d->attributes & 0x10)) {
    OPERATION_FAIL;
    if (isdir) {
      ERRORP("\n\r  \"%s\" is not a directory!\n\r",name);
    } else {
      ERRORP("\n\r  \"%s\" is not a file!\n\r",name);
    }
    COMMENT("\n\r  Check configuration file and disk for errors!!!\n\r\n\r");
    PANIC;
  }
  *dim=d->file_size;
  return d->starting_cluster;
}


/*
 File loading.
*/
int Load(int cluster, uint segment, uint offset, uint endsegment) {
for (; !FAT_EOF(cluster); cluster=NextCluster(cluster)) {
    read_cluster(cluster,segment,offset);
    offset+=(bytes_per_cluster%16);
    segment+=(bytes_per_cluster/16);
    if (segment>endsegment)
      NoSpaceError(0);
  }
  return 0;
}


/*
 Returns an absolute path.
 If filename is already in absolute format no changes are done,
 otherwise it is combined with root.
*/
char *AbsolutePath(char *filename, char *root) {
  static FileName[MAX_FILE_NAME];
  char *p=FileName;

  if (*filename=='/') return filename;
  strcpy(FileName,root);
  for (; *p; p++);
  if (*(p-1)!='/') {
    *p='/';
    p++;
  }
  strcpy(p,filename);
  return FileName;
}
