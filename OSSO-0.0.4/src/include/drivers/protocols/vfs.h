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
 * file : $Source: /cvsroot/osso/OSSO/src/include/drivers/protocols/vfs.h,v $
 * Description: Generic data for protocols of VFS.
 ***************************************************************************

 ***************************************************************************
 * $Id: vfs.h,v 1.2 2001/08/23 19:21:11 iurlano Exp $
 ***************************************************************************
 *
 * $Log: vfs.h,v $
 * Revision 1.2  2001/08/23 19:21:11  iurlano
 * Changed IDE to publish correctly each partition, whole disk included.
 * Added mount point support to vfs. Extended its protocol.
 * Changed fat driver to accept two mandatory parameters: device (the partitions with the fs) and mount (the mount point)
 * Now it is possible to mount a partition and actually see it int the FS!!
 * Works!
 * Changed fs.c in libOSSO to use the vfs for dir and file access.
 * Changed mount utility to inspect partitions and mount points state.
 * Added Documentation for FAT driver
 *
 * Revision 1.1  2001/07/01 15:17:32  iurlano
 * Added first prototype of partition management system
 * Added mount program
 *
 *
 ***************************************************************************/

#ifndef _VFSINT_H_
#define _VFSINT_H_

#define MAX_PARTITION_NAME    20
#define MAX_MOUNT_PATH       200

#define VFS_PUBLISH_PARTITION      1
#define VFS_REQUEST_PARTITION      2
#define VFS_ENUMERATE_PARTITION    3

#define VFS_GET_FS_PID            10
#define VFS_PUBLISH_MOUNT_POINT   11
#define VFS_ENUMERATE_MOUNT_POINT 12


#define VFS_SUCCESS     0
#define VFS_ERROR      -1

typedef struct {
  int class;
  //char name[MAX_PARTITION_NAME];
  int tag;
  unsigned int pid;
  int idx;
  char name[MAX_MOUNT_PATH];
} vfs_message;

#endif
