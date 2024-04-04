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
 * file : $Source: /cvsroot/osso/OSSO/src/drivers/vfs.c,v $
 * Description: Virtual File System
 ***************************************************************************

 ***************************************************************************
 * $Id: vfs.c,v 1.6 2001/08/23 19:21:11 iurlano Exp $
 ***************************************************************************
 *
 * $Log: vfs.c,v $
 * Revision 1.6  2001/08/23 19:21:11  iurlano
 * Changed IDE to publish correctly each partition, whole disk included.
 * Added mount point support to vfs. Extended its protocol.
 * Changed fat driver to accept two mandatory parameters: device (the partitions with the fs) and mount (the mount point)
 * Now it is possible to mount a partition and actually see it int the FS!!
 * Works!
 * Changed fs.c in libOSSO to use the vfs for dir and file access.
 * Changed mount utility to inspect partitions and mount points state.
 * Added Documentation for FAT driver
 *
 * Revision 1.5  2001/08/22 14:19:26  iurlano
 * Added partition support to the FAT driver. Tested with hda3 and hda4 and seems working
 *
 * Revision 1.4  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.3  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.2  2001/07/01 15:17:32  iurlano
 * Added first prototype of partition management system
 * Added mount program
 *
 * Revision 1.1  2001/06/24 17:06:36  steros
 * Added vfs.
 *
 *
 ***************************************************************************/


#include <printf.h>
#include <messages.h>
#include <processes.h>
#include <string.h>
#include <protocols/mpsint.h>
#include <protocols/drv2mps.h>
#include <protocols/vfs.h>


#define MAX_PARTITIONS 20

typedef struct __Partition {
  char name[MAX_PARTITION_NAME];
  unsigned int pid;
  //  unsigned long long int sectors;
  int active;
  int tag;
} Partition;

Partition Partitions[MAX_PARTITIONS];

#define MAX_MOUNT_POINTS   20


typedef struct __Mount_Point {
  int active;
  unsigned int pid;
  char mount_path[MAX_MOUNT_PATH];
} Mount_Point;

Mount_Point Mount_Points[MAX_MOUNT_POINTS];

int Root_Index;

