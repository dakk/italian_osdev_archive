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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/errors.h,v $
 * Description: Kernel's error codes
 ***************************************************************************

 ***************************************************************************
 * $Id: errors.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: errors.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.6  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.5  2000/12/04 17:47:55  marcopex
 * Event handling started
 *
 * Revision 1.3  2000/10/16 14:12:16  iurlano
 * Translation in progress
 *
 *
 * Revision 1.1.1.1  2000/05/30 08:11:06  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

/* General error codes */
#define E_SUCCESS              0   /* No error */
#define E_NOT_IMPLEMENTED_YET  1   /* ... */

/* Char devices */
#define E_NOMORECHDEV          30
#define E_WRONGCHDEV           31
#define E_BUFTOOLONG           32

/* Events */
#define E_EVENT_EXIST          40  /* The requested event already exist */
#define E_EVENT_NOTEXIST       41  /* The named event not exist */
#define E_EVENT_NOPERMS        42  /* Permission denied */
#define E_EVENT_PUB_TAKEN      43  /* This event is public and already taken */
#define E_EVENT_PUB_NOTTAKEN   44  /* This event is public but it's free (zzoserve?) */
#define E_EVENT_NOMORE         45  /* No space in the kernel for more events (ALE'!!!) */
#define E_EVENT_WRONGVALS      46  /* Wrong settings */

#define E_SHOULD_BLOCK         50  /* The requested operation should have blocked */

#define E_PID_NOT_FOUND        60  /* Process not found in process list */

#define E_PID_DEAD             61  /* Requested process is already dead */

#define E_NOT_OWNER            128
#define E_PERM_DENIED          129




