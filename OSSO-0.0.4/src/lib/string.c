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


/*************************** LIB O S S O ***********************************
 * file : lib/string.c                                                     *
 * String handling routines                                                *
 ***************************************************************************

 ***************************************************************************
 * $Id: string.c,v 1.7 2001/07/08 11:12:51 steros Exp $
 ***************************************************************************
 *
 * $Log: string.c,v $
 * Revision 1.7  2001/07/08 11:12:51  steros
 * Converted MPS to dynamic allocation and other little fix.
 *
 * Revision 1.6  2001/06/25 20:23:01  steros
 * Added strcat.
 *
 * Revision 1.5  2001/04/20 15:30:59  iurlano
 * Changes in to the fat driver
 * creating the filesystem protocol
 *
 * Revision 1.4  2001/01/08 09:52:54  steros
 * New boot procedure implemented.
 *
 * Revision 1.3  2000/12/04 17:17:45  steros
 * Implemented kernel level strcpy and lib/kernel level sprintf.
 *
 * Revision 1.2  2000/08/03 15:57:44  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:19  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.3  2000/03/26 13:20:23  iurlano
 * Completamento protocollo di INIT
 * Da completarne il test
 *
 * Revision 1.2  2000/03/20 12:02:05  iurlano
 * lavoro su drivers
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.2  1999/12/02 15:02:43  iurlano
 *
 * Aggiustato files nella directory lib
 *
 *
 ***************************************************************************/


int strcmp(const char * s1,const char * s2) {
  const char *scan1,* scan2;
  int ret;

  scan1=&s1[0];
  scan2=&s2[0];

  ret=*scan1-*scan2;
  while (ret==0 && (*scan1)!='\0' && (*scan2)!='\0') {
    scan1++;
    scan2++;
    ret=*scan1-*scan2;
  }
  return ret;
}


int strcasecmp(const char *s1, const char *s2) {
  const char *p1=s1, *p2=s2;
  char diff='A'-'a', c1, c2;
  int ret;

  c1=(*p1>='A' && *p1<='Z') ? (*p1)-diff : *p1;
  c2=(*p2>='A' && *p2<='Z') ? (*p2)-diff : *p2;
  ret=c1-c2;
  if (!*p1 || !*p2) return ret;
  for (p1++, p2++; !ret && *p1 && *p2; p1++, p2++) {
    c1=(*p1>='A' && *p1<='Z') ? (*p1)-diff : *p1;
    c2=(*p2>='A' && *p2<='Z') ? (*p2)-diff : *p2;
    ret=c1-c2;
  }
  return ret;
}

int strncmp(const char * s1,const char * s2,unsigned int n) {
  const char *scan1,* scan2;
  int ret;
  int t;

  if (n==0) return 0;

  scan1=&s1[0];
  scan2=&s2[0];

  ret=*scan1-*scan2;
  t=1;
  while (ret==0 && (t<n) && (*scan1)!='\0' && (*scan2)!='\0') {
    scan1++;
    scan2++;
    ret=*scan1-*scan2;
    t++;
  }
  return ret;
}


char *strcpy(char *dest, const char *src) {
  int i;
  char * d;
  d=dest;
  for (i=0;*src;i++) *dest++=*src++;
  *dest='\0';
  return d;
}


char *strncpy(char *dest, const char *src, unsigned int n) {
  int i;
  for (i=0;src[i] && i<n;i++) dest[i]=src[i];
  dest[i]='\0';
  return dest;
}


unsigned int strlen(const char *s) {
  int i;
  for (i=0;s[i];i++) ;
  return i;
}


char *strcat(char *dest, const char *src) {
  char *d=dest;

  for (; *d; d++);
  strcpy(d,src);
  return dest;
}


void *memcpy(void *dest, const void *src, unsigned int n) {
  int i;
  for (i=0;i<n;i++) ((char*)dest)[i]=((char*)src)[i];
  return dest;
}


static void sprintf_s(char **point, char *s) {
  int i;

  for (i=0;s[i]!='\0';i++) {
    **point=s[i];
    (*point)++;
  }
}


static void sprintf_d(char **point, int d) {
  char tmp[20];
  int i;

  if (d==0) {
    **point='0';
    (*point)++;
  } else {
    i=0;
    tmp[0]='0';
    while (d>0) {
      tmp[i++]='0'+d%10;
      d/=10;
    }
    for (i--; i>=0; i--) {
      **point=tmp[i];
      (*point)++;
    }
  }
}


