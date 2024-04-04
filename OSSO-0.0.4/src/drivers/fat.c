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


/******************************* O S S O ***********************************
 * file : drivers/fat.c                                                    *
 * DOS FAT Driver                                                          *
 ***************************************************************************

 ***************************************************************************
 * $Id: fat.c,v 1.42 2001/08/23 19:21:11 iurlano Exp $
 ***************************************************************************
 *
 * $Log: fat.c,v $
 * Revision 1.42  2001/08/23 19:21:11  iurlano
 * Changed IDE to publish correctly each partition, whole disk included.
 * Added mount point support to vfs. Extended its protocol.
 * Changed fat driver to accept two mandatory parameters: device (the partitions with the fs) and mount (the mount point)
 * Now it is possible to mount a partition and actually see it int the FS!!
 * Works!
 * Changed fs.c in libOSSO to use the vfs for dir and file access.
 * Changed mount utility to inspect partitions and mount points state.
 * Added Documentation for FAT driver
 *
 * Revision 1.41  2001/08/22 14:19:26  iurlano
 * Added partition support to the FAT driver. Tested with hda3 and hda4 and seems working
 *
 * Revision 1.40  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.39  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.38  2001/07/08 11:12:50  steros
 * Converted MPS to dynamic allocation and other little fix.
 *
 * Revision 1.37  2001/07/03 16:58:23  steros
 * Long filename management complete (maybe...).
 *
 * Revision 1.36  2001/07/01 15:17:32  iurlano
 * Added first prototype of partition management system
 * Added mount program
 *
 * Revision 1.35  2001/06/29 17:45:52  steros
 * Added long file name for fat. Not complete!
 *
 * Revision 1.34  2001/06/26 17:47:57  steros
 * Corrected & added useful constants.
 *
 * Revision 1.33  2001/06/25 20:25:04  steros
 * Added vfat first try.
 *
 * Revision 1.32  2001/06/23 18:14:38  steros
 * Restructurated FAT to manage FAT32 and improve performance.
 *
 * Revision 1.31  2001/06/16 11:35:21  steros
 * Added dir scanning functions. Fixed various bugs.
 *
 * Revision 1.30  2001/06/14 12:35:08  iurlano
 * modified Get_Public_Dir_Entry_size to handle correctly the case of
 * the root_dir (returns only the non-null entries number)
 *
 * Revision 1.29  2001/06/13 21:04:48  steros
 * Various fixes.
 *
 * Revision 1.28  2001/06/13 16:45:28  steros
 * FAT/SHELL Restructuration, bug fixes and flush function to avoid line
 * buffering.
 *
 * Revision 1.27  2001/06/12 11:02:26  iurlano
 * Added interface fucntion to fat.c fs.c and others (not working)
 *
 * Revision 1.26  2001/06/12 10:18:01  iurlano
 * Added limited directory capabilities to the shell (cd command) not yet working
 *
 * Revision 1.25  2001/06/10 17:17:47  steros
 * Working on fat basic functions.
 *
 * Revision 1.24  2001/06/04 16:57:22  steros
 * Bug fixes.
 *
 * Revision 1.23  2001/06/03 17:54:51  steros
 * Fat restructuring.
 *
 * Revision 1.22  2001/05/30 21:25:05  steros
 * Corrected serial input as substitute of keyboard: now it work!
 * And other little adjustment...
 *
 * Revision 1.21  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 * Revision 1.20  2001/05/01 01:04:19  iurlano
 * Work on Dynamic Linker problem
 * Seems like that the shared MPS_Pid is working now.
 * I thing that the condivision of a variable from the lib and the program works if the variable is declared in the program.
 * for the other case, I've to work on it and I don't think I'm going to do it very soon.
 *
 * Revision 1.19  2001/04/29 17:40:48  steros
 * Added dynamic mps pid request on drv2mps also.
 * Improved mps and his protocols.
 * There's still problems in taking MPS pid from process MPS_Pid variable.
 *
 * Revision 1.18  2001/04/28 12:15:52  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.17  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.16  2001/04/26 22:09:09  iurlano
 * fat work (again)
 * working snapshot ?
 *
 * Revision 1.15  2001/04/26 19:25:54  iurlano
 * fat work
 * bugfixes
 *
 * Revision 1.14  2001/04/24 17:20:08  iurlano
 * fat now works.
 * Warning: instable snapshot
 * random errors launchi and forking processes
 * now the shell launches commands (only in the root dir)
 *
 * Revision 1.13  2001/04/23 22:16:13  iurlano
 * fat work
 *
 * Revision 1.12  2001/04/23 15:33:18  iurlano
 * work on FAT and fs protocol
 *
 * Revision 1.11  2001/04/23 14:43:05  iurlano
 * work on FAT module
 * work on FS protocol
 *
 * Revision 1.10  2001/04/22 20:04:29  iurlano
 * added fs management module in libOSSO
 * work on fs protocol
 *
 * Revision 1.9  2001/04/22 13:17:38  iurlano
 * Changed mpstest
 * mps now working
 * integrated some code for pid asking in the libosso
 *
 * Revision 1.8  2001/04/20 15:30:59  iurlano
 * Changes in to the fat driver
 * creating the filesystem protocol
 *
 * Revision 1.7  2001/04/19 15:17:58  iurlano
 * Fixed some bug
 * changed ide driver
 * changed fat driver (cabled the pid of the ide driver)
 * changed shell( now accept 3 commands mem, info, ls) TRY IT
 * VERY STABLE SNAPSHOT
 *
 * Revision 1.6  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.5  2001/03/25 15:27:25  steros
 * Divided MPS interfaces in more structured files.
 *
 * Revision 1.4  2001/03/10 17:08:48  steros
 * Added header and lib functions to interface driver to MPS.
 *
 * Revision 1.3  2001/01/08 09:52:54  steros
 * New boot procedure implemented.
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:01  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/


#include <types.h>
#include <string.h>
#include <memory.h>

#include <printf.h>

#include <drivers.h>
#include <messages.h>
#include <string.h>
#include <processes.h>
#include <protocols/mpsint.h>
#include <protocols/disk.h>
#include <protocols/vfs.h>
#include <protocols/filesys.h>
#include <protocols/drv2mps.h>

#include <drivers/protocols/mpsint.h>
#include <drivers/protocols/prg2mps.h>


/*-----------------------------------------------------------------*/
//#define DEBUG
#define LONGNAME_SUPPORT
#define READ_ONLY
/*-----------------------------------------------------------------*/


/*-------------------- FAT standard constants --------------------*/
#define MAX_SHORT_PATH_LEN     64   /* Max length of path in 8.3
                                       format names.              */
#define MAX_LONG_PATH_LEN     260   /* Max length of path in long
                                       format names.              */
#define MAX_LONG_FILENAME_LEN 255   /* Max length of filename in
                                       long format.               */
#define MAX_FILE_SIZE  0xFFFFFFFF   /* Max length of files.       */
#define MAX_DIR_ENTRIES     65536   /* Max number of entries in a
                                       directory chain            */
/*-----------------------------------------------------------------*/



/* WARNING this is only a prototype of driver */


#ifdef DYNAMIC_MPS_PID
  extern unsigned long MPS_Pid;
