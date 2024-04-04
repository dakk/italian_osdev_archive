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


/********************** Driver's protocols O S S O *************************
 * file : drivers/lib/include/protocols/disk.h                             *
 * File System driver protocol                                             *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: filesys.h,v 1.8 2001/06/29 17:45:52 steros Exp $
 ***************************************************************************
 *
 * $Log: filesys.h,v $
 * Revision 1.8  2001/06/29 17:45:52  steros
 * Added long file name for fat. Not complete!
 *
 * Revision 1.7  2001/06/16 11:35:21  steros
 * Added dir scanning functions. Fixed various bugs.
 *
 * Revision 1.6  2001/06/13 16:45:28  steros
 * FAT/SHELL Restructuration, bug fixes and flush function to avoid line
 * buffering.
 *
 * Revision 1.5  2001/06/12 11:02:26  iurlano
 * Added interface fucntion to fat.c fs.c and others (not working)
 *
 * Revision 1.4  2001/06/10 17:17:47  steros
 * Working on fat basic functions.
 *
 * Revision 1.3  2001/05/01 01:04:19  iurlano
 * Work on Dynamic Linker problem
 * Seems like that the shared MPS_Pid is working now.
 * I thing that the condivision of a variable from the lib and the program works if the variable is declared in the program.
 * for the other case, I've to work on it and I don't think I'm going to do it very soon.
 *
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.6  2001/04/26 22:09:09  iurlano
 * fat work (again)
 * working snapshot ?
 *
 * Revision 1.5  2001/04/23 22:16:13  iurlano
 * fat work
 *
 * Revision 1.4  2001/04/23 15:33:19  iurlano
 * work on FAT and fs protocol
 *
 * Revision 1.3  2001/04/23 14:43:05  iurlano
 * work on FAT module
 * work on FS protocol
 *
 * Revision 1.2  2001/04/22 20:04:29  iurlano
 * added fs management module in libOSSO
 * work on fs protocol
 *
 * Revision 1.1  2001/04/20 15:30:59  iurlano
 * Changes in to the fat driver
 * creating the filesystem protocol
 *
 *
 ***************************************************************************/

#ifndef __FILE_SYS_PROTOCOL
#define __FILE_SYS_PROTOCOL

#include <types.h>

/* COMMANDS */

#define FS_CMD_OPEN                   1  /* Open a file.               */
#define FS_CMD_CLOSE                  2  /* Close a file.              */
#define FS_CMD_READ                   3  /* Reads x bytes from a file. */
#define FS_CMD_LSEEK                  4  /* Reposition file pointer.   */

#define FS_CMD_OPEN_DIR               5  /* Open a dir.                */
#define FS_CMD_CLOSE_DIR              6  /* Close a dir.               */
#define FS_CMD_READ_DIR               7  /* Reads x entries froma dir. */

#define FS_CMD_GET_ENTRY              8 /* Get a public_dir_entry of a file */
#define FS_CMD_GET_DIR_ENTRY_SIZE     10 /* Get number of dir_entries of a dir */
#define FS_CMD_GET_DIR_ENTRY          11 /* Get a dir_entry of a dir */
#define FS_CMD_GET_DIR_ENTRY_MUL      12 /* Get a block of dir_entries of a dir */

#define FS_CMD_CLEAR_CACHE         0x80

/* Commands not yet implemented */
#define FS_CMD_WRITEFILE              7 /* Writes x bytes from a file starting from y */
#define FS_CMD_UNLINKFILE             8 /* Deletes a file */


/* Constants */

#define MAX_ACCEPTABLE_PATH_LENGTH  260
#define MAX_FILE_NAME_LENGTH        255
#define MAX_BUFFER_SIZE            3400 /* For actual messages limit */

/*  FS_CMD_GET_DIR_ENTRY_SIZE
    Returns dimension of dir entries in arg1
    needs PATH as char[] after command structure
*/


/*  FS_CMD_GET_DIR_ENTRY
    Returns the nth dir_entry
    needs PATH as char[] after command structure
    arg1 is the number of the dir_entry required
*/

#define S_IFMT     0170000  // bitmask for the file type bitfields
#define S_IFSOCK   0140000  // socket
#define S_IFLNK    0120000  // symbolic link
#define S_IFREG    0100000  // regular file
#define S_IFBLK    0060000  // block device
#define S_IFDIR    0040000  // directory
#define S_IFCHR    0020000  // character device
#define S_IFIFO    0010000  // fifo
#define S_ISUID    0004000  // set UID bit
#define S_ISGID    0002000  // set GID bit (see below)
#define S_ISVTX    0001000  // sticky bit (see below)
#define S_IRWXU    00700    // mask for file owner permissions
#define S_IRUSR    00400    // owner has read permission
#define S_IWUSR    00200    // owner has write permission
#define S_IXUSR    00100    // owner has execute permission
#define S_IRWXG    00070    // mask for group permissions
#define S_IRGRP    00040    // group has read permission
#define S_IWGRP    00020    // group has write permission
#define S_IXGRP    00010    // group has execute permission
#define S_IRWXO    00007    // mask for permissions for others (not in group)
#define S_IROTH    00004    // others have read permission
#define S_IWOTH    00002    // others have write permisson
#define S_IXOTH    00001    // others have execute permission

