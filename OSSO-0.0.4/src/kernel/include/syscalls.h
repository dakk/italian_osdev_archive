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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/syscalls.h,v $
 * Prototypes of functions and defines of syscall
 ***************************************************************************/

/***************************************************************************/
/*
 * $Id: syscalls.h,v 1.4 2001/08/15 20:06:36 iurlano Exp $
 */
/***************************************************************************/
/*
 * $Log: syscalls.h,v $
 * Revision 1.4  2001/08/15 20:06:36  iurlano
 * Added DMA management functions into the kernel and library
 *
 * Revision 1.3  2001/06/26 21:00:41  iurlano
 * Added mem information program.
 * Changed shell mem command to meminfo
 *
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.9  2001/04/23 15:33:19  iurlano
 * work on FAT and fs protocol
 *
 * Revision 1.8  2001/04/17 18:04:12  iurlano
 * Fixed compilation code
 * Warning: This is a very unstable snapshot
 * There are a lot of strange problems
 *
 * Revision 1.7  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.6  2001/04/03 16:50:04  marcopex
 * Some little event testing & debugging
 *
 * Revision 1.5  2001/02/15 21:04:31  iurlano
 * Restore keyboard functionality (expecting MarcoPex to finish his work)
 * Some Bug fixes.
 *
 * Revision 1.4  2000/12/08 16:45:27  iurlano
 * Thread Work in progress.
 * First working version reached. Needs Debug.
 *
 * Revision 1.3  2000/12/07 11:24:20  marcopex
 * try to compile...
 *
 * Revision 1.2  2000/11/24 18:25:54  iurlano
 * Added first try of a DMA memory management. Yet to be tested
 *
 * Revision 1.1.1.1  2000/05/30 08:11:08  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.3  2000/04/03 19:07:37  iurlano
 * Prototipi driver NE200 PCI
 *
 * Revision 1.2  2000/03/22 23:16:03  iurlano
 * Correzioni drivers
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:41  pecciant
 *
 *
 * Revision 1.17  2000/01/17 23:44:42  iurlano
 * Aggiunto INIT
 *
 * Revision 1.16  2000/01/16 22:43:31  iurlano
 * Aggiunta tabella dei drivers e relative funzioni.
 * da testare
 *
 * Revision 1.15  2000/01/13 13:43:46  iurlano
 * *** empty log message ***
 *
 * Revision 1.14  2000/01/06 17:00:03  iurlano
 * Aggiunto albero directory 'drivers'
 * Aggiunte alcune syscalls
 *
 * Revision 1.13  1999/12/25 11:11:48  iurlano
 * *** empty log message ***
 *
 * Revision 1.12  1999/12/21 17:41:57  iurlano
 * *** empty log message ***
 *
 * Revision 1.11  1999/12/21 13:46:35  pecciant
 * Aggiunte READ/WRITE e dispositivi a caratteri
 *
 * Revision 1.10  1999/12/20 17:32:27  iurlano
 * Aggiunta Fork()
 * Aggiunte costanti di errore
 *
 * Revision 1.9  1999/12/20 13:00:23  iurlano
 * *** empty log message ***
 *
 * Revision 1.8  1999/12/17 22:46:06  iurlano
 *
 * I messaggi sembrano funzionare !!!!
 *
 * Revision 1.7  1999/12/17 17:26:57  iurlano
 * Aggiunte alcune syscall, tipo getpid
 * Lavoro su sistema di messaggi
 * Migliorata libreria
 *
 * Revision 1.6  1999/12/01 12:28:01  iurlano
 * Aggiustato presunto bug in scheduler.c
 * Creata libOSSO per le syscalls attuali
 * Adesso la roba in test compila con -lOSSO
 * Aggiunto costanti per altre syscalls
 * Aggiunta implementazione della syscall signal. E' da provare
 *
 * Revision 1.5  1999/11/29 23:24:28  iurlano
 * Secondo tentativo di gestione di segnali.
 * Sembra funzionare
 *
 * Corretto bug nella gestione delle interruzioni.
 * Aggiunta syscall per settare un signal handler
 *
 * Revision 1.4  1999/11/25 13:08:59  pecciant
 * Standardizzati gli header dei .h
 *
 * Revision 1.3  1997/01/17 05:25:24  iurlano
 * Corretto parte di memory.c per la gestione delle pagine.
 * Tolto malloc() dal codice sostituendola con allocazioni statiche.
 *   E' da togliere da fat.c che per ora e' inutilizzabile.
 * Cambiata allocazione dei registri per syscall.
 * Aggiunte alcune syscall
 *
 * Revision 1.2  1999/10/21 15:38:41  iurlano
 * Aggiustamento commenti
 * Aggiunta header per la paginazione
 *
 */
/***************************************************************************/


#ifndef __SYSCALLS_DEFS
#define __SYSCALLS_DEFS

#define SYS_EXI                0 /* Non Attiva */

#define SYS_SIGNAL              1
#define SYS_SIGNAL_HANDLER      2

#define SYS_SEND_MSG            3
#define SYS_RECV_MSG            4
#define SYS_RECV_MSG_ALL        5 /* Non Attiva */
#define SYS_GETB_MSG            6
#define SYS_FREE_MSG            7
#define SYS_DISP_MSG            8 /* Non Attiva */

#define SYS_PRINT              10
#define SYS_GETPID             11
#define SYS_GETPPID            12

#define SYS_MALLOSSO           19
#define SYS_FREEOSSO           20
#define SYS_FORK               21
#define SYS_CREATE_PROCESS_ELF 22
#define SYS_CREATE_THREAD      23

#define SYS_EXIT               25
#define SYS_EXIT_THREAD        26

/* DMA */

#define SYS_GET_DMA_MEM        30
#define SYS_FREE_DMA_MEM       31

#define SYS_REQ_DMA_CHANNEL    33
#define SYS_FREE_DMA_CHANNEL   34
#define SYS_START_DMA_OP       35

/* CHAR DEVICES */
#define SYS_GET_CH_BUFF        40
#define SYS_REL_CH_BUFF        41        

/* READ / WRITE */
#define SYS_WRITE              50
#define SYS_READ               51  

/* SEMAPHORES */
#define SYS_INIT_SEM           70
#define SYS_INIT_BOOL_SEM      71
#define SYS_P_SEM              72
#define SYS_BOOL_P_SEM         73
#define SYS_NBLK_P_SEM         74
#define SYS_NBLK_BOOL_P_SEM    75
#define SYS_V_SEM              76
#define SYS_BOOL_V_SEM         77

/* EVENTS */

#define SYS_GETEVENT           80
#define SYS_CLEAREVENT         81
#define SYS_SETEVENT           82
#define SYS_ESIGNAL            83
#define SYS_EWAIT              84

/* Drivers */
#define DRV_PRINT_PROC_INFO   129
#define DRV_REGISTER          130
#define DRV_GET_CLASS         131

#define DRV_SCROLL_UP         160
#define DRV_SCROLL_DOWN       161
#define PCI_READ_BYTE         180
#define PCI_READ_WORD         181
#define PCI_READ_DWORD        182

#define SYS_GET_HW_SEM        999

#define SYS_GETMPSPID        1010
#define SYS_SETMPSPID        1011

/* SYSINFO */
#define SYS_GET_MEM_FREE_PAGES    10000
#define SYS_GET_MEM_INFO          10001
#define SYS_GET_PROCESS_INFO      10002


/* SYS_PRINT Arguments */

#define SYS_PRINT_CHAR          0
#define SYS_PRINT_INT           1
#define SYS_PRINT_DWORD         2
#define SYS_PRINT_HEX           3

#endif