#endif


/* Max number of opened files. */
#define MAX_FILE_NUM 32


#define DIR_SEP '/'   /* Directory separator. */


typedef struct {
  byte   BS_jmpBoot[3]    __attribute__ ((packed));
  char   BS_OEMName[8]    __attribute__ ((packed));
  uint16 BPB_BytsPerSec   __attribute__ ((packed));
  byte   BPB_SecPerClus   __attribute__ ((packed));
  uint16 BPB_RsvdSecCnt   __attribute__ ((packed));
  byte   BPB_NumFATs      __attribute__ ((packed));
  uint16 BPB_RootEntCnt   __attribute__ ((packed));
  uint16 BPB_TotSec16     __attribute__ ((packed));
  byte   BPB_Media        __attribute__ ((packed));
  uint16 BPB_FATSz16      __attribute__ ((packed));
  uint16 BPB_SecPerTrk    __attribute__ ((packed));
  uint16 BPB_NumHeads     __attribute__ ((packed));
  uint32 BPB_HiddSec      __attribute__ ((packed));
  uint32 BPB_TotSec32     __attribute__ ((packed));
  union {
    struct FAT16_t {
      byte   BS_DrvNum          __attribute__ ((packed));
      byte   BS_Reserved1       __attribute__ ((packed));
      byte   BS_BootSig         __attribute__ ((packed));
      uint32 BS_VolID           __attribute__ ((packed));
      char   BS_VolLab[11]      __attribute__ ((packed));
      char   BS_FilSysType[8]   __attribute__ ((packed));
    } FAT16   __attribute__ ((packed));
    struct FAT32_t {
      uint32 BPB_FATSz32        __attribute__ ((packed));
      uint16 BPB_ExtFlags       __attribute__ ((packed));
      uint16 BPB_FSVer          __attribute__ ((packed));
      uint32 BPB_RootClus       __attribute__ ((packed));
      uint16 BPB_FSInfo         __attribute__ ((packed));
      uint16 BPB_BkBootSec      __attribute__ ((packed));
      byte   BPB_Reserved[12]   __attribute__ ((packed));
      byte   BS_DrvNum          __attribute__ ((packed));
      byte   BS_Reserved1       __attribute__ ((packed));
      byte   BS_BootSig         __attribute__ ((packed));
      uint32 BS_VolID           __attribute__ ((packed));
      char   BS_VolLab[11]      __attribute__ ((packed));
      char   BS_FilSysType[8]   __attribute__ ((packed));
    } FAT32   __attribute__ ((packed));
  } Type      __attribute__ ((packed));
} BootSect_t  __attribute__ ((packed));

byte BSector[4096];

BootSect_t     *BootSect=(BootSect_t *)BSector;
struct FAT16_t *Boot16=&(((BootSect_t *)BSector)->Type.FAT16);
struct FAT32_t *Boot32=&(((BootSect_t *)BSector)->Type.FAT32);

typedef struct __Dir_Entry {
  char   filename[8];
  char   ext[3];
  byte   attributes;
  byte   NTRes;
  byte   CreationTimeTenth;
  uint16 CreationTime;
  uint16 CreationDate;
  uint16 LastAcccessDate;
  uint16 starting_cluster_H;
  uint16 LastWriteTime;
  uint16 LastWriteDate;
  uint16 starting_cluster_L;
  uint32 file_size;
} Dir_Entry __attribute__ ((packed));

#ifdef LONGNAME_SUPPORT
typedef struct __LongName_Entry {
  struct signature_t {
    byte sequence_number:5,
         unused1:1,
         final_component:1,   /* Set if last component of long name. */
         is_deleted:1;
  } signature              __attribute__ ((packed));
  char   lname1[10]        __attribute__ ((packed));
  byte   attributes        __attribute__ ((packed));
  byte   flags             __attribute__ ((packed));
  byte   checksum          __attribute__ ((packed));
  char   lname2[12]        __attribute__ ((packed));
  uint16 first_cluster     __attribute__ ((packed));
  char   lname3[4]         __attribute__ ((packed));
} LongName_Entry __attribute__ ((packed));
#endif

#define DE_STARTING_CLUSTER(e) (((e).starting_cluster_H<<16) |     \
                               (e).starting_cluster_L)

#define FAT_ATTR_READ_ONLY         0x01
#define FAT_ATTR_HIDDEN            0x02
#define FAT_ATTR_SYSTEM            0x04
#define FAT_ATTR_VOLUME_ID         0x08
#define FAT_ATTR_DIRECTORY         0x10
#define FAT_ATTR_ARCHIVE           0x20
#define FAT_ATTR_LONG_NAME    (FAT_ATTR_READ_ONLY|FAT_ATTR_HIDDEN|FAT_ATTR_SYSTEM|FAT_ATTR_VOLUME_ID)

typedef char FAT_File_Name[12];


enum FAT_Type_t {
  FAT12,
  FAT16,
  FAT32
} FAT_Type;


/* Shift values for fast calculations. */
uint16 BytesPerSectorSH,
       SizeOfDirEntrySH=5;

uint16 BytesPerSector,
       RootDirSectors,
       NumRootDirEntries;
uint32 FATSz, EOF_Mark, BAD_Mark,
       FirstDataSector,
       TotalSectors,
       CountOfClusters,
       DataSec,
       BytesPerCluster,
       DirEntriesPerCluster;
ulong DriveLogicalStart,
      Fat1StartSector,
      RootDirStartSector,
      DataStartSector;

Dir_Entry *root_dir=0;
byte *First_FAT=0;


char Root_Directory[2] = { DIR_SEP, '\0' };

#ifdef LONGNAME_SUPPORT
  char LongName[MAX_LONG_FILENAME_LEN+1];
  char ExtLongName[MAX_LONG_FILENAME_LEN+1]; // Used by ExtractFirstLongName.
#endif

/* This data should be as a sort of little global cache. */
/* THIS ONLY WORKS FOR MONO_PARTITION - MONO_THREAD!!!!! */

char *Current_Directory = Root_Directory;
Dir_Entry Current_DirEntry;

int CurrentCluster=0;
char *Cluster=0;



/* Disk Device Informations */

char DiskDeviceName[50];
char MountPoint[MAX_MOUNT_PATH];
dword DiskDevicePid=-1;
byte DiskPartTag=-1;



/*-----------------------------------------------------------------*/
/*                Data of a single file descriptor.                */
/*-----------------------------------------------------------------*/

typedef struct {
  char      *CurrentFileName;
  Dir_Entry DirEntry;
  uint32    CurrentCluster;
  char      *Cluster;
  ulong     FileOffset;   /* If used by directory scanning function */
                          /* and current directory is '/', it is    */
                          /* the index of the entry in root dir.    */
  struct {
    int //read:1,
        //write:1,
        in_synch:1,   /* Indicates if CurrentCluster is loaded. */
        dir_mode:1,   /* Set if opened as dir. */
        is_root:1;    /* Set if is root dir.   */
  } bits;
} FileData_t;

FileData_t FileData[MAX_FILE_NUM];
int NumOpenFiles=0;





