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
 * file : $Source: /cvsroot/osso/OSSO/src/lib/fs.c,v $
 * First attempt of a VERY basic fs approach
 ***************************************************************************

 ***************************************************************************
 * $Id: fs.c,v 1.18 2001/08/23 19:21:11 iurlano Exp $
 ***************************************************************************
 *
 * $Log: fs.c,v $
 * Revision 1.18  2001/08/23 19:21:11  iurlano
 * Changed IDE to publish correctly each partition, whole disk included.
 * Added mount point support to vfs. Extended its protocol.
 * Changed fat driver to accept two mandatory parameters: device (the partitions with the fs) and mount (the mount point)
 * Now it is possible to mount a partition and actually see it int the FS!!
 * Works!
 * Changed fs.c in libOSSO to use the vfs for dir and file access.
 * Changed mount utility to inspect partitions and mount points state.
 * Added Documentation for FAT driver
 *
 * Revision 1.17  2001/07/08 11:12:51  steros
 * Converted MPS to dynamic allocation and other little fix.
 *
 * Revision 1.16  2001/06/26 21:21:21  iurlano
 * Started to implement vfs support in lib/fs.c
 *
 * Revision 1.15  2001/06/24 22:18:12  iurlano
 * Optimized process creation functions
 * Added programs to the test section in xconfig
 * Added some functions to kernel-loader.c
 * Page faults now terminate the running process
 * Bugfixed memory leak in program execution from the shell
 *
 * Revision 1.14  2001/06/16 11:35:21  steros
 * Added dir scanning functions. Fixed various bugs.
 *
 * Revision 1.13  2001/06/13 21:04:48  steros
 * Various fixes.
 *
 * Revision 1.12  2001/06/13 16:45:28  steros
 * FAT/SHELL Restructuration, bug fixes and flush function to avoid line
 * buffering.
 *
 * Revision 1.11  2001/06/12 11:02:26  iurlano
 * Added interface fucntion to fat.c fs.c and others (not working)
 *
 * Revision 1.10  2001/06/10 17:17:47  steros
 * Working on fat basic functions.
 *
 * Revision 1.9  2001/05/30 21:25:05  steros
 * Corrected serial input as substitute of keyboard: now it work!
 * And other little adjustment...
 *
 * Revision 1.8  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 * Revision 1.7  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.6  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.5  2001/04/26 22:09:09  iurlano
 * fat work (again)
 * working snapshot ?
 *
 * Revision 1.4  2001/04/23 22:16:13  iurlano
 * fat work
 *
 * Revision 1.3  2001/04/23 15:33:19  iurlano
 * work on FAT and fs protocol
 *
 * Revision 1.2  2001/04/23 14:43:05  iurlano
 * work on FAT module
 * work on FS protocol
 *
 * Revision 1.1  2001/04/22 20:04:29  iurlano
 * added fs management module in libOSSO
 * work on fs protocol
 *
 *
 ***************************************************************************/


#include <messages.h>

#include <printf.h>

#include <drivers/protocols/mpsint.h>
#include <drivers/protocols/prg2mps.h>
#include <drivers/protocols/filesys.h>
#include <drivers/protocols/vfs.h>

//typedef unsigned long int dword;


#ifdef DYNAMIC_MPS_PID
  extern unsigned long MPS_Pid;
#endif

dword vfs_pid=-1;
static dword fat_pid=-1;

#define MAX_OPENED_FILES 100

typedef struct __FileDes {
  int filedescriptor; /* FS driver local file descriptor */
  unsigned long int fs_pid; /* FS driver pid */
  int active; /* Valid Entry */
} FileDes;

FileDes FileDescriptors[MAX_OPENED_FILES];


