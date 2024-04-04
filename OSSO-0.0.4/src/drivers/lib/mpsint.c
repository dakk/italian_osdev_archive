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
 * file : $Source: /cvsroot/osso/OSSO/src/drivers/lib/mpsint.c,v $
 * Description: Generic data for protocols of MPS.
 ***************************************************************************

 ***************************************************************************
 * $Id: mpsint.c,v 1.9 2001/08/04 18:22:47 steros Exp $
 ***************************************************************************
 *
 * $Log: mpsint.c,v $
 * Revision 1.9  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.8  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.7  2001/06/03 09:53:17  steros
 * Bug fixes.
 *
 * Revision 1.6  2001/05/02 18:50:27  steros
 * Driver dependancies automation implemented.
 *
 * Revision 1.5  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 * Revision 1.4  2001/04/29 17:40:48  steros
 * Added dynamic mps pid request on drv2mps also.
 * Improved mps and his protocols.
 * There's still problems in taking MPS pid from process MPS_Pid variable.
 *
 * Revision 1.3  2001/04/28 12:15:52  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.2  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.1  2001/03/25 15:27:25  steros
 * Divided MPS interfaces in more structured files.
 *
 *
 ***************************************************************************/


#include <drivers/protocols/mpsint.h>

char *MpsErrors[] = {
  /*  0 */  "No errors",
  /*  1 */  "Memory allocation error",
  /*  2 */  "Unexpected message received",
  /*  3 */  "Version is not compatible",
  /*  4 */  "Bad message"
};


char *DrvMpsErrors[] = {
  /*  1 */  "Driver name too long",
  /*  2 */  "Driver category name too long",
  /*  3 */  "Too many drivers",
  /*  4 */  "Driver alredy registered",
  /*  5 */  "Driver not present",
};


char *PrgMpsErrors[] = {
  /*  1 */  "Driver not found",
};




#include <printf.h>

/*
 This function returns the descrition string of an error.
*/
char *MpsStrerror(MpsRetVal err) {
/*-*  // This code generate a seg-fault!!!
printf("<MpsStrerror> ind: %d\n",(int)(PrgMpsErrors[0]));
printf("<MpsStrerror> char: %c\n",*PrgMpsErrors[0]);
*-*/
  if (err>=0 && err<=MPS_NUM_ERRMSG)
    return MpsErrors[err];
  else {
    if (err>ERRDRV_DM && err<ERRDRV_DM_LAST)
      return DrvMpsErrors[err-ERRDRV_DM-1];
    else {
      if (err>ERRDRV_PM && err<=(ERRDRV_PM_LAST))
        return PrgMpsErrors[err-ERRDRV_PM-1];
      else
        return "Unknown error";
    }
  }
}
