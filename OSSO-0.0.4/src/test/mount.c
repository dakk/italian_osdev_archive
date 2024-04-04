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


#include <printf.h>
#include <processes.h>
#include <memory.h>

#include <messages.h>
#include <string.h>
#include <protocols/mpsint.h>
#include <protocols/drv2mps.h>
#include <protocols/vfs.h>
#include <drivers/protocols/mpsint.h>
#include <drivers/protocols/prg2mps.h>

/* Needed uppercase to differentiate from the one in lib/fs.c */
//static dword Vfs_pid=-1;


int main (void) {
  MpsRetVal errcode;
  vfs_message * msg, * ans;
  msginfo info;
  int vfs_pid=-1;

  errcode=GetDriverPid("system","vfs",&vfs_pid,MPS_WAITRUN);
  if (errcode) {
    printf("[mount] ***** Error in getting vfs data: %s!!!\n",
	   MpsStrerror(errcode));
    exit();
  }
  
  if (vfs_pid<=0) {
    printf("ERROR : VFS not found\n");
    exit();
  }

  printf("Enumerating Partitions ...\n");
  msg=getbmsg();
  msg->class=VFS_ENUMERATE_PARTITION;
  msg->idx=0;
  printf("Sending message to VFS (%d)\n",vfs_pid);
  send_msg(vfs_pid,msg,sizeof(*msg));
  ans=recv_msg(&info);
  if (ans->class!=VFS_SUCCESS) printf("VFS Returned an error.\n"); else
  while ((ans->class==VFS_SUCCESS) && (ans->idx!=-1)) {
    printf("Partition found : %s driver %d tag %d\n",ans->name,ans->pid,ans->tag);
    msg=getbmsg();
    *msg=*ans;
    msg->class=VFS_ENUMERATE_PARTITION;
    freemsg(ans);
    send_msg(vfs_pid,msg,sizeof(*msg));
    ans=recv_msg(&info);
  }
  

  printf("Enumerating Mount Points ...\n");
  msg=getbmsg();
  msg->class=VFS_ENUMERATE_MOUNT_POINT;
  msg->idx=0;
  printf("Sending message to VFS (%d)\n",vfs_pid);
  send_msg(vfs_pid,msg,sizeof(*msg));
  ans=recv_msg(&info);
  if (ans->class!=VFS_SUCCESS) printf("VFS Returned an error.\n"); else
  while ((ans->class==VFS_SUCCESS) && (ans->idx!=-1)) {
    printf("Mount Point found : %s driver %d\n",ans->name,ans->pid);
    msg=getbmsg();
    *msg=*ans;
    msg->class=VFS_ENUMERATE_MOUNT_POINT;
    freemsg(ans);
    send_msg(vfs_pid,msg,sizeof(*msg));
    ans=recv_msg(&info);
  }

}