/*
 Initialize structures.
*/
void Init(void) {
  FileData_t *fd=FileData;
  int c;

  for (c=0; c<MAX_FILE_NUM; c++, fd++) {
    fd->CurrentFileName=0;
    //fd->DirEntry...
    fd->CurrentCluster=0;
    fd->Cluster=0;
    fd->FileOffset=0;
  }
  NumOpenFiles=0;
  #ifdef LONGNAME_SUPPORT
    LongName[MAX_LONG_FILENAME_LEN]='\0';
  #endif
}




void Exit(void) {
  DriverExit();
  exit();
}




/*-----------------------------------------------------------------*/
/*                   Disk interfacing functions.                   */
/*-----------------------------------------------------------------*/

int read_sector(int id, int start_logical_sector, byte *settore)  {
  msginfo mi;
  DISK_COMMAND *msg;
  byte *answ;
  int i;

  msg=getbmsg();
  msg->command=DSK_CMD_READ_SECTOR;
  msg->arg1=0;
  msg->tag=DiskPartTag;
  msg->arg2=id+start_logical_sector;
  msg->next_cmd=0;
  send_msg(DiskDevicePid,msg,sizeof(*msg));
  answ=recv_msg(&mi);
  for (i=0; i<BytesPerSector; i++)
    settore[i]=answ[i+sizeof(DISK_COMMAND)];
  freemsg(answ);
  return 0;
}


int write_sector(int id, int start_logical_sector, byte *settore) {
#ifndef READ_ONLY
  msginfo mi;
  DISK_COMMAND *msg;
  byte *answ;
  int i;

  msg=getbmsg();
  msg->command=DSK_CMD_WRITE_SECTOR;
  msg->arg1=0;
  msg->tag=DiskPartTag;
  msg->arg2=id+start_logical_sector;
  msg->next_cmd=0;
  for (i=0; i<BytesPerSector; i++)
    answ[i+sizeof(DISK_COMMAND)]=settore[i];
  send_msg(DiskDevicePid,msg,sizeof(*msg)+BytesPerSector);
  answ=recv_msg(&mi);
  freemsg(answ);
#endif
  return 0;
}


void read_cluster(uint32 clustnum, byte *memory) {
  unsigned int t, k;


  for(t=0;t<BootSect->BPB_SecPerClus;t++) {
    k=FirstDataSector+((clustnum-2)*BootSect->BPB_SecPerClus)+t;

//printf("[* FAT:read_cluster] cluster %u, settore %d\n",clustnum,k);

    if (k>83300) {
      printf("[***FAT***] Sector reading out of bounds (%x)\n",k);
      while (1) ;
    } else
      read_sector(0,k,memory+(t<<BytesPerSectorSH));
  }
}


void write_cluster(uint32 clustnum, byte *memory) {
  unsigned int t,k;

  for(t=0;t<BootSect->BPB_SecPerClus;t++) {
    k=FirstDataSector+((clustnum-2)*BootSect->BPB_SecPerClus)+t;
    //printf("Writing cluster: Sector %d\n",k);
    write_sector(0,k,memory+(t<<BytesPerSectorSH));
  }
}


/*-----------------------------------------------------------------*/
/*                    Initialization functions.                    */
/*-----------------------------------------------------------------*/


/*
 Return the shift value for a number with only a bit set.
*/
int GetShiftValue(uint16 value) {
  uint16 v=1;
  int i=0, ris=-1;

  for (; i<16; i++, v<<=1) {
    if (value&v) {
      if (ris!=-1) return -2;
      ris=i;
    }
  }
  return ris;
}


/*
 Initialize FAT.
*/
void FS_init(int start_logical_sector) {
  int k;

  BytesPerSector=512;  /* This should be 4096: IDE must support it. */
  if (read_sector(0,start_logical_sector,BSector)) {
    printf("Error reading sector\n");
    return;
  }
  BytesPerSectorSH=GetShiftValue(BootSect->BPB_BytsPerSec);
  if (BytesPerSectorSH<=0) {
    printf("[FAT] Error: bytes per sector is not a power of 2!!!!");
  }
  RootDirSectors=((BootSect->BPB_RootEntCnt<<SizeOfDirEntrySH)+
                  (BootSect->BPB_BytsPerSec-1))>>BytesPerSectorSH;

  if (BootSect->BPB_FATSz16)
    FATSz=BootSect->BPB_FATSz16;
  else
    FATSz=BootSect->Type.FAT32.BPB_FATSz32;
  FirstDataSector=BootSect->BPB_RsvdSecCnt+
                  (BootSect->BPB_NumFATs*FATSz)+RootDirSectors;
  if (BootSect->BPB_TotSec16)
    TotalSectors=BootSect->BPB_TotSec16;
  else
    TotalSectors=BootSect->BPB_TotSec32;
  DataSec=TotalSectors-(BootSect->BPB_RsvdSecCnt+
          (BootSect->BPB_NumFATs*FATSz)+RootDirSectors);
  CountOfClusters=DataSec/BootSect->BPB_SecPerClus;

  if (CountOfClusters<4085) {
    FAT_Type=FAT12;
  } else
    if (CountOfClusters<65525) {
      FAT_Type=FAT16;
    } else {
      FAT_Type=FAT32;
    }

  switch (FAT_Type) {
    case FAT12:
      EOF_Mark=0x0FF8;
      BAD_Mark=0x0FF7;
      break;
    case FAT16:
      EOF_Mark=0xFFF8;
      BAD_Mark=0xFFF7;
      break;
    default:   // case FAT32
      EOF_Mark=0x0FFFFFF8;
      BAD_Mark=0x0FFFFFF7;
  }

  DriveLogicalStart=start_logical_sector;
  Fat1StartSector=BootSect->BPB_RsvdSecCnt;
  RootDirStartSector=Fat1StartSector+FATSz*BootSect->BPB_NumFATs;
  DataStartSector=RootDirStartSector+RootDirSectors;

  BytesPerCluster=BootSect->BPB_SecPerClus<<BytesPerSectorSH;
  DirEntriesPerCluster=BytesPerCluster>>SizeOfDirEntrySH;
  NumRootDirEntries=(RootDirSectors<<BytesPerSectorSH)>>SizeOfDirEntrySH;

  /* Checks for consistency of fat information. */
  if (BSector[510]!=0x55 || BSector[511]!=0xAA ||
      BootSect->BPB_RsvdSecCnt<1 || BootSect->BPB_NumFATs<1 ||
      (FAT_Type==FAT32 && (BootSect->BPB_RootEntCnt ||
                           BootSect->BPB_FATSz16)))
  {
    printf("[FAT] ERROR: This is not a FAT filesystem or it is damaged!\n");
    Exit();
  }

  if (Cluster) freeosso(Cluster);
  if (root_dir) freeosso(root_dir);
  if (First_FAT) freeosso(First_FAT);

  Cluster=mallosso(BytesPerCluster);
  root_dir=mallosso(RootDirSectors<<BytesPerSectorSH);
  First_FAT=mallosso(FATSz<<BytesPerSectorSH);

  for (k=0; k<FATSz; k++)
    read_sector(0,DriveLogicalStart+Fat1StartSector+k,
                ((byte *)First_FAT)+(k<<BytesPerSectorSH));
  for (k=0; k<RootDirSectors; k++)
    read_sector(0,DriveLogicalStart+RootDirStartSector+k,
                ((byte *)root_dir)+(k<<BytesPerSectorSH));

  printf("FAT Parameters:\n");
  printf("  OEM Name          : ");
  for (k=0; k<8; k++) printf("%c",BootSect->BS_OEMName[k]);
  printf("\n  Bytes Per Sector  : %d (%d)\n",BytesPerSector,
                                             BytesPerSectorSH);
  printf("  Reserved Sectors  : %d\n",BootSect->BPB_RsvdSecCnt);
  printf("  Sector Per Cluster: %d\n",BootSect->BPB_SecPerClus);
  printf("  FATs Number       : %d\n",BootSect->BPB_NumFATs);
  printf("  Logical Sectors   : %d\n",TotalSectors);
  printf("  Root Dir Entry    : %d\n",BootSect->BPB_RootEntCnt);
  printf("  Medium Descriptor : %x\n",BootSect->BPB_Media);
  printf("  Sectors Per Fat   : %d\n",FATSz);
  printf("  Sectors Per Track : %d\n",BootSect->BPB_SecPerTrk);
  printf("  Heads Number      : %d\n",BootSect->BPB_NumHeads);
  printf("  Hidden Sectors    : %d\n",BootSect->BPB_HiddSec);
  printf("  FAT Type          : %s\n",(FAT_Type==FAT12 ? "FAT12" :
                                      (FAT_Type==FAT16 ? "FAT16" :
                                                         "FAT32")));
}