int main(void) {
  MpsRetVal ret;
  char *params;
  int i;
  vfs_message * msg;
  vfs_message * ans;
  msginfo info;
  

  printf("[VFS] Started with pid=%d\n",getpid());

  Root_Index=-1;
  /* Start initialization. */
  if ((ret=DriverInit(&params))!=0) {
    printf("[VFS] ***** Error in initialization phase: %s!!!\n",
	   MpsStrerror(ret));
    exit();
  }

  /* Parameters processing. */
  printf("Params(%d '%c') :\n",*params,*params);
  while (*params) {
    printf("[VFS] Variable : %s;",params);
    while (*params++) ;
    printf(" Value : %s\n",(*params)?params:"<DEFAULT>");
    while (*params++) ;
  }

  for (i=0;i<MAX_PARTITIONS;i++) Partitions[i].active=0;

  for (i=0;i<MAX_MOUNT_POINTS;i++) Mount_Points[i].active=0;
  
  /* This ends initialization phase. */
  if ((ret=DriverStart("system","vfs",DRV_I_MONO,0,1,
		       "This is the Virtual File System Manager."))!=0)
    {
      printf("[VFS] ***** ERROR: MPS don't allow me to register: %s!!!\n",
	     MpsStrerror(ret));
      exit();
    }

  printf("VFS Registered. Entering message loop\n");
  // Messages processing...
  while (1) {
    msg=recv_msg(&info);

    ans=getbmsg();
    *ans=*msg;

    switch (msg->class) {
    case VFS_PUBLISH_PARTITION:
      for (i=0;(i<MAX_PARTITIONS) && (Partitions[i].active!=0);i++) ;
      if (i==MAX_PARTITIONS) {
	printf("[VFS] No space for another partition\n");
	ans->class=VFS_ERROR;
	break;
      }
      Partitions[i].active=1;
      Partitions[i].tag=msg->tag;
      Partitions[i].pid=info.mittente;
      strcpy(Partitions[i].name,msg->name);
      ans->class=VFS_SUCCESS;
      printf("Partition '%s' registered from pid %d with tag %d\n",
	     msg->name,info.mittente,msg->tag);
      break;
    case VFS_REQUEST_PARTITION:
      printf("VFS Request for '%s'\n",msg->name);
      ans->class=VFS_ERROR;
      for (i=0;i<MAX_PARTITIONS;i++) {
	if ((Partitions[i].active==1) &&
	    (strcmp(Partitions[i].name,msg->name)==0)) {
	  ans->tag=Partitions[i].tag;
	  ans->pid=Partitions[i].pid;
	  /*
	    printf("Partition Check! '%s' '%s' tag=%d\n",
	    Partitions[i].name,msg->name,Partitions[i].tag);
	  */
	  ans->class=VFS_SUCCESS;
	  break;
	}
      }
      if (ans->class==VFS_ERROR)
	printf("VFS Request : Partition not found\n");

      break;
    case VFS_ENUMERATE_PARTITION:
      i=msg->idx;
      for (;(i<MAX_PARTITIONS) && (Partitions[i].active==0);i++) ;
      if (i==MAX_PARTITIONS) ans->idx=-1;
      else {
	*ans=*msg;
	ans->idx=i+1;
	strcpy(ans->name,Partitions[i].name);
	ans->tag=Partitions[i].tag;
	ans->pid=Partitions[i].pid;
	ans->class=VFS_SUCCESS;
      }
      break;
    case VFS_GET_FS_PID: 
      {
	char * p, *s;
	*ans=*msg;
	ans->class=VFS_ERROR;
	for (i=0;i<MAX_MOUNT_POINTS;i++) {
	  if ((i!=Root_Index)&&(Mount_Points[i].active==1)) {
	    p=&(msg->name);
	    s=&(Mount_Points[i].mount_path);
	    for (;(*p)&&(*s)&&(*p==*s);p++,s++) ;
	    if (*s=='\0') {
	      //if (*p==0) strcpy(p,"/");
	      if (*p=='/') {
		strcpy(ans->name,p);
		ans->pid=Mount_Points[i].pid;
		ans->class=VFS_SUCCESS;
		break;
	      }
	    }
	  }
	}
	if ((i==MAX_MOUNT_POINTS)&&(Root_Index!=-1)) {
	  ans->pid=Mount_Points[Root_Index].pid;
	  ans->class=VFS_SUCCESS;
	}
      }
      break;
    case VFS_PUBLISH_MOUNT_POINT:
      *ans=*msg;
      for (i=0;(i<MAX_MOUNT_POINTS)&&(Mount_Points[i].active!=0);i++) ;
      if (i==MAX_MOUNT_POINTS) {
	ans->class=VFS_ERROR;
      } else {
	strcpy(Mount_Points[i].mount_path,msg->name);
	Mount_Points[i].pid=info.mittente;
	Mount_Points[i].active=1;
	if (strcmp(msg->name,"/")==0) {
	  if (Root_Index==-1) Root_Index=i;
	  else printf("[VFS] : Root mount point already registered, not registering again.\n");
	}
	ans->class=VFS_SUCCESS;
      }
      break;
    case VFS_ENUMERATE_MOUNT_POINT:
      i=msg->idx;
      for (;(i<MAX_MOUNT_POINTS) && (Mount_Points[i].active==0);i++) ;
      if (i==MAX_MOUNT_POINTS) ans->idx=-1;
      else {
	*ans=*msg;
	ans->idx=i+1;
	strcpy(ans->name,Mount_Points[i].mount_path);
	ans->pid=Mount_Points[i].pid;
	ans->class=VFS_SUCCESS;
      }
      break;
    default:
      printf("[VFS] Error : invalid received message class :%d\n",msg->class);
      ans->class=VFS_ERROR;
      break;
    }
    send_msg(info.mittente,ans,sizeof(*ans));
    freemsg(msg);
  }
  DriverExit();
  return 0;
}

