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
 * file : drivers/lib/include/protocols/init.h
 * Protocol defining interactions between drivers and init
 ***************************************************************************

 ***************************************************************************
 * $Id: init.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: init.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.2  2000/08/25 13:39:38  iurlano
 * Added CHANGELOG
 * more translations
 *
 * Revision 1.1.1.1  2000/05/30 08:11:04  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#ifndef __INIT_PROTOCOL
#define __INIT_PROTOCOL

#define DEVICE_NAME_SIZE 32
#define PARTITION_NAME_SIZE 32

#define MAX_DEVICES 32
#define MAX_PARTITIONS 32

#include <types.h>
#include <processes.h>

#define INIT_INITIALIZE      1
/*
  From INIT to DRIVER.
  Message for driver initialization
  DATA field contains command line got from INITCONF
 */

#define INIT_PUBLISH_DEV     2
/*
  From DRIVER to INIT.
  Message for publishing a new device
  Arg1 contains device class
  Arg2 contains device id to be used in messages to DRIVER
  DATA contains device name and class dependent data.
 */

#define INIT_PUBLISH_PART    3
/*
  From DRIVER to INIT.
  Message for publishing a new partition
  
  Arg1 is partition's starting sector
  Arg2 is partition's ending sector
  Arg3 is a pointer to the place in the buffer that contains the name of the
    device to which the partition belongs
  DATA is partition's name
 */

#define INIT_FINISHED        4
/*
  From DRIVER to INIT.
  Message for notifying initialization's end
 */

#define INIT_GET_DEV_INFO    5
/*
  From DRIVER to INIT.
  Message for rquesting device info.
  DATA is device name.
 */

#define INIT_ANSW_DEV_INFO   6
/*
  From INIT to DRIVER.
  Answer message with infos on a device.
  Arg1 == 0 if no error. Error code otherwise.
  DATA is the entry of device table (DEVICE_ENTRY)
*/

#define INIT_ENUM_DEVICES   7
/*
  From DRIVER to INIT.
  Message requesting enumeration of all the devices of a class.
  Arg1 is the requested class. 0 means every class to be searched.
  Arg2 is the point from which continue on next call. Must be 0 for
    first call.
 */

#define INIT_ANSW_ENUM_DEV  8
/*
  From INIT to DRIVER.
  Answer message with infos on a device class.

  Arg3 is 0 if OK or error code if something has gone wrong
  Arg2 is the point from which continue on next call.
  Arg1 is the class requested
  DATA is device name.
 */

#define INIT_GET_PART_INFO   9
/*
  From DRIVER to INIT.
  Message requesting partition info.
  DATA is partition's name.
 */

#define INIT_ANSW_PART_INFO  10
/*
  From INIT to DRIVER.
  Answer message with partition's info.
  Arg1 is 0 if OK otherwise the error code.
  DATA contains partition's data (PARTITION_ENTRY).
 */

#define INIT_PUBLISH_FS      11
/*
  From DRIVER to INIT.
  Message for notifying INIT that DRIVER is the FS for a partition.
  DATA contains partition's name.
 */

#define INIT_ENUM_PARTITIONS 12
/*
  From DRIVER to INIT.
  Messsage for requesting partition list.
 */

#define INIT_ANSW_ENUM_PART  13
/*
  From DRIVER to INIT.
  Answer message with partition's list.
  Arg1 is 0 if OK or error code if something has gone wrong
  DATA contains partitions' names.
 */

typedef
struct __INIT_COMMAND {
  dword command;
  dword arg1;
  dword arg2;
  dword arg3;
  char data[512]; 
} INIT_COMMAND;

typedef
struct __DEVICE_ENTRY {
  char nome[DEVICE_NAME_SIZE];
  dword classe;
  PID processo;
  dword dispositivo;
} DEVICE_ENTRY;

typedef
struct __PARTITION_ENTRY {
  char nome[PARTITION_NAME_SIZE];
  char device[DEVICE_NAME_SIZE];
  dword start;
  dword end;
} PARTITION_ENTRY;

#endif