/*-----------------------------------------------------------------*/
/*                        String functions.                        */
/*-----------------------------------------------------------------*/

/*
 Convert a file name from FAT format to the readable one.
 Returns a pointer to the converted name.
*/
char *FATFormat2Readable(char *fatformat, char *readable) {
  char *p=readable;
  int i=8;

  if ((unsigned char)*fatformat!=0xE5) {
    for (; i; i--) *(readable++)=*(fatformat++);
    for (; *(readable-1)==' '; readable--);
    if (*fatformat!=' ' || *(fatformat+1)!=' ' ||
        *(fatformat+2)!=' ') *(readable++)='.';
    for (i=3; i; i--) *(readable++)=*(fatformat++);
    for (; *(readable-1)==' '; readable--);
  }
  *readable='\0';
  return p;
}


/*
 Convert a file name from readable format to the FAT one.
 Returns a pointer to the converted name.
 If newp!=0, at that address will be written the pointer to the
 character where the string processing has finished.
*/
char *Readable2FATFormat(char *readable, char *fatformat, char **newp) {
  char *p=fatformat;
  int i=8;

  if (*readable==DIR_SEP) readable++;
  /* Copy name part. */
  for (; i && *readable!='\0' && *readable!=DIR_SEP && *readable!='.';
       *(fatformat++)=*(readable++), i--);
  if (*readable=='.') readable++;
  while (fatformat-p<8) *(fatformat++)=' ';
  /* Copy extension part. */
  for (i=3; i && *readable!='\0' && *readable!=DIR_SEP;
       *(fatformat++)=*(readable++), i--);
  while (fatformat-p<11) *(fatformat++)=' ';
  *fatformat='\0';
  if (newp) *newp=readable;
  return p;
}


#ifdef LONGNAME_SUPPORT
/*
 Extract first file/dir name from path.
*/
char *ExtractFirstName(char *path) {
  char *d=ExtLongName;

  while (*path==DIR_SEP) path++;
  for (; *path!=DIR_SEP && *path; ) *(d++)=*(path++);
  *d='\0';
  return ExtLongName;
}
#endif


/*-----------------------------------------------------------------*/
/*                         Base functions.                         */
/*-----------------------------------------------------------------*/


#define FAT_EOF(cluster) ((cluster)>=EOF_Mark || !(cluster))

#define LONGNAME_ENTRY(direntry) ((direntry).attributes&FAT_ATTR_LONG_NAME)

#define VALID_ENTRY(direntry) ((direntry).filename &&            \
                               *((direntry).filename) &&         \
                               *((direntry).filename)!=' ' &&    \
                               (unsigned char)*((direntry).filename)!=0xE5 \
                               && !(LONGNAME_ENTRY(direntry)))


/*
 Return next cluster, reading it from FAT table.
*/
uint32 GetNextCluster(uint32 cluster) {
  uint32 c;

  switch (FAT_Type) {
    case FAT12:
      c=((uint16 *)First_FAT)[cluster+(cluster>>1)];
      if (cluster&1) c>>=4;
      c&=0x0FFF;
      break;
    case FAT16:
      c=((uint16 *)First_FAT)[cluster];
      break;
    default:
      c=(((uint32 *)First_FAT)[cluster])&0x0FFFFFFF;
  }
  return c;
}


/*
 Load the cluster specified in the global space if not already
 present (if index = -1) else in file data structure.
*/
void GetCluster(uint32 cluster, int index) {
  if (index<0) {
    if (cluster==CurrentCluster) return;
    read_cluster(cluster,Cluster);
    CurrentCluster=cluster;
  } else {
    FileData_t *fd=&(FileData[index]);

    if (cluster==fd->CurrentCluster && fd->bits.in_synch) return;
    read_cluster(cluster,fd->Cluster);
    fd->CurrentCluster=cluster;
    fd->bits.in_synch=1;
  }
}


/*
 Get next cluster in file table updating it.
*/
void Struct_NextCluster(FileData_t *fd) {
  fd->CurrentCluster=GetNextCluster(fd->CurrentCluster);
  fd->bits.in_synch=0;
}


#ifdef LONGNAME_SUPPORT
/*
 Check if a name correspond to an entry.
*/
int ProcessEntry(char *shortname, char *longname, Dir_Entry *de) {
  static char *p=0;
  int i;

  if (!de) {
    p=&(LongName[MAX_LONG_FILENAME_LEN]);
    return 0;
  }

  if (LONGNAME_ENTRY(*de) && !(((LongName_Entry *)de)->signature.is_deleted)) {
    p-=13;
    for (i=0; i<5; i++) *(p+i)=((LongName_Entry *)de)->lname1[i*2];
    for (i=0; i<6; i++) *(p+5+i)=((LongName_Entry *)de)->lname2[i*2];
    for (i=0; i<2; i++) *(p+11+i)=((LongName_Entry *)de)->lname3[i*2];
    // Save the checksum to compare it with short name...
  };

  if (VALID_ENTRY(*de)) {
    if (!strcasecmp(shortname,de->filename)) return 1;
      #ifdef LONGNAME_SUPPORT
        if (*p!='\0' && !strcmp(longname,p)) return 1;
      #endif
    p=&(LongName[MAX_LONG_FILENAME_LEN]);
  }
  return 0;
}
#endif


/*
 Get the root dir entry corresponding to the specified file (in
 fat format) and returns it in "de".
 Returns 0 if ok.
*/
#ifdef LONGNAME_SUPPORT
int GetRootDirEntry(char *longname, char *shortname, Dir_Entry **de)
#else
int GetRootDirEntry(char *shortname, Dir_Entry **de)
#endif
{
  uint16 t;

  #ifdef LONGNAME_SUPPORT
    ProcessEntry(0,0,0);
  #endif
  for (t=0;t<BootSect->BPB_RootEntCnt;t++) {
    #ifdef LONGNAME_SUPPORT
      if (ProcessEntry(shortname,longname,&(root_dir[t])))
    #else
      if (!strcasecmp(filename,root_dir[t].filename))
    #endif
    {
      *de=&(root_dir[t]);
      return 0;
    }
  }
  return -1;
}