static void sprintf_ld(char **point, long int d) {
  char tmp[20];
  int i;

  if (d==0) {
    **point='0';
    (*point)++;
  } else {
    i=0;
    tmp[0]='0';
    while (d>0) {
      tmp[i++]='0'+d%10;
      d/=10;
    }
    for (i--;i>=0;i--) {
      **point=tmp[i];
      (*point)++;
    }
  }
}


static void sprintf_u(char **point, unsigned int d) {
  char tmp[20];
  int i;

  if (d==0) {
    **point='0';
    (*point)++;
  } else {
    i=0;
    tmp[0]='0';
    while (d>0) {
      tmp[i++]='0'+d%10;
      d/=10;
    }
    for (i--;i>=0;i--) {
      **point=tmp[i];
      (*point)++;
    }
  }
}


static void sprintf_b(char **point, unsigned int d) {
  char tmp[35];
  int i;

  if (d==0) {
    **point='0';
    (*point)++;
  } else {
    i=0;
    tmp[0]='0';
    while (d>0) {
      tmp[i++]='0'+(d&1);
      d>>=1;
    }
    for (i--;i>=0;i--) {
      **point=tmp[i];
      (*point)++;
    }
  }
}


static void sprintf_x(char **point, int d) {
  char tmp[20];
  int i;

  if (d==0) {
    **point='0';
    (*point)++;
  } else {
    i=0;
    tmp[i]='0';
    while (d>0) {
      if ((d&0xF)<0xA)
	tmp[i++]='0'+(d&0xF);
      else tmp[i++]='A'+(d&0xF)-10;
      d>>=4;
    }
    for (i--;i>=0;i--) {
      **point=tmp[i];
      (*point)++;
    }
  }
}


static void sprintf_lx(char **point, long int d) {
  char tmp[40];
  int i;

  if (d==0) {
    **point='0';
    (*point)++;
  } else {
    i=0;
    tmp[i]='0';
    while (d>0) {
      if ((d&0xF)<0xA)
	tmp[i++]='0'+(d&0xF);
      else tmp[i++]='A'+(d&0xF)-10;
      d>>=4;
    }
    for (i--; i>=0; i--) {
      **point=tmp[i];
      (*point)++;
    }
  }
}


int sprintf(char *str, char *format, ...) {
  int i;
  char *argomenti; /* pointer to arguments to be printed */
  char *temp;
  char *point=str;

  argomenti=(char *) &format + sizeof(format);

  /* Initialized to the first element */

  for (i=0;format[i]!='\0';i++) {
    /* Scan */
    switch (format[i]) {
    case '%':
      if (format[i+1]=='s') {  /* STRING */
	temp=*((char **)argomenti);
	sprintf_s(&point,temp);
	argomenti+=4; /* Pointer update */
	i++;
      }
      else if (format[i+1]=='c') { /* Char */
        *point=*((char *)argomenti);
        point++;
	argomenti+=4;
	i++;
      }
      else if (format[i+1]=='d') {
	sprintf_d(&point,*((signed int*)argomenti));
	argomenti+=4;
	i++;
      }
      else if (format[i+1]=='l') {
	if (format[i+2]=='d') {
	  sprintf_ld(&point,*((long signed int*)argomenti));
	  argomenti+=4;
	  i+=2;
	} else if (format[i+2]=='x') {
	  sprintf_lx(&point,*((long unsigned int*)argomenti));
	  argomenti+=4;
	  i+=2;
	}
      }
      else if (format[i+1]=='u') { /* unsigned 32 bit integer */
	sprintf_u(&point,*((unsigned int*)argomenti));
	argomenti+=4;
	i++;
      }
      else if (format[i+1]=='x') { /* Hexdecimal 32 bit integer */
	sprintf_x(&point,*((unsigned int*)argomenti));
	argomenti+=4;
	i++;
      }
      else if (format[i+1]=='b') { /* 32 bit binary*/
	sprintf_b(&point,*((unsigned int*)argomenti));
	argomenti+=4;
	i++;
      }
      break;
    default: /* Normal char */
      *point=format[i];
      point++;
      break;
    }
  }
  *point='\0';
  return (int)(point-str);
}
