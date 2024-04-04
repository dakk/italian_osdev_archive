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
 * file : $Source: /cvsroot/osso/OSSO/src/include/drivers/protocols/mpsint.h,v $
 * Description: Generic data for protocols of MPS.
 ***************************************************************************

 ***************************************************************************
 * $Id: mpsint.h,v 1.8 2001/08/04 18:22:47 steros Exp $
 ***************************************************************************
 *
 * $Log: mpsint.h,v $
 * Revision 1.8  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.7  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.6  2001/05/02 18:50:27  steros
 * Driver dependancies automation implemented.
 *
 * Revision 1.5  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 * Revision 1.4  2001/05/01 01:04:19  iurlano
 * Work on Dynamic Linker problem
 * Seems like that the shared MPS_Pid is working now.
 * I thing that the condivision of a variable from the lib and the program works if the variable is declared in the program.
 * for the other case, I've to work on it and I don't think I'm going to do it very soon.
 *
 * Revision 1.3  2001/04/29 17:40:48  steros
 * Added dynamic mps pid request on drv2mps also.
 * Improved mps and his protocols.
 * There's still problems in taking MPS pid from process MPS_Pid variable.
 *
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.2  2001/04/20 20:45:36  steros
 * Added GetDriverPid function for driver pid retrival from MPS.
 *
 * Revision 1.1  2001/03/25 15:27:25  steros
 * Divided MPS interfaces in more structured files.
 *
 *
 ***************************************************************************/


#ifndef _MPSINT_H_
#define _MPSINT_H_


#define MAX_DRIVER_NAME_LEN     32
#define MAX_DRIVER_CATEGORY_LEN 32





/*----------------------------------*/
/*  DYNAMIC MPS PID ENABLE/DISABLE  */
/*----------------------------------*/

/*
 Abilita la ricerca dinamica del pid.
 Se non definito usa la costante definita in prg2mps.c (6).
*/
  #define DYNAMIC_MPS_PID

/*
 Se definito ricalcola ogni volta il pid di mps.
 Se non definito lo prende direttamente dalla variabile MPS_Pid
 del programma che lo usa.
 ATTENZIONE: Funziona solo se DYNAMIC_MPS_PID e' definito.
*/
  #define REQUEST_MPS_PID

/*----------------------------------*/





/*---------------------------------------------------------------------*/
/*                            Error types.                             */
/*---------------------------------------------------------------------*/

/*
  ATTENTION: keep synchronized these structures!!!
             See mpsint.c for error strings definitions,
*/

typedef enum {

  /*** ----------------------- Common errors ----------------------- ***/

  /*    0 */  ERRDRV_NOERR = 0,    /* No problems: all right!          */
  /*    1 */  ERRDRV_NOMEM,        /* Memory allocation error.         */
  /*    2 */  ERRDRV_WRONGMSG,     /* Message is not the expected one. */
  /*    3 */  ERRDRV_WRONGVER,     /* Version is not compatible.       */
  /*    4 */  ERRDRV_BADMSG,       /* Format of message unknown.       */

  /* LAST */  ERRDRV_LAST,         /* THIS MUST BE EVER LAST!!! */

  /*** --------------------- Drv -> Mps errors --------------------- ***/

  /* 1000 */  ERRDRV_DM = 1000,
  /*    1 */  ERRDRV_DM_DRVTOOLONG,
  /*    2 */  ERRDRV_DM_CATTOOLONG,
  /*    3 */  ERRDRV_DM_TOOMANYDRV,
  /*    4 */  ERRDRV_DM_DRVPRESENT,
  /*    5 */  ERRDRV_DM_DRVNOTPRESENT,

  /* LAST */  ERRDRV_DM_LAST,         /* THIS MUST BE EVER LAST!!! */

  /*** --------------------- Prg -> Mps errors --------------------- ***/

  /* 2000 */  ERRDRV_PM = 2000,
  /*    1 */  ERRDRV_PM_DRVNOTFOUND,

  /* LAST */  ERRDRV_PM_LAST,         /* THIS MUST BE EVER LAST!!! */
} MpsRetVal;


#define    MPS_NUM_ERRMSG ERRDRV_LAST - ERRDRV_NOERR - 1
#define DRVMPS_NUM_ERRMSG ERRDRV_DM_LAST - ERRDRV_DM - 1
#define PRGMPS_NUM_ERRMSG ERRDRV_PM_LAST - ERRDRV_PM - 1



/*---------------------------------------------------------------------*/
/*                         Classes definitions.                        */
/*---------------------------------------------------------------------*/

typedef enum {
     MPS_CLASS,
  DRVMPS_CLASS,
  PRGMPS_CLASS
} mps_class_type;


/*---------------------------------------------------------------------*/
/*                     Generic message definition.                     */
/*---------------------------------------------------------------------*/

typedef struct {
  mps_class_type Class;
  int            Version;
  int            Release;
  MpsRetVal      Status;
  char Body[1];           /* Variable length. */
} mps_msg_type;



/*---------------------------------------------------------------------*/
/*                   MPS class messages definition.                    */
/*---------------------------------------------------------------------*/

typedef enum {
  MPS_WAITFORDRIVER,
  MPS_INIT_COMPLETE,
  MPS_DEBUG_REQUEST,
  MPS_DEBUG_REQUEST_REPLY
} mps_request_type;


typedef struct {
  mps_class_type   Class;
  mps_request_type type;
  int              pid;
} mps_msg_t;


/*---------------------------------------------------------------------*/
/*                             Prototipes.                             */
/*---------------------------------------------------------------------*/

extern char *MpsErrors[];
extern char *DrvMpsErrors[];
extern char *PrgMpsErrors[];


char *MpsStrerror(MpsRetVal err);


#endif