/*
 Get the dir entry corresponding to the specified file (in fat
 format) in the specified directory entry (sde) and returns it in
 "de".
 Returns 0 if ok.
*/
#ifdef LONGNAME_SUPPORT
int GetDirEntry(char *longname, char *shortname, Dir_Entry *sde,
                Dir_Entry **de)
#else
int GetDirEntry(char *shortname, Dir_Entry *sde, Dir_Entry **de)
#endif
{
  Dir_Entry *pstart=(Dir_Entry *)Cluster, *p;
  uint32 c=DE_STARTING_CLUSTER(*sde);
  uint16 l;

  #ifdef LONGNAME_SUPPORT
    ProcessEntry(0,0,0);
  #endif
  for (; !FAT_EOF(c); c=GetNextCluster(c)) {
    GetCluster(c,-1);
    for (l=DirEntriesPerCluster, p=pstart; l; p++, l--) {
      #ifdef LONGNAME_SUPPORT
        if (ProcessEntry(shortname,longname,p))
      #else
        if (!strcasecmp(shortname,p->filename))
      #endif
      {
        *de=p;
        return 0;
      }
    }
  }
  return -1;
}


/*
 Find a directory entry, starting from the path specified (in
 readable format).
 Returns 0 if ok.
*/
int LocateEntry(char *path, Dir_Entry **de) {
  char nome[12], *p;
  int res;

#ifdef DEBUG
printf("[* FAT:LocateEntry] <START> path='%s'\n",path);
#endif
  if (*path==DIR_SEP ||
     (*Current_Directory==DIR_SEP && !*(Current_Directory+1)))
  {
  #ifdef LONGNAME_SUPPORT
    p=ExtractFirstName(path);
    res=GetRootDirEntry(p,Readable2FATFormat(path,nome,&path),de);
  #else
    res=GetRootDirEntry(Readable2FATFormat(path,nome,&path),de);
  #endif
    if (res) return res;
  } else {
    *de=&Current_DirEntry;
  }
  if (!de) return 0;

  while (*path!='\0' && *(path+1)!='\0') {
    if (!((*de)->attributes & FAT_ATTR_DIRECTORY))  // Not a directory!
        return 0;
  #ifdef LONGNAME_SUPPORT
    p=ExtractFirstName(path);
    res=GetDirEntry(p,Readable2FATFormat(path,nome,&path),*de,de);
  #else
    res=GetDirEntry(Readable2FATFormat(path,nome,&path),*de,de);
  #endif
    if (res) return res;
  }
#ifdef DEBUG
printf("[* FAT:LocateEntry] <END>\n");
#endif
  return 0;
}


/*
 Find the directory entry number "num", in the path specified
 (in readable format).
 Returns the available contiguous entries.
*/
uint16 LocateNumEntry(char *path, uint16 idx, Dir_Entry **de) {
  Dir_Entry *pstart=(Dir_Entry *)Cluster, *p;
  char nome[12], *lp;
  uint16 c;
  uint32 i=0;
  int res, num=idx;

  if (*path==DIR_SEP && *(path+1)=='\0') {
    for (i=NumRootDirEntries, p=root_dir; i && num>=0; p++, i--) {
      if (p->filename && *(p->filename) && *(p->filename)!=' ' &&
          (unsigned char)*(p->filename)!=0xE5) {
        if (!num) {
          *de=p;
          return i;
        }
        num--;
      }
    }
  } else {
  #ifdef LONGNAME_SUPPORT
    lp=ExtractFirstName(path);
    res=GetRootDirEntry(lp,Readable2FATFormat(path,nome,&path),de);
  #else
    res=GetRootDirEntry(Readable2FATFormat(path,nome,&path),de);
  #endif
    if (res) return res;
    c=DE_STARTING_CLUSTER(*(*de));
    for (; !FAT_EOF(c) && num>=0; c=GetNextCluster(c)) {
      GetCluster(c,-1);
      for (i=DirEntriesPerCluster, p=pstart; i && *(p->filename); p++, i--) {
        if (VALID_ENTRY(*p)) {
          if (!num) {
            *de=p;
            return DirEntriesPerCluster-i;
          }
//          num--;   // Count only valid entries.
        }
        num--;  // Count all entries.
      }
    }
  }
  return 0;
}



/*-----------------------------------------------------------------*/
/*                      High level functions.                      */
/*-----------------------------------------------------------------*/


/*
 Returns the start cluster for the specified file.
*/
uint32 get_start_cluster(char *filename) {
  Dir_Entry *de;
  int res;

  if (*filename==DIR_SEP && *(filename+1)=='\0')
    return -1;
  res=LocateEntry(filename,&de);
  if (!res) return DE_STARTING_CLUSTER(*de);
  return 0;
}


/*
 Returns the file size for the specified file.
*/
ulong get_file_size(char *filename) {
  Dir_Entry *de;
  uint32 cl;
  int res;

/*
  if (*filename==DIR_SEP && *(filename+1)=='\0')
    return FAT0.Root_Dir_Sector_Size*FAT0.Bytes_Per_Sector;
*/
  res=LocateEntry(filename,&de);
  if (res) return 0;
  if (de->attributes & FAT_ATTR_DIRECTORY) {
    cl=DE_STARTING_CLUSTER(*de);
    if (!cl) return 0;
    res=BytesPerCluster;
    for (; !FAT_EOF(cl); cl=GetNextCluster(cl), res+=BytesPerCluster);
    return res;
  } else
    return de->file_size;
}


/*
 Open a file returning a descriptor to it.
*/
int fs_open(char *filename) {
  FileData_t *fd=FileData;
  Dir_Entry *de;
  int c;

  if (NumOpenFiles>=MAX_FILE_NUM) return 0;

  /* Search for file. */
  if (*filename==DIR_SEP && *(filename+1)=='\0')
    return 0;
  if (LocateEntry(filename,&de)) return 0;

  /* Find a free entry. */
  for (c=0; c<MAX_FILE_NUM && fd->CurrentFileName; c++, fd++);
  c++;

  /* Update file structures. */
  fd->CurrentFileName=(char *)mallosso(strlen(filename)+1);
  strcpy(fd->CurrentFileName,filename);
  memcpy(&(fd->DirEntry),de,sizeof(Dir_Entry));
  fd->CurrentCluster=DE_STARTING_CLUSTER(*de);
  if (!fd->Cluster)
    fd->Cluster=mallosso(BytesPerCluster);
  fd->bits.in_synch=0;
  fd->bits.dir_mode=0;
  fd->bits.is_root=0;
  fd->FileOffset=0;
  NumOpenFiles++;

  return c;
}


/*
 Close a file.
 Return 0 if ok.
*/
int fs_close(int descr) {
  FileData_t *fd=&(FileData[descr-1]);

  if (!fd->CurrentFileName) return -1;
  freeosso(fd->CurrentFileName);
  fd->CurrentFileName=0;
  NumOpenFiles--;
  return 0;
}


