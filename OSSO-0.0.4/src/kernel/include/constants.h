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
 * file : include/constants.h
 * Constants for OSSO
 ***************************************************************************


 ***************************************************************************
 * $Id: constants.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: constants.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.13  2001/04/26 22:09:09  iurlano
 * fat work (again)
 * working snapshot ?
 *
 * Revision 1.12  2001/04/26 19:25:54  iurlano
 * fat work
 * bugfixes
 *
 * Revision 1.11  2001/04/05 21:06:13  iurlano
 * Completed xconfig modifies. Now requires the tix package (Sorry but tcl/tk doesn't provide a NoteBook widget :-(( )
 *
 * Revision 1.10  2001/04/05 14:10:54  iurlano
 * xconfig changes
 *
 * Revision 1.9  2001/04/03 21:34:44  iurlano
 * Dynamic semaphores work in progress
 * Little changes in make xconfig
 * Added and changed in the code the value of THREAD_INVALID from 0 to -1
 *
 * Revision 1.8  2001/04/03 17:14:26  iurlano
 * work on dynamic semaphores
 *
 * Revision 1.7  2001/04/02 15:26:15  iurlano
 * Working version with only 40 max processes, due to memory conflicts
 *
 * Revision 1.6  2001/04/02 14:35:34  iurlano
 * Tuning memory areas
 *
 * Revision 1.5  2000/12/05 16:36:01  iurlano
 * Thread managementwork in progress
 *
 * Revision 1.4  2000/12/04 22:24:15  iurlano
 * Thread work just started
 *
 * Revision 1.3  2000/11/30 22:47:15  iurlano
 * Changed gdt handling.
 * Removed LDT for each task and added a global LDT for USER LEVEL tasks
 *
 * Revision 1.2  2000/08/20 16:46:06  iurlano
 * Fixed bug in init in modules loading
 * IDE driver test. Doesn't work with my Quantum Fireball 4.3 Gb but does
 *   work with my Conner 42MB
 * More translation
 * Fixed some bad comments
 * Changed paging.c for a test (see the comments there)
 *
 * Revision 1.1.1.1  2000/05/30 08:11:05  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#ifndef __CONST_DEFS
#define __CONST_DEFS

#include "../../Config_Kernel.h"

/* Process Constants */

#define PROCESS_STACK_SIZE         65532
#define PROCESS_LDT_ENTRIES           16

#define THREAD_USER_STACKS_SIZE 8000
#define THREAD_KERNEL_STACKS_SIZE 8000


//#define MAX_THREADS    100  /* Max total existing threads */


//#define MAX_PROCESSES  30  /* Max total existing processes */
#define MAX_REFERENCES 50   /* Max number of threads in codapronti */
#define MAXQUEUE 20         /* Max number of processes sleeping in queue.*/

#define USERLEVEL_PROCESS_STACK_SIZE ((64*1024)-4)
#define SUPERVISORLEVEL_PROCESS_STACK_SIZE ((64*1024)-4)

/* Semaphore Constants */

#define SEM_MAX_PROCESSES 30

/* Messages Constants */

#define MAX_MESSAGES (DIM_PAGE/sizeof(MSG_HEADER))

/* GDT Constants */

#define LDT_SELECTOR            (4)
#define GDT_SELECTOR            (0)
#define USER_RPL_SELECTOR       (3)
#define SUPERVISOR_RPL_SELECTOR (0)



#define NULL_SEGMENT_DES          0x0
#define KERNEL_CODE_SEGMENT_DES   0x1
#define KERNEL_DATA_SEGMENT_DES   0x2
#define KERNEL_TSS_SEGMENT_DES    0x3

//#define SUPERVISOR_LDT_SEGMENT_DES 0x4
#define USER_LDT_SEGMENT_DES       0x4

#define USER_CODE_SEGMENT_DES     0x0 /* Defined in LDT */
#define USER_DATA_SEGMENT_DES     0x1 /* Defined in LDT */


#define NULL_SEGMENT_SEL          (NULL_SEGMENT_DES <<3)
#define KERNEL_CODE_SEGMENT_SEL   (KERNEL_CODE_SEGMENT_DES <<3)
#define KERNEL_DATA_SEGMENT_SEL   (KERNEL_DATA_SEGMENT_DES <<3)
#define KERNEL_TSS_SEGMENT_SEL    (KERNEL_TSS_SEGMENT_DES <<3)

//#define SUPERVISOR_LDT_SEGMENT_SEL (SUPERVISOR_LDT_SEGMENT_DES<<3)
#define USER_LDT_SEGMENT_SEL       (USER_LDT_SEGMENT_DES<<3)

/*
#define USER_CODE_SEGMENT_SEL     ((0<<3)|7)
#define USER_DATA_SEGMENT_SEL     ((1<<3)|7)
*/

#define USER_CODE_SEGMENT_SEL     ((USER_CODE_SEGMENT_DES<<3)|LDT_SELECTOR|USER_RPL_SELECTOR)
#define USER_DATA_SEGMENT_SEL     ((USER_DATA_SEGMENT_DES<<3)|LDT_SELECTOR|USER_RPL_SELECTOR)


#define LAST_USED_GDT_ENTRY       USER_LDT_SEGMENT_DES

/*
#define USER_NULL_SEGMENT_DES     0x0
#define USER_CODE_SEGMENT_DES     0x1
#define USER_DATA_SEGMENT_DES     0x2

#define USER_NULL_SEGMENT_SEL     ((USER_NULL_SEGMENT_DES<<3)|7)
#define USER_CODE_SEGMENT_SEL     ((USER_CODE_SEGMENT_DES<<3)|7)
#define USER_DATA_SEGMENT_SEL     ((USER_DATA_SEGMENT_DES<<3)|7)
*/
/* Memory Constants */

#define DIM_PAGE 4096         /* page size */

#endif
