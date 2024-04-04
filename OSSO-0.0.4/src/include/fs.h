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


/*************************** lib O S S O  ***********************************
 * file : $Source: /cvsroot/osso/OSSO/src/include/fs.h,v $
 * Header for file system handling functions
 ***************************************************************************

 ***************************************************************************
 * $Id: fs.h,v 1.6 2001/06/16 11:35:21 steros Exp $
 ***************************************************************************
 *
 * $Log: fs.h,v $
 * Revision 1.6  2001/06/16 11:35:21  steros
 * Added dir scanning functions. Fixed various bugs.
 *
 * Revision 1.5  2001/06/13 16:45:28  steros
 * FAT/SHELL Restructuration, bug fixes and flush function to avoid line
 * buffering.
 *
 * Revision 1.4  2001/06/12 11:02:26  iurlano
 * Added interface fucntion to fat.c fs.c and others (not working)
 *
 * Revision 1.3  2001/06/10 17:17:47  steros
 * Working on fat basic functions.
 *
 * Revision 1.2  2001/04/28 12:15:52  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.3  2001/04/26 22:09:09  iurlano
 * fat work (again)
 * working snapshot ?
 *
 * Revision 1.2  2001/04/23 22:16:13  iurlano
 * fat work
 *
 * Revision 1.1  2001/04/23 14:43:05  iurlano
 * work on FAT module
 * work on FS protocol
 *
 *
 ***************************************************************************/

#ifndef __FS_LIB
#define __FS_LIB

#include <drivers/protocols/filesys.h>

FS_PUBLIC_DIR_ENTRY Get_Dir_Entry(char * path,int idx);
FS_PUBLIC_DIR_ENTRY Get_Entry(char * path);

void Get_Dir_Entry_Mul(char * path,int idx,int count,FS_PUBLIC_DIR_ENTRY * DirEntries);

//int stat(const char *file_name, struct stat *buf);

dword Get_Dir_Entry_Number(char * path);

int Open_File(char *path);
int Close_File(int descriptor);
int Read_File(int descriptor, dword offset, dword count, byte *buffer);
dword Lseek_File(int descriptor, long offset, int whence);

int Open_Dir(char *path);
int Close_Dir(int descriptor);
int Read_Dir(int descriptor, int count, FS_PUBLIC_DIR_ENTRY *DirEntries);

void FS_Clear_Cache(void);

#endif