#define S_ISTYPE(mode, mask)  (((mode) & S_IFMT) == (mask))

#define S_ISDIR(mode)    S_ISTYPE((mode), S_IFDIR)
#define S_ISCHR(mode)    S_ISTYPE((mode), S_IFCHR)
#define S_ISBLK(mode)    S_ISTYPE((mode), S_IFBLK)
#define S_ISREG(mode)    S_ISTYPE((mode), S_IFREG)



#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2



struct stat {
  dword st_size;
  dword st_mode;
};

typedef struct __FS_PUBLIC_DIR_ENTRY {
  char filename[MAX_FILE_NAME_LENGTH+1];
  dword attribute;
  dword creation_time;
  dword modification_time;
  dword file_size;
} FS_PUBLIC_DIR_ENTRY;


/* Basic file functions. */

typedef struct __FS_OPEN_COMMAND {
  byte command;
  char path[MAX_ACCEPTABLE_PATH_LENGTH+1];
  int  filedescriptor;
} FS_OPEN_COMMAND;


typedef struct __FS_CLOSE_COMMAND {
  byte command;
  int  filedescriptor;
} FS_CLOSE_COMMAND;


typedef struct __FS_READ_COMMAND {
  byte  command;
  dword Offset,Count;
  int   filedescriptor;
  byte  buffer[MAX_BUFFER_SIZE];
} FS_READ_COMMAND;


typedef struct __FS_LSEEK_COMMAND {
  byte command;
  int  filedescriptor;
  long offset;
  byte whence;
} FS_LSEEK_COMMAND;



/* Directory scanning functions. */

typedef struct __FS_OPEN_DIR_COMMAND {
  byte command;
  char path[MAX_ACCEPTABLE_PATH_LENGTH+1];
  int  filedescriptor;
} FS_OPEN_DIR_COMMAND;


typedef struct __FS_CLOSE_DIR_COMMAND {
  byte command;
  int  filedescriptor;
} FS_CLOSE_DIR_COMMAND;


typedef struct __FS_READ_DIR_COMMAND {
  byte  command;
  int   filedescriptor;
  dword num_entries;
  FS_PUBLIC_DIR_ENTRY entries[50];
} FS_READ_DIR_COMMAND;





typedef struct __FS_GET_ENTRY_COMMAND {
  byte command;
  char path[MAX_ACCEPTABLE_PATH_LENGTH+1];
  FS_PUBLIC_DIR_ENTRY Dir_Entry;
} FS_GET_ENTRY_COMMAND;


typedef struct __FS_GET_DIR_ENTRY_SIZE_COMMAND {
  byte command;
  dword DIR_ENTRIES; /* Store the number of dir entries in the directory specified with PATH */
  char path[MAX_ACCEPTABLE_PATH_LENGTH+1];
} FS_GET_DIR_ENTRY_SIZE_COMMAND;

typedef struct __FS_GET_DIR_ENTRY_COMMAND {
  byte command;
  dword Dir_Entry_Idx;
  char path[MAX_ACCEPTABLE_PATH_LENGTH+1];
  FS_PUBLIC_DIR_ENTRY Dir_Entry;
} FS_GET_DIR_ENTRY_COMMAND;

typedef struct __FS_GET_DIR_ENTRY_MUL_COMMAND {
  byte command;
  dword Dir_Entry_Idx;
  dword Dir_Entry_Count;
  char path[MAX_ACCEPTABLE_PATH_LENGTH+1];
  FS_PUBLIC_DIR_ENTRY Dir_Entry[50];
} FS_GET_DIR_ENTRY_MUL_COMMAND;

typedef struct __FS_CLEAR_CACHE_COMMAND {
  byte command;
} FS_CLEAR_CACHE_COMMAND;

typedef struct __FS_COMMAND {
  byte command;
  dword arg1,arg2; /* arg1 is the device number (currently 0 only) */
  /* arg2 is the absolute sector number from the start of disk */
  byte tag;
  /* Needed to identify a command if executed out of order */
  /* Currently tag is not used */
  /* it is byte for alignement matters */
  word buffer_dim; /* Buffer follows immediately */
  word next_cmd; /* 0 if last */
} FS_COMMAND;

#endif