void Find_VFS_Pid(void) {
  MpsRetVal errcode;

#ifdef DYNAMIC_MPS_PID
//  printf("<***** fs: MPS pid = %d *****>\n",MPS_Pid);
#endif
  errcode=GetDriverPid("system","vfs",&vfs_pid,MPS_WAITRUN);
  if (errcode) {
    printf("[fs.c] ***** Error in getting vfs data: %s!!!\n",
	   MpsStrerror(errcode));
    exit();
  }
  if (vfs_pid==0) {
    printf("ERROR : VFS not found\n");
    while (1);
  }
}


void Find_Fat_Pid(void) {
  MpsRetVal errcode;

#ifdef DYNAMIC_MPS_PID
//  printf("<***** fs: MPS pid = %d *****>\n",MPS_Pid);
#endif
  errcode=GetDriverPid("fs","fat",&fat_pid,MPS_WAITRUN);
  if (errcode) {
    printf("[fs.c] ***** Error in getting data: %s!!!\n",
	   MpsStrerror(errcode));
    exit();
  }
  if (fat_pid==0) {
    printf("ERROR FAT fs not found\n");
    while (1);
  }
}

void Init_Fs_Data(void) {
  int i;

  /*
    vfs_pid=0;
    fat_pid=0;
  */
  for (i=0;i<MAX_OPENED_FILES;i++)
    FileDescriptors[i].active=0;

  FileDescriptors[0].active=1;
  FileDescriptors[1].active=1;
  FileDescriptors[2].active=1;

  /* Cannot call this functions here.
     Don't know why
  */

  //Find_VFS_Pid();
  //Find_Fat_Pid();

}


int Open_File(char *path) {
  FS_OPEN_COMMAND *msg, *ans;
  int c;
  msginfo mi;

  vfs_message * vmsg, * vans;
  int fs_pid;

  for (c=0;(c<MAX_OPENED_FILES)&&(FileDescriptors[c].active!=0);c++) ;
  if (c==MAX_OPENED_FILES) return -1;

  /* Need Mutual exclusion at process level!!! */
  FileDescriptors[c].active=1;
  if (fat_pid==-1) {
    Find_Fat_Pid();
    Find_VFS_Pid();
  }

  vmsg=getbmsg();
  vmsg->class=VFS_GET_FS_PID;
  strcpy(vmsg->name,path);
  send_msg(vfs_pid,vmsg,1);
  vans=recv_msg(&mi);
  if (vans->class==VFS_ERROR) printf("Vfs returned an error in checking the fs driver\n");
  else {
    printf("Vfs tells that fs driver is %d and path is %s\n",
	   vans->pid,vans->name);
    fs_pid=vans->pid;
    path=vans->name;
  }
  

  msg=getbmsg();
  strcpy(msg->path,path);
  msg->command=FS_CMD_OPEN;
  send_msg(fs_pid,msg,1);
  ans=recv_msg(&mi);
  FileDescriptors[c].filedescriptor=ans->filedescriptor;
  FileDescriptors[c].fs_pid=fs_pid;

  freemsg(ans);
  return c;
}


int Close_File(int descriptor) {
  FS_CLOSE_COMMAND *msg, *ans;

  msginfo mi;

  if (fat_pid==-1)  {
    Find_Fat_Pid();
    Find_VFS_Pid();
  } 
  if (FileDescriptors[descriptor].active!=1) return -1;
  msg=getbmsg();
  msg->filedescriptor=FileDescriptors[descriptor].filedescriptor;
  msg->command=FS_CMD_CLOSE;
  send_msg(FileDescriptors[descriptor].fs_pid,msg,1);
  ans=recv_msg(&mi);
  freemsg(ans);
  FileDescriptors[descriptor].active=0;
  return 0;
}


