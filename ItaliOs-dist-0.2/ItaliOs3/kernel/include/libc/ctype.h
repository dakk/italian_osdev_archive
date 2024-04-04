/*
ItaliOs
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _C_TYPE_H_
#define _C_TYPE_H_

#include <stddef.h>

int isalnum(char c);

int isalpha(char c);

int iscntrl(char c);


int islower(char c);

int isspace(char c);

int isupper(char c);

char toupper(char c);

char tolower(char c);

int isspecial(double d, char *bufp);

int isnumber(char c,int base);

int tonumber(char c);

char todigit(int c);

int isxdigit(char c);

int isdigit(char c);


#endif
