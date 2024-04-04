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

#include <ctype.h>
#include <string.h>

int isalnum(char c)
{
	return (isalpha(c) || isdigit(c));
}

int isalpha(char c)
{
	if ((c >= 'A' && c <= 'z'))
		return (1);
	else
		return (0);
}

int iscntrl(char c)
{
	if (c < '0') {
		return 1;
	} else {
		return 0;
	}
}


int islower(char c)
{
	if ((c >= 'a' && c <= 'z'))
		return (1);
	else
		return (0);
}

int isspace(char c)
{
	if ((c >= 0x09 && c <= 0x0d) || (c == 0x20))
		return (1);
	return (0);
}

int isupper(char c)
{
	if ((c >= 'A' && c <= 'Z'))
		return (1);
	else
		return (0);
}

char toupper(char c)
{
	if (c >= 'a' && c <= 'z')
		return (c - 'a' + 'A');
	else
		return (c);
}

char tolower(char c)
{
	if (c >= 'A' && c <= 'Z')
		return (c - 'A' + 'a');
	else
		return (c);
}


int isspecial(double d, char *bufp)
{
	/* IEEE standard number rapresentation */
	register struct IEEEdp {
		unsigned manl:32;
		unsigned manh:20;
		unsigned exp:11;
		unsigned sign:1;
	} *ip = (struct IEEEdp *) &d;

	if (ip->exp != 0x7ff)
		return (0);
	if (ip->manh || ip->manl) {
		if (bufp != NULL)
			strcpy(bufp, "NaN");
		return (1);
	} else if (ip->sign) {
		if (bufp != NULL)
			strcpy(bufp, "+Inf");
		return (2);
	} else {
		if (bufp != NULL)
			strcpy(bufp, "-Inf");
		return (3);
	}
}

int isnumber(char c, int base)
{
	static char *digits = "0123456789ABCDEF";
	if ((c >= '0' && c <= digits[base - 1]))
		return (1);
	else
		return (0);
}

int tonumber(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	else if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	else
		return (c);
}

char todigit(int c)
{
	if (c >= 0 && c <= 9)
		return (c + '0');
	else if (c >= 10 && c <= 15)
		return (c + 'A' - 10);
	else
		return (c);
}

int isxdigit(char c)
{
	if ((c >= '0' && c <= 'F') || (c >= 'a' && c <= 'f'))
		return (1);
	else
		return (0);
}

int isdigit(char c)
{
	if ((c >= '0' && c <= '9'))
		return (1);
	else
		return (0);
}
