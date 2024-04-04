/*!	\file lib/stdlib/atoi.c
 *	\brief LIBC alphanumeric to integer routine.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-10-27
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <stdlib.h>

//! \brief Convert a string to a signed int.
//! \param str The start of the string.
//! \return The integer value of the string.
int atoi(const char *str)
{
	int num=0, sign=1;
	char ch;

	if ((ch = *str++)=='\0') return(0);

	if ((ch=='-') || (ch=='+'))
	{
		sign = ((ch=='+') ? 1 : -1);
		ch = *str++;
	}

	while ((ch >= '0') && (ch <= '9'))
	{
		num = num*10 + (ch-'0');
		ch = *str++;
	}
	return (num*sign);
}