int Read_File(int descriptor, dword offset, dword count, byte *buffer) {
  FS_READ_COMMAND *msg, *ans;
  msginfo mi;
  int byte_read=0;
  int to_do, cur_offset, dest_idx;

  to_do=count;
  cur_offset=offset;
  dest_idx=0;

  //if (fat_pid==-1) Find_Fat_Pid();

  if (FileDescriptors[descriptor].active!=1) return -1;
  while (to_do>MAX_BUFFER_SIZE) {
    msg=getbmsg();
    msg->filedescriptor=FileDescriptors[descriptor].filedescriptor;
    msg->command=FS_CMD_READ;
    msg->Offset=cur_offset;
    msg->Count=MAX_BUFFER_SIZE;
    send_msg(FileDescriptors[descriptor].fs_pid,msg,sizeof(*msg));
    ans=recv_msg(&mi);
    cur_offset+=ans->Count;
    byte_read+=ans->Count;
    memcpy(buffer+dest_idx,ans->buffer,ans->Count);
    dest_idx+=ans->Count;
    to_do-=ans->Count;
    if (ans->Count==0) printf("ERROR : Count=0\n");
    freemsg(ans);
  }
  if (to_do>0) {
    msg=getbmsg();
    msg->filedescriptor=FileDescriptors[descriptor].filedescriptor;
    msg->command=FS_CMD_READ;
    msg->Offset=cur_offset;
    msg->Count=to_do;
    send_msg(FileDescriptors[descriptor].fs_pid,msg,sizeof(*msg));
    ans=recv_msg(&mi);
    byte_read+=ans->Count;
    memcpy(buffer+dest_idx,ans->buffer,ans->Count);
    freemsg(ans);
  }
  return byte_read;
}


dword Lseek_File(int descriptor, long offset, int whence) {
  FS_LSEEK_COMMAND *msg, *ans;
  dword newoffset;

  msginfo mi;

  //if (fat_pid==-1) Find_Fat_Pid();
  if (FileDescriptors[descriptor].active!=1) return -1;

  msg=getbmsg();
  msg->filedescriptor=FileDescriptors[descriptor].filedescriptor;
  msg->offset=offset;
  msg->whence=whence;
  msg->command=FS_CMD_LSEEK;
  send_msg(FileDescriptors[descriptor].fs_pid,msg,1);
  ans=recv_msg(&mi);
  newoffset=ans->offset;
  freemsg(ans);
  return newoffset;
}


int Open_Dir(char *path) {
  FS_OPEN_DIR_COMMAND *msg, *ans;
  int c;

  msginfo mi;
  vfs_message * vmsg, * vans;
  int fs_pid;
  char * p;

  if (fat_pid==-1)  {
    Find_Fat_Pid();
    Find_VFS_Pid();
  }

  p=path;
  if (*p) {
    for (p=path;*(p+1);p++) ;
    if (*p!='/')
      strcpy(p+1,"/");
  }

  printf("Open dir with '%s'\n",path);
  vmsg=getbmsg();
  vmsg->class=VFS_GET_FS_PID;
  strcpy(vmsg->name,path);
  send_msg(vfs_pid,vmsg,1);
  vans=recv_msg(&mi);
  if (vans->class==VFS_ERROR) printf("Vfs returned an error in checking the fs driver\n");
  else {
    printf("Vfs tells that fs driver is %d and path is '%s'\n",
	   vans->pid,vans->name);
    fs_pid=vans->pid;
    path=vans->name;
  }
  

  //printf("checking free fdes\n");
  for (c=0;(c<MAX_OPENED_FILES)&&(FileDescriptors[c].active!=0);c++) ;
  if (c==MAX_OPENED_FILES) return -1;
  //printf("free fdes found %d \n",c);
  /* Need Mutual exclusion at process level!!! */
  FileDescriptors[c].active=1;

  msg=getbmsg();
  strcpy(msg->path,path);
  msg->command=FS_CMD_OPEN_DIR;
  //printf("Sending msg fat_pid=%d\n",fat_pid);
  send_msg(fs_pid,msg,1);
  ans=recv_msg(&mi);
  //printf("Msg received\n");
  FileDescriptors[c].filedescriptor=ans->filedescriptor;
  FileDescriptors[c].fs_pid=fs_pid;

  freemsg(ans);
  freemsg(vans);
  return c;
}


