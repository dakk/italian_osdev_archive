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

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sprintf.h>

#define STD_SIZE	0
#define SHORT_SIZE	1
#define LONG_SIZE	2

int vsprintf(char *buf, char *fmt, va_list parms)
{
	int scanned = 0, w = 0, prec = 0, l = 0, size = 0;
	int n1 = 0;
	unsigned n2 = 0, parsing = 0, flag = 0;
	double n3 = 0.0;
	char *base = buf;
	char *sp = NULL;
	void *fp = NULL;
	while (*fmt != 0) {
		if (*fmt != '%' && !parsing) {
			/* No token detected */
			*buf++ = *fmt++;
			scanned++;
		} else {
			/* We need to make a conversion */
			if (*fmt == '%') {
				fmt++;
				parsing = 1;
				w = 10;
				prec = 4;
				size = STD_SIZE;
				flag = 0;
			}
			/* Parse token */
			switch (*fmt) {
			case '%':
				*buf++ = '%';
				scanned++;
				parsing = 0;
				break;
			case 'c':
				*buf++ = va_arg(parms, char);
				scanned++;
				parsing = 0;
				break;
			case 'i':
			case 'd':
				switch (size) {
				case STD_SIZE:
					n1 = va_arg(parms, int);
					break;
				case LONG_SIZE:
					n1 = va_arg(parms, long int);
					break;
				case SHORT_SIZE:
					n1 = va_arg(parms, short int);
					break;
				}
				l = dcvt(n1, buf, 10, w, flag);
				scanned += l;
				buf += l;
				parsing = 0;
				break;
			case 'u':
				switch (size) {
				case STD_SIZE:
					n2 = va_arg(parms, unsigned);
					break;
				case LONG_SIZE:
					n2 = va_arg(parms, unsigned long);
					break;
				case SHORT_SIZE:
					n2 = va_arg(parms, unsigned short);
					break;
				}
				l = ucvt(n2, buf, 10, w, flag);
				scanned += l;
				buf += l;
				parsing = 0;
				break;
			case 'x':
			case 'X':
				switch (size) {
				case STD_SIZE:
					n2 = va_arg(parms, unsigned);
					break;
				case LONG_SIZE:
					n2 = va_arg(parms, unsigned long);
					break;
				case SHORT_SIZE:
					n2 = va_arg(parms, unsigned short);
					break;
				}
				l = ucvt(n2, buf, 16, w, flag);
				scanned += l;
				buf += l;
				parsing = 0;
				break;
			case 'p':
				fp = va_arg(parms, void *);
				n2 = (unsigned long) (fp);
				l = ucvt(n2, buf, 16, w, 0);
				scanned += l;
				buf += l;
				parsing = 0;
				break;
			case 's':
				sp = va_arg(parms, char *);
				while (*sp != 0) {
					*buf++ = *sp++;
					l++;
				}
				scanned += l;
				parsing = 0;
				break;
			case 'f':
				switch (size) {
				case STD_SIZE:
					n3 = va_arg(parms, double);
					break;
				case LONG_SIZE:
					n3 = va_arg(parms, long double);
					break;
					/*
					 * It seems that the compilers push a float as
					 * a double ... Need to check!
					 */
				case SHORT_SIZE:
					n3 = va_arg(parms, double);
					break;
				}
				l = fcvt(n3, buf, w, prec, flag);
				scanned += l;
				buf += l;
				parsing = 0;
				break;
			case 'e':
				switch (size) {
				case STD_SIZE:
					n3 = va_arg(parms, double);
					break;
				case LONG_SIZE:
					n3 = va_arg(parms, long double);
					break;
					/*
					 * It seems that the compilers push a float as
					 * a double ... Need to check!
					 */
				case SHORT_SIZE:
					n3 = va_arg(parms, double);
					break;
				}
				l = ecvt(n3, buf, w, prec, flag);
				scanned += l;
				buf += l;
				parsing = 0;
				break;
			case 'g':
				switch (size) {
				case STD_SIZE:
					n3 = va_arg(parms, double);
					break;
				case LONG_SIZE:
					n3 = va_arg(parms, double);
					break;
					/*
					 * It seems that the compilers push a float as
					 * a double ... Need to check!
					 */
				case SHORT_SIZE:
					n3 = va_arg(parms, double);
					break;
				}
				l = gcvt(n3, buf, w, prec, flag);
				scanned += l;
				buf += l;
				parsing = 0;
				break;
			case 'l':
				size = LONG_SIZE;
				break;
			case 'n':
			case 'h':
				size = SHORT_SIZE;
				break;
			case '+':
				flag |= ADD_PLUS;
				break;
			case '-':
				flag |= LEFT_PAD;
				break;
			case '.':
				parsing = 2;
				flag |= RESPECT_WIDTH;
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '0':
				if (parsing == 1) {
					w = strtou(fmt, 10, &base);
					/* MG */
					/* if the first numeral is zero a ZERO pad is */
					/* required */
					/* but not if LEFT_PAD is set */
					if (*fmt != '0' || flag & LEFT_PAD)
						flag |= SPACE_PAD;
					else
						flag |= ZERO_PAD;
					fmt = base - 1;
				} else if (parsing == 2) {
					prec = strtou(fmt, 10, &base);
					fmt = base - 1;
					parsing = 1;
				}
				break;
			default:
				parsing = 0;
				break;
			}
			fmt++;
		}
	}
	*buf = 0;
	return (scanned);
}
