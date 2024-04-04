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
 * file : $Source: /cvsroot/osso/OSSO/src/drivers/keyscancodes.h,v $
 * Keyboard scancodes definition
 ***************************************************************************

 ***************************************************************************
 * $Id: keyscancodes.h,v 1.2 2000/08/04 17:47:58 iurlano Exp $
 ***************************************************************************
 *
 * $Log: keyscancodes.h,v $
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:01  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:43  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:40  pecciant
 *
 *
 * Revision 1.1  2000/01/06 17:00:03  iurlano
 * Aggiunto albero directory 'drivers'
 * Aggiunte alcune syscalls
 *
 * Revision 1.2  1999/11/25 13:08:56  pecciant
 * Standardizzati gli header dei .h
 *
 *
 ***************************************************************************/




#ifndef __KEYSCANCODES_DEFS
#define __KEYSCANCODES_DEFS

/* + 128 == extended code */

#define kscn_Left_Shift      ( 42       )
#define kscn_Right_Shift     ( 54       )

#define kscn_Left_Control    ( 29       )
#define kscn_Right_Control   ( 29 + 128 )

#define kscn_Left_Alt        ( 56       )
#define kscn_Right_Alt       ( 56 + 128 )

typedef unsigned char char_line[4];


/* Char map, with modifiers */

/* normal,shift,control,alt */

char_line char_table[]={
  {0,0,0,0},
  {27,27,27,27},
  {'1','!',0,'1'},
  {'2','"',0,'2'},
  {'3',156,0,'3'},
  {'4','$',0,'4'},
  {'5','%',0,'5'},
  {'6','&',0,'6'},
  {'7','/',0,'7'},
  {'8','(',0,'{'},
  {'9',')',0,'}'},
  {'0','=',0,'~'},
  {39,'?',0,'`'},
  {141,'^',0,0},
  {127,127,127,127},
  {9,9,9,9},
  {'q','Q',0,'q'},
  {'w','W',0,'w'},
  {'e','E',0,'e'},
  {'r','R',0,'r'},
  {'t','T',0,'t'},
  {'y','Y',0,'y'},
  {'u','U',0,'u'},
  {'i','I',0,'i'},
  {'o','O',0,'o'},
  {'p','P',0,'p'},
  {138,130,0,'['},
  {'+','*',0,']'},
  {'\n','\n','\n','\n'}, /* Return */
  {0,0,0,0}, /* left CONTROL */
  {'a','A',0,'a'},
  {'s','S',0,'s'},
  {'d','D',0,'d'},
  {'f','F',0,'f'},
  {'g','G',0,'g'},
  {'h','H',0,'h'},
  {'j','J',0,'j'},
  {'k','K',0,'k'},
  {'l','L',0,'l'},
  {162,128,0,'@'},
  {133,248,0,'#'},
  {'\\','|',0,'\\'},
  {0,0,0,0}, /* left Shift */
  {151,0,0,0},
  {'z','Z',0,'z'},
  {'x','X',0,'x'},
  {'c','C',0,'c'},
  {'v','V',0,'v'},
  {'b','B',0,'b'},
  {'n','N',0,'n'},
  {'m','M',0,'m'},
  {',',';',0,0},
  {'.',':',0,0},
  {'-','_',0,0},
  {0,0,0,0},             /* right Shift */
  {'*','*',0,'*'},       /* * of numeric pad */
  {0,0,0,0},             /* left Alt */
  {' ',' ',0,0},
  {0,0,0,0},             /* Caps Lock */
  {0,0,0,0},             /* F1 */
  {0,0,0,0},             /* F2 */
  {0,0,0,0},             /* F3 */
  {0,0,0,0},             /* F4 */
  {0,0,0,0},             /* F5 */
  {0,0,0,0},             /* F6 */
  {0,0,0,0},             /* F7 */
  {0,0,0,0},             /* F8 */
  {0,0,0,0},             /* F9 */
  {0,0,0,0},             /* F10 */

  {0,0,0,0},             /* NumLock */
  {0,0,0,0},             /* Scroll Lock */

  {'7','7',0,'7'},       /* 7 of numeric pad */
  {'8','8',0,'8'},
  {'9','9',0,'9'},
  {'-','-',0,'-'},
  {'4','4',0,'4'},
  {'5','5',0,'5'},
  {'6','6',0,'6'},
  {'+','+',0,'+'},
  {'1','1',0,'1'},
  {'2','2',0,'2'},
  {'3','3',0,'3'},
  {'0','0',0,'0'},
  {'.','.',0,'.'},
  {0,0,0,0}, /* Unknown key */
  {0,0,0,0}, /* Unknown key */
  {0,0,0,0}, /* Unknown key */
  {0,0,0,0}, /* F11 */
  {0,0,0,0}, /* F12 */
};

#endif
