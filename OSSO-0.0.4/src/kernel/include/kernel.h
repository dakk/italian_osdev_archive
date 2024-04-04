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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/kernel.h,v $
 * Descrizione:  definizione di quello che serve per le prove...
 ***************************************************************************/

/***************************************************************************/
/*
 * $Id: kernel.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 */
/***************************************************************************/
/*
 * $Log: kernel.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.1.1.1  2000/05/30 08:11:06  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:41  pecciant
 *
 *
 * Revision 1.4  1999/11/25 13:08:55  pecciant
 * Standardizzati gli header dei .h
 *
 * Revision 1.3  1999/10/21 15:38:40  iurlano
 * Aggiustamento commenti
 * Aggiunta header per la paginazione
 *
 */
/***************************************************************************/


#ifndef __KERNEL_DEFS
#define __KERNEL_DEFS

#include <types.h>

void get_IO_ports_address(void);

void print_partition_table(void);
void read_partition_table(byte * settore);

void get_FAT_parameters(int start_logical_sector);
void print_root_dir(int start_logical_sector);
int get_start_cluster(char * filename);
int get_file_size(char * filename);
int read_file(char * filename,char * memory);

void init_Process_Structures(void);

#endif