/*
 Position file pointer to the specified offset.
 It works like Linux "lseek".
*/
long fs_lseek(int descr, long offset, int whence) {
  FileData_t *fd=&(FileData[descr-1]);
  uint32 cluster_idx, cluster;

  /* Get absolute offset. */
  switch (whence) {
    case SEEK_CUR:
      offset+=fd->FileOffset;
      break;
    case SEEK_END:
      if (offset>=0) return -1;
      offset+=fd->DirEntry.file_size;
      break;
  }
  if (offset>=fd->DirEntry.file_size) return -1;
  cluster_idx=offset/BytesPerCluster;

  /* If offset is not in CurrentCluster, update it. */
  if ((fd->FileOffset/BytesPerCluster)!=cluster_idx) {
    cluster=DE_STARTING_CLUSTER(fd->DirEntry);
    for (; !FAT_EOF(cluster) && cluster_idx;
         cluster_idx--, cluster=GetNextCluster(cluster));
    fd->CurrentCluster=cluster;
    fd->bits.in_synch=0;
  }

  /* Update file offset. */
  fd->FileOffset=offset;
  return offset;
}


/*
 Read an opened file.
 If offset < 0 then load from actual file offset.
 Return number of bytes read.
*/
ulong fs_read(int descr, long offset, ulong count, byte *memory) {
  FileData_t *fd=&(FileData[descr-1]);
  ulong numbytes=0, start, num;

#ifdef DEBUG
  printf("[* FAT:fs_read] <START> descr=%d, offset=%d, count=%d\n",
         descr,offset,count);
#endif
  if (!(fd->CurrentFileName) || fd->bits.dir_mode) return 0;

  /* Find the correct starting cluster. */
  if (offset>=0) {   /* If not a contiguous reading, reposition. */
    if (fs_lseek(descr,offset,SEEK_SET)<0) return 0;
  } else
    offset=fd->FileOffset;

  /* If request is out of boundary, resize count value. */
  if (offset+count>fd->DirEntry.file_size)
    count=fd->DirEntry.file_size-offset;

  /* If first cluster is not whole... */
  start=offset%BytesPerCluster;
  if (start && count && !FAT_EOF(fd->CurrentCluster)) {
    num=BytesPerCluster-start;
    if (num>count) num=count;
    if (!fd->bits.in_synch) GetCluster(fd->CurrentCluster,descr-1);
    memcpy(memory,fd->Cluster+start,num);
    Struct_NextCluster(fd);
    count-=num;
    numbytes+=num;
    memory+=num;
  }

  /* Read whole cluster. */
  while (!FAT_EOF(fd->CurrentCluster) && count>=BytesPerCluster) {
    read_cluster(fd->CurrentCluster,memory);
    count-=BytesPerCluster;
    numbytes+=BytesPerCluster;
    memory+=BytesPerCluster;
    Struct_NextCluster(fd);
  }

  /* If last cluster is not whole... */
  if (count && !FAT_EOF(fd->CurrentCluster)) {
    num=BytesPerCluster-start;
    if (!fd->bits.in_synch) GetCluster(fd->CurrentCluster,descr-1);
    memcpy(memory,fd->Cluster,count);
    numbytes+=count;
  }

  fd->FileOffset+=numbytes;
#ifdef DEBUG
  printf("[* FAT:fs_read] <END> numbytes=%d\n",numbytes);
#endif
  return numbytes;
}



/* --------------- Directory scanning functions --------------- */


/*
 Open a directory returning a descriptor to it.
*/
int fs_open_dir(char *dirname) {
  FileData_t *fd=FileData;
  Dir_Entry *de;
  int c;

#ifdef DEBUG
  printf("[* FAT:fs_open_dir] <START> dirname='%s'\n",dirname);
#endif
  if (NumOpenFiles>=MAX_FILE_NUM) return 0;

  /* Search for file. */
  if (!*dirname) return 0;
  if (*dirname==DIR_SEP && !*(dirname+1))
    de=0;
  else {
    if (LocateEntry(dirname,&de)) return 0;
    if (!(de->attributes & FAT_ATTR_DIRECTORY)) return 0;  /* Not a dir. */
  }

  /* Find a free entry. */
  for (c=0; c<MAX_FILE_NUM && fd->CurrentFileName; c++, fd++);
  c++;

  /* Update file structures. */
  fd->CurrentFileName=(char *)mallosso(strlen(dirname)+1);
  strcpy(fd->CurrentFileName,dirname);
  if (de) {
    memcpy(&(fd->DirEntry),de,sizeof(Dir_Entry));
    fd->CurrentCluster=DE_STARTING_CLUSTER(*de);
    fd->bits.is_root=0;
  } else {
    fd->bits.is_root=1;
    fd->CurrentCluster=0;
  }
  if (!fd->Cluster)
    fd->Cluster=mallosso(BytesPerCluster);
  fd->bits.in_synch=0;
  fd->bits.dir_mode=1;
  fd->FileOffset=0;
  NumOpenFiles++;

#ifdef DEBUG
  printf("[* FAT:fs_open_dir] <END> descr=%d\n",c);
#endif
  return c;
}


/*
 Close a directory.
 Returns 0 if ok.
*/
int fs_close_dir(int descr) {
  return fs_close(descr);
}


