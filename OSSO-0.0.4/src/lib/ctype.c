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


int isupper (int c) {
  return ((c>='A') && (c<='Z'));
}

int islower (int c) {
  return ((c>='a') && (c<='z'));
}

int isdigit (int c) {
  return ((c>='0') && (c<='9'));
}

int isalpha (int c) {
  return (isupper(c) || islower(c));
}

int isalnum (int c) {
  return (isalpha(c) || isdigit(c));
}

int isascii (int c) {
  return (c==(c&0x7F));
}

int isblank (int c) {
  return ((c==' ') || (c=='\t'));
}

int isgraph (int c) {
  /* DA FARE */
  return 0;
}

int ispunct (int c) {
  /* DA FARE */
  return 0;
}

int iscntrl (int c) {
  /* DA FARE */
  return 0;
}

int isprint (int c) {
  return (isgraph(c) || (c==' '));
}

int isspace (int c) {
  return ((c==' ') || (c=='\t') || (c=='\n') || (c=='\f') 
	  || (c=='\r') || (c=='\v'));
}

int isxdigit (int c) {
  return (isdigit(c) || (c>='A' && c<='F') || (c>='a' && c<='f'));
}