int Close_Dir(int descriptor) {
  FS_CLOSE_DIR_COMMAND *msg, *ans;

  msginfo mi;


  if (fat_pid==-1)  {
    Find_Fat_Pid();
    Find_VFS_Pid();
  }
  if (FileDescriptors[descriptor].active!=1) return -1;

  msg=getbmsg();
  msg->filedescriptor=FileDescriptors[descriptor].filedescriptor;
  msg->command=FS_CMD_CLOSE_DIR;
  send_msg(FileDescriptors[descriptor].fs_pid,msg,1);
  ans=recv_msg(&mi);
  freemsg(ans);

  FileDescriptors[descriptor].active=0;
  return 0;
}


int Read_Dir(int descriptor, int count, FS_PUBLIC_DIR_ENTRY *DirEntries) {
  FS_READ_DIR_COMMAND * msg;
  FS_READ_DIR_COMMAND * ans_msg;
  int i;
  msginfo mi;

  if (fat_pid==-1)  {
    Find_Fat_Pid();
    Find_VFS_Pid();
  } 
  if (FileDescriptors[descriptor].active!=1) return -1;

  msg=getbmsg();
  msg->filedescriptor=FileDescriptors[descriptor].filedescriptor;
  msg->command=FS_CMD_READ_DIR;
  msg->num_entries=count;
  send_msg(FileDescriptors[descriptor].fs_pid,msg,1);
  ans_msg=recv_msg(&mi);
  for (i=0;i<count;i++) {
    DirEntries[i]=ans_msg->entries[i];
  }
  freemsg(ans_msg);
  return ans_msg->num_entries;
}




FS_PUBLIC_DIR_ENTRY Get_Dir_Entry(char * path,int idx) {
  FS_GET_DIR_ENTRY_COMMAND * msg;
  FS_GET_DIR_ENTRY_COMMAND * ans_msg;
  FS_PUBLIC_DIR_ENTRY c;

  vfs_message * vmsg, * vans;
  msginfo mi;
  int fs_pid;

  if (fat_pid==-1)  {
    Find_Fat_Pid();
    Find_VFS_Pid();
  }

  vmsg=getbmsg();
  vmsg->class=VFS_GET_FS_PID;
  strcpy(vmsg->name,path);
  send_msg(vfs_pid,vmsg,1);
  vans=recv_msg(&mi);
  if (vans->class==VFS_ERROR) printf("Vfs returned an error in checking the fs driver\n");
  else {
    printf("Vfs tells that fs driver is %d and path is %s\n",
	   vans->pid,vans->name);
    fs_pid=vans->pid;
    path=vans->name;
  }
  
  
  msg=getbmsg();
  strcpy(msg->path,path);
  msg->command=FS_CMD_GET_DIR_ENTRY;
  msg->Dir_Entry_Idx=idx;
  send_msg(fs_pid,msg,1);
  ans_msg=recv_msg(&mi);
  c=ans_msg->Dir_Entry;
  freemsg(ans_msg);
  freemsg(vans);
  return c;
}


FS_PUBLIC_DIR_ENTRY Get_Entry(char * path) {
  FS_GET_ENTRY_COMMAND * msg;
  FS_GET_ENTRY_COMMAND * ans_msg;
  FS_PUBLIC_DIR_ENTRY c;

  vfs_message * vmsg, * vans;
  msginfo mi;
  int fs_pid;

  if (fat_pid==-1)  {
    Find_Fat_Pid();
    Find_VFS_Pid();
  }

  vmsg=getbmsg();
  vmsg->class=VFS_GET_FS_PID;
  strcpy(vmsg->name,path);
  send_msg(vfs_pid,vmsg,1);
  vans=recv_msg(&mi);
  if (vans->class==VFS_ERROR) printf("Vfs returned an error in checking the fs driver\n");
  else {
    printf("Vfs tells that fs driver is %d and path is %s\n",
	   vans->pid,vans->name);
    fs_pid=vans->pid;
    path=vans->name;
  }
  

  msg=getbmsg();
  strcpy(msg->path,path);
  msg->command=FS_CMD_GET_ENTRY;
  send_msg(fs_pid,msg,1);
  ans_msg=recv_msg(&mi);
  c=ans_msg->Dir_Entry;
  freemsg(ans_msg);
  freemsg(vans);
  return c;
}