/*
 Read an opened directory.
 Returns number of entries read.
*/
int fs_read_dir(int descr, dword count, FS_PUBLIC_DIR_ENTRY *direntries) {
  FileData_t *fd=&(FileData[descr-1]);
  FS_PUBLIC_DIR_ENTRY *pdir=direntries;
  Dir_Entry *de;
  dword num=0;
  char go_on=1;
  #ifdef LONGNAME_SUPPORT
    char *p=&(LongName[MAX_LONG_FILENAME_LEN]);
    int i;
  #endif

#ifdef DEBUG
  printf("[* FAT:fs_read_dir] <START> descr=%d, count=%d\n",descr,count);
#endif
  if (!(fd->CurrentFileName) || !fd->bits.dir_mode) return 0;

  if (fd->bits.is_root) {
    if (fd->FileOffset>=BootSect->BPB_RootEntCnt) return 0;
    de=&(root_dir[fd->FileOffset]);
  } else {
    if (fd->FileOffset/BytesPerCluster!=fd->CurrentCluster/BytesPerCluster)
      if (fs_lseek(descr,fd->FileOffset,SEEK_SET)<0) return 0;
    if (FAT_EOF(fd->CurrentCluster)) return 0;
    if (!fd->bits.in_synch) GetCluster(fd->CurrentCluster,descr-1);
    de=&(((Dir_Entry *)fd->Cluster)[(fd->FileOffset%BytesPerCluster)>>
                                     SizeOfDirEntrySH]);
  }

  for (; num<count && go_on && *(de->filename);) {

    #ifdef LONGNAME_SUPPORT
      if (LONGNAME_ENTRY(*de) && !(((LongName_Entry *)de)->signature.is_deleted)) {
        p-=13;
        for (i=0; i<5; i++) *(p+i)=((LongName_Entry *)de)->lname1[i*2];
        for (i=0; i<6; i++) *(p+5+i)=((LongName_Entry *)de)->lname2[i*2];
        for (i=0; i<2; i++) *(p+11+i)=((LongName_Entry *)de)->lname3[i*2];
        // Save the checksum to compare it with short name...
      };
    #endif
    if (VALID_ENTRY(*de)) {
      #ifdef LONGNAME_SUPPORT
        if (*p!='\0') {
          //printf("[FAT:LONGFILENAME] <%s>\n",p);
          strcpy(pdir->filename,p);
        } else
      #endif
      FATFormat2Readable(de->filename,pdir->filename);
      pdir->file_size=de->file_size;
      pdir->attribute=de->attributes&FAT_ATTR_DIRECTORY ? S_IFDIR : S_IFREG;
      if (!(de->attributes&FAT_ATTR_ARCHIVE)) pdir->attribute|=00111;
      if (de->attributes&FAT_ATTR_READ_ONLY)
        pdir->attribute|=00444;
      else
        pdir->attribute|=00666;
      pdir++;
      num++;
      #ifdef LONGNAME_SUPPORT
        p=&(LongName[MAX_LONG_FILENAME_LEN]);
      #endif
    }

    if (fd->bits.is_root) {
      fd->FileOffset++;
      if (fd->FileOffset>=BootSect->BPB_RootEntCnt) {
        go_on=0;
        break;
      }
      de=&(root_dir[fd->FileOffset]);
    } else {
      fd->FileOffset+=sizeof(Dir_Entry);
      if (!(fd->FileOffset%BytesPerCluster)) {
        Struct_NextCluster(fd);
        if (FAT_EOF(fd->CurrentCluster)) {
          go_on=0;
          break;
        }
        GetCluster(fd->CurrentCluster,descr-1);
      }
      de=&(((Dir_Entry *)fd->Cluster)[(fd->FileOffset%BytesPerCluster)>>
                                       SizeOfDirEntrySH]);
    }

  }
#ifdef DEBUG
  printf("[* FAT:fs_read_dir] <END> num=%d\n",num);
#endif
  return num;
}


/*
 Returns the directory entry for the specified file/directory.
*/
FS_PUBLIC_DIR_ENTRY Get_Public_Entry(char *path) {
  Dir_Entry *direntry;
  FS_PUBLIC_DIR_ENTRY pdir;
  int i;

  pdir.attribute=0;
  if (LocateEntry(path,&direntry)) {
    *(pdir.filename)='\0';
    return pdir;
  }
  for (i=0;i<20;i++) pdir.filename[i]=' ';
  FATFormat2Readable(direntry->filename,pdir.filename);
  pdir.file_size=direntry->file_size;
  if (direntry->attributes&FAT_ATTR_DIRECTORY) pdir.attribute|=S_IFDIR;
  if (!(direntry->attributes&FAT_ATTR_ARCHIVE)) pdir.attribute|=00111;
  if (direntry->attributes&FAT_ATTR_READ_ONLY)
    pdir.attribute|=00444;
  else
    pdir.attribute|=00666;
  return pdir;
}



FS_PUBLIC_DIR_ENTRY Get_Public_Dir_Entry(char *dir, int idx) {
  Dir_Entry *direntry;
  FS_PUBLIC_DIR_ENTRY pdir;
  int i;

  pdir.attribute=0;
  if (!LocateNumEntry(dir,idx,&direntry)) {
    *(pdir.filename)='\0';
    return pdir;
  }
  for (i=0;i<20;i++) pdir.filename[i]=' ';
  FATFormat2Readable(direntry->filename,pdir.filename);
  pdir.file_size=direntry->file_size;
  if (direntry->attributes&FAT_ATTR_DIRECTORY) pdir.attribute|=S_IFDIR;
  if (!(direntry->attributes&FAT_ATTR_ARCHIVE)) pdir.attribute|=00111;
  if (direntry->attributes&FAT_ATTR_READ_ONLY)
    pdir.attribute|=00444;
  else
    pdir.attribute|=00666;
  return pdir;
}


void Get_Public_Dir_Entry_Mul(char *dir, int idx, int count,
                              FS_PUBLIC_DIR_ENTRY *direntries)
{
  Dir_Entry *direntry;
  FS_PUBLIC_DIR_ENTRY *pdir=direntries;
  int i,x;

  for (x=0; x<count; x++, pdir++) {
    pdir->attribute=0;
    for (i=0;i<20;i++) pdir->filename[i]=' ';
    if (LocateNumEntry(dir,idx+x,&direntry)) {
      FATFormat2Readable(direntry->filename,pdir->filename);
      pdir->file_size=direntry->file_size;
      if (direntry->attributes&FAT_ATTR_DIRECTORY) pdir->attribute|=S_IFDIR;
      if (!(direntry->attributes&FAT_ATTR_ARCHIVE)) pdir->attribute|=00111;
      if (direntry->attributes&FAT_ATTR_READ_ONLY)
        pdir->attribute|=00444;
      else
        pdir->attribute|=00666;
    }
  }
}


int Get_Public_Dir_Entry_size(char *Dir) {
  int i;

#ifdef DEBUG
  printf("[* FAT:Get_Public_Dir_Entry_size] dir='%s'\n",Dir);
#endif
  if (*Dir==DIR_SEP && *(Dir+1)=='\0') {
    for (i=0;i<BootSect->BPB_RootEntCnt;i++) {
      if (root_dir[i].filename[0]=='\0') return i;
    }
    return RootDirSectors<<BytesPerSectorSH;
  } else return get_file_size(Dir)>>SizeOfDirEntrySH;
}


dword vfs_pid=-1;

/* Main CODE -- Message interface */

