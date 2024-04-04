/* string.h -- strings manipulation functions definitions */
/******************************************************************************
* Exabyte Operating System                                                    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; either version 2              *
* of the License, or (at your option) any later version.                      *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License           *
* along with this program; if not, write to the Free Software                 *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
*******************************************************************************/

#ifndef STRING_H
#define STRING_H

#include <arch-ia32/string.h>	// assembly architecture specific functions

#ifdef VBSTRING

/* Visual Basic compatibility functions */

static inline unsigned char *ltrim(unsigned char *string)
{
	while (*string == ' ')
		string++;
	return string;
}

static inline unsigned char *rtrim(unsigned char * string)
{
	unsigned char *aux = string;
	
	while( *aux )
		aux++;
	
	while( *(--aux) == ' ' )
		*aux='\0';

	return string;
}

static inline unsigned char *trim(unsigned char * string)
{
	return ltrim( rtrim(string) );
}


static inline int instr(int start, unsigned char * string1, unsigned char * string2)
{
	unsigned char *aux = string2;

	if( !(*string1) )
		return 0;

	if( !(*string2) )
		return start;

	string1 += start;

	while( *string1 )
	{
		if( *aux == *string1 )
			aux++;
		else
		{
			aux=string2;
			start++;
		}
		if( !(*aux) )
			return start+1;
		
		string1++;
	}
	
	return 0;
}

static inline void left(unsigned char *dest, unsigned const char * string, size_t lenght)
{
	while( lenght-- && (*string) )
		*dest++ = *string++; 
	*dest='\0';
}


static inline void right(unsigned char *dest, unsigned const char * string, size_t lenght)
{
	unsigned const char *aux = string;

	while( *aux++ );

	if( aux-string < lenght )
		lenght = aux-string+1;	

	dest += lenght-1;

	while( lenght-- )
		*dest-- = *aux--;
}


static inline void mid(unsigned char *dest, unsigned const char * string, size_t start, size_t lenght)
{
	string += start-1;
	while( lenght-- && *string )
		*dest++ = *string++;
	*dest = '\0';
}

#endif	/* VBSTRING */

#endif	/* STRING_H */