void Get_Dir_Entry_Mul(char * path,int idx,int count,FS_PUBLIC_DIR_ENTRY * DirEntries) {
  FS_GET_DIR_ENTRY_MUL_COMMAND * msg;
  FS_GET_DIR_ENTRY_MUL_COMMAND * ans_msg;
  int i;
  msginfo mi;
  vfs_message * vmsg, * vans;
  int fs_pid;

  if (fat_pid==-1)  {
    Find_Fat_Pid();
    Find_VFS_Pid();
  }
 
  vmsg=getbmsg();
  vmsg->class=VFS_GET_FS_PID;
  strcpy(vmsg->name,path);
  send_msg(vfs_pid,vmsg,1);
  vans=recv_msg(&mi);
  if (vans->class==VFS_ERROR) printf("Vfs returned an error in checking the fs driver\n");
  else {
    printf("Vfs tells that fs driver is %d and path is %s\n",
	   vans->pid,vans->name);
    fs_pid=vans->pid;
    path=vans->name;
  }
  


  msg=getbmsg();
  strcpy(msg->path,path);
  msg->command=FS_CMD_GET_DIR_ENTRY_MUL;
  msg->Dir_Entry_Idx=idx;
  msg->Dir_Entry_Count=count;
  send_msg(fs_pid,msg,1);
  ans_msg=recv_msg(&mi);
  for (i=0;i<count;i++) {
    DirEntries[i]=ans_msg->Dir_Entry[i];
  }
  freemsg(ans_msg);
  freemsg(vans);
}


void FS_Clear_Cache(void) {
  FS_CLEAR_CACHE_COMMAND * msg;
  FS_CLEAR_CACHE_COMMAND * ans;
  msginfo mi;

  if (fat_pid==-1)  {
    Find_Fat_Pid();
    Find_VFS_Pid();
  } 

  msg=getbmsg();
  msg->command=FS_CMD_CLEAR_CACHE;
  send_msg(fat_pid,msg,sizeof(*msg));
  ans=recv_msg(&mi);
  freemsg(ans);
}

dword Get_Dir_Entry_Number(char * path) {
  FS_GET_DIR_ENTRY_SIZE_COMMAND * msg;
  FS_GET_DIR_ENTRY_SIZE_COMMAND * ans_msg;
  dword c;

  vfs_message * vmsg, * vans;
  int fs_pid;
  msginfo mi;

  if (fat_pid==-1)  {
    Find_Fat_Pid();
    Find_VFS_Pid();
  }

  vmsg=getbmsg();
  vmsg->class=VFS_GET_FS_PID;
  strcpy(vmsg->name,path);
  send_msg(vfs_pid,vmsg,1);
  vans=recv_msg(&mi);
  if (vans->class==VFS_ERROR) printf("Vfs returned an error in checking the fs driver\n");
  else {
    printf("Vfs tells that fs driver is %d and path is %s\n",
	   vans->pid,vans->name);
    fs_pid=vans->pid;
    path=vans->name;
  }
  

  msg=getbmsg();
  strcpy(msg->path,path);
  msg->command=FS_CMD_GET_DIR_ENTRY_SIZE;
  send_msg(fs_pid,msg,1);
  ans_msg=recv_msg(&mi);
  c=ans_msg->DIR_ENTRIES;
  freemsg(ans_msg);
  freemsg(vans);
  return c;
}