int main(void) {
  //  SemaphoreAddress sem;
  //  char *t;
  char *params;
  //  union drvmps_msg *msg;
  MpsRetVal ret;

  MpsRetVal errcode;
  vfs_message * vmsg,* vans;
  msginfo info;


  Init();

#ifdef DYNAMIC_MPS_PID
  //printf("<*-*-*-*-* fat: MPS pid = %u *-*-*-*-*>\n",MPS_Pid);
#endif
  printf("[FAT] very very start\n");

  /* Start initialization. */
  if ((ret=DriverInit(&params))!=0) {
    printf("[FAT] ***** Error in initialization phase: %s!!!\n",
	   MpsStrerror(ret));
    exit();
  }

  errcode=GetDriverPid("system","vfs",&vfs_pid,MPS_WAITRUN);
  if (errcode || (vfs_pid==0)) {
    printf("[FAT] ***** Error in getting VFS pid: %s!!!\n",
	   MpsStrerror(errcode));
    exit();
  }

  //  sem=get_HW_Sem(1);

  strcpy(DiskDeviceName,"");  
  strcpy(MountPoint,"");
  {
    int x,y,z;
    for (x=1;x<1110;x++)
      for (y=1;y<999;y++) z=34/x*y+44/y;
  }

  /* Parameters processing. */
  //printf("Params(%d '%c') :\n",*params,*params);
  while (*params) {
    char *var;
    printf("[FAT] Variable : %s;",params);
    var=params;
    while (*params++) ;
    printf(" Value : %s\n",(*params)?params:"<DEFAULT>");
    if (strcasecmp(var,"device")==0) {
      strcpy(DiskDeviceName,params);
    } else if (strcasecmp(var,"mount")==0) {
      strcpy(MountPoint,params);
    }
    while (*params++) ;
  }

  /* DRIVER SPECIFIC INIT SEQUENCE */



  if (DiskDeviceName[0]=='\0') {
    printf("[FAT] : Mount device unspecified, exiting ...\n");
    Exit();
  } else {
      vmsg=getbmsg();
      vmsg->class=VFS_REQUEST_PARTITION;
      strcpy(vmsg->name,DiskDeviceName);
      send_msg(vfs_pid,vmsg,sizeof(*vmsg));
      vans=recv_msg(&info);
      if (vans->class==VFS_ERROR) {
	printf("[FAT] : VFS_ERROR, exiting ...\n");
	Exit();
      }
      DiskDevicePid=vans->pid;
      DiskPartTag=vans->tag;
      printf("[FAT] :  Got tag = %d\n",DiskPartTag);
  }

  if (MountPoint[0]=='\0') {
    printf("[FAT] : Mount Point unspecified, exiting ...\n");
    Exit();
  } else {
      vmsg=getbmsg();
      vmsg->class=VFS_PUBLISH_MOUNT_POINT;
      strcpy(vmsg->name,MountPoint);
      send_msg(vfs_pid,vmsg,sizeof(*vmsg));
      vans=recv_msg(&info);
      if (vans->class==VFS_ERROR) {
	printf("[FAT] : VFS_ERROR, exiting ...\n");
	Exit();
      }
  }

  /* This ends initialization phase. */
  if ((ret=DriverStart("fs","fat",DRV_I_MULTI,0,1,
		       "This driver (tries to) manage fat filesystems."))!=0)
    {
      printf("[FAT] ***** ERROR: MPS don't allow me to register: %s!!!\n",
	     MpsStrerror(ret));
      Exit();
    }

  {
    MpsRetVal errcode;
#ifdef DYNAMIC_MPS_PID
    //  printf("<***** fat: MPS pid = %d *****>\n",MPS_Pid);
#endif
    /*
      errcode=GetDriverPid("hd_device","ide",&ide_pid,MPS_WAITRUN);
      if (errcode || (ide_pid==0)) {
      printf("[FAT] ***** Error in getting IDE pid: %s!!!\n",
      MpsStrerror(errcode));
      Exit();
      }
    */
  }

  printf("[FAT] Started. (pid = %d) Device = %s; Mount Point = %s\n",getpid(),DiskDeviceName,MountPoint);

  FS_init(0);

  //-  get_FAT_parameters(0);

  printf("[FAT] Ok. I'm registered to MPS.\n");

  printf("[FAT] Entering loop...\n");
  while (1) {
    //    FAT_File_Name fn;
    //    char nome[]="astro/robot/a.d/ciao.net";
    //    int n,m;
    FS_COMMAND * messaggio;
    FS_COMMAND * risposta;

    messaggio=recv_msg(&info);
    risposta=getbmsg();
    *risposta=*messaggio;
    switch (messaggio->command) {
    case FS_CMD_OPEN:
      ((FS_OPEN_COMMAND*)risposta)->filedescriptor=
        fs_open(((FS_OPEN_COMMAND*)messaggio)->path);
      break;
    case FS_CMD_CLOSE:
      fs_close(((FS_CLOSE_COMMAND*)messaggio)->filedescriptor);
      break;
    case FS_CMD_READ:
      {
        FS_READ_COMMAND *msg;
        FS_READ_COMMAND *ans;

        msg=(FS_READ_COMMAND *)messaggio;
        ans=(FS_READ_COMMAND *)risposta;
        ans->Count=fs_read(msg->filedescriptor,msg->Offset,
                           msg->Count,ans->buffer);
        break;
      }
    case FS_CMD_LSEEK:
      ((FS_LSEEK_COMMAND*)risposta)->offset=
        fs_lseek(((FS_LSEEK_COMMAND*)messaggio)->filedescriptor,
                 ((FS_LSEEK_COMMAND*)messaggio)->offset,
                 ((FS_LSEEK_COMMAND*)messaggio)->whence);
      break;
    case FS_CMD_OPEN_DIR:
      ((FS_OPEN_DIR_COMMAND*)risposta)->filedescriptor=
        fs_open_dir(((FS_OPEN_DIR_COMMAND*)messaggio)->path);
      break;
    case FS_CMD_CLOSE_DIR:
      fs_close_dir(((FS_CLOSE_DIR_COMMAND*)messaggio)->filedescriptor);
      break;
    case FS_CMD_READ_DIR:
      {
        FS_READ_DIR_COMMAND *msg;
        FS_READ_DIR_COMMAND *ans;

        msg=(FS_READ_DIR_COMMAND *)messaggio;
        ans=(FS_READ_DIR_COMMAND *)risposta;
        ans->num_entries=fs_read_dir(msg->filedescriptor,
                                     msg->num_entries, ans->entries);
      }
      break;
    case FS_CMD_GET_DIR_ENTRY_SIZE:
      ((FS_GET_DIR_ENTRY_SIZE_COMMAND*)risposta)->DIR_ENTRIES=
        Get_Public_Dir_Entry_size(
				  ((FS_GET_DIR_ENTRY_SIZE_COMMAND*)messaggio)->path);
      break;
    case FS_CMD_GET_DIR_ENTRY:
      ((FS_GET_DIR_ENTRY_COMMAND*)risposta)->Dir_Entry=
        Get_Public_Dir_Entry(
			     ((FS_GET_DIR_ENTRY_COMMAND*)messaggio)->path,
			     ((FS_GET_DIR_ENTRY_COMMAND*)messaggio)->Dir_Entry_Idx);
      break;
    case FS_CMD_GET_ENTRY:
      ((FS_GET_ENTRY_COMMAND*)risposta)->Dir_Entry=
        Get_Public_Entry(
			 ((FS_GET_ENTRY_COMMAND*)messaggio)->path);
      break;
    case FS_CMD_GET_DIR_ENTRY_MUL:
      Get_Public_Dir_Entry_Mul(
			       ((FS_GET_DIR_ENTRY_MUL_COMMAND*)messaggio)->path,
			       ((FS_GET_DIR_ENTRY_MUL_COMMAND*)messaggio)->Dir_Entry_Idx,
			       ((FS_GET_DIR_ENTRY_MUL_COMMAND*)messaggio)->Dir_Entry_Count,
			       ((FS_GET_DIR_ENTRY_MUL_COMMAND*)risposta)->Dir_Entry);
      break;
    case FS_CMD_CLEAR_CACHE:
      {
       	FS_CLEAR_CACHE_COMMAND * ans;

        ans=(FS_CLEAR_CACHE_COMMAND *)risposta;
        FS_init(0);
	//        get_FAT_parameters(0);
        break;
      }
    default:
      printf("[FAT] MESSAGE unknown\n");
      while (1) ;
      break;
    }

    send_msg(info.mittente,risposta,4090);
    //int_root_dir(0);
    freemsg(messaggio);
    /* Process message and answer */

  }
  DriverExit();
  return 0;
}
