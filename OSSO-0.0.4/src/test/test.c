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


/******************************* O S S O ** TEST ***************************
 * file : test/test.c                                                      *
 * Programma di test                                                       *
 ***************************************************************************

 ***************************************************************************
 * $Id: test.c,v 1.3 2001/04/26 19:25:54 iurlano Exp $
 ***************************************************************************
 *
 * $Log: test.c,v $
 * Revision 1.3  2001/04/26 19:25:54  iurlano
 * fat work
 * bugfixes
 *
 * Revision 1.2  2000/08/22 14:57:55  iurlano
 * More Translation and cleaning
 *
 * Revision 1.1.1.1  2000/05/30 08:11:21  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.7  1999/12/10 13:34:59  iurlano
 * *** empty log message ***
 *
 * Revision 1.6  1999/12/02 15:04:12  iurlano
 * Pulizia della directory test
 *
 *
 ***************************************************************************/


#include <printf.h>
#include <signals.h>

char stringa[20]="Funziona?\n";
void main(void) 
{
  int *t;
  char stringa2[20]="RRIIIRFunziona?\n";

  printf("K\n'%s'\n",stringa);
  printf("Address of stringa : %x\n",(unsigned int)stringa);
  printf("Address of stringa2 : %x\n%s\n",(unsigned int)stringa2,stringa2);

}


