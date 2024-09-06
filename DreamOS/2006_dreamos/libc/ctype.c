/* Alcune funzioni relative ai caratteri
 * Dreamos
 * ctype.c
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*
 * Autrice: Lisa Vitolo
 * Data: 26 gennaio 2006
 */

#include<ctype.h>

/* controlla se c è una cifra */
int isdigit(int c) {
  if (c >= 48 && c <= 57)
    return 1;
  return 0;
}

/* controlla se c è una lettera */
int isalpha(int c)
{
  if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122 ))
    return 1;
  return 0;
}

/* controlla se c è una cifra o una lettera */
int isalnum(int c)
{
  return (isalpha(c) || isdigit(c));
}

/* controlla se c è una cifra esadecimale */
int isxdigit(int c)
{
  if (isdigit(c) || (c >= 65 && c <= 70 ))
    return 1;
  return 0;
}

/* controlla se c è un carattere minuscolo */
int islower(int c)
{
  if (c >= 97 && c <= 122)
    return 1;
  return 0;
}

/* controlla se c è un carattere maiuscolo */
int isupper(int c)
{
  if (c >= 65 && c <= 90)
    return 1;
  return 0;
}

/* converte c in un carattere minuscolo */
int tolower(int c)
{
  if (isalpha(c) == 0 || islower(c))
    return c;
  else return c + OFFSET;
}

/* converte c in un carattere maiuscolo */
int toupper(int c)
{
  if (isalpha(c) == 0 || isupper(c))
    return c;
  else return c - OFFSET;
}
