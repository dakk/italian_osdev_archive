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

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <sprintf.h>
#include <math.h>
#include <float.h>
#include <limits.h>


static unsigned long long next = 0;

int rand(void)
{
	next = next * 1103515245L + 12345;
	next = (next << 15) ^ (next >> 27);
	return (int) ((next >> 4) & RAND_MAX);
}

void srand(unsigned int seed)
{
	next = seed;
}


unsigned abs(int x)
{
	if (x < 0)
		return (-x);
	else
		return (x);
}


int max(int a, int b)
{
	if (a > b)
		return a;
	else
		return b;
}

int min(int a, int b)
{
	if (a < b)
		return a;
	else
		return b;
}

unsigned ecvt(double v, char *buffer, int width, int prec, int flag)
{
	register int len = 0;
	long int exponent, abs_exponent;
	/* Manage Inf & NaN */
	if (isspecial(v, buffer))
		return (strlen(buffer));
	/* Check for negative value & add optional + sign */
	if (v < 0) {
		*buffer++ = '-';
		v = -v;
		len++;
	} else if (flag & ADD_PLUS) {
		*buffer++ = '+';
		len++;
	}
	/* Zero check! */
	if (v < DBL_MIN) {
		*buffer++ = '0';
		*buffer = 0;
		return (1);
	}
	/* Evaluate the exponent */
	if (v < 1) {
		exponent = 0;
		while (v < 1.0) {
			v *= 10.0;
			exponent--;
		}
	} else {
		exponent = 0;
		while (v >= 10.0) {
			v /= 10.0;
			exponent++;
		}
	}
	abs_exponent = abs(exponent);
	if (abs_exponent > 99)
		width -= 3;
	else if (abs_exponent > 9)
		width -= 2;
	else if (exponent > 0)
		width -= 1;
	/* Now the number as IP in range [0,1] */
	/* Convert this as a fixed point format */
	len += fcvt(v, buffer, width, prec, flag | RESPECT_WIDTH);
	/* Now convert the exponent */
	if (exponent == 0)
		return (len);
	buffer += len;
	*buffer++ = 'e';
	len += dcvt(exponent, buffer, 10, exponent / 10 + 2, ADD_PLUS) + 1;
	return (len);
}


unsigned fcvt(double v, char *buffer, int width, int prec, int flag)
{
	double ip, fp, val;
	register int l = 0;
	register int digits;
	int conv, got_a_digit, exceeded, len = 0;
	char tmp[300];
	memset(tmp, 0, 300);
	/* Manage Inf & NaN */
	if (isspecial(v, buffer))
		return (strlen(buffer));
	/* Check for negative value & add optional + sign */
	if (v < 0) {
		*buffer++ = '-';
		v = -v;
		len++;
	} else if (flag & ADD_PLUS) {
		*buffer++ = '+';
		len++;
	}
	/* Extract integer part & mantissa */
	fp = modf(v, &ip);
	/* Process integer part */
	digits = 0;
	if (ip >= 1.0) {
		while (ip >= 1.0) {
			/* The 0.01 here is used to correct rounding errors */
			/* Ex: 1.2 --> 1.1999999999999... adjust to 1.2 */
			/*
			 * ip = ip / 10.0;
			 * modf(ip,&val);
			 * val = (ip - val)*10.0;
			 */
			val = fmod(ip, 10.0);
			ip = ip / 10.0;
			tmp[digits++] = todigit((unsigned) (val));
		}
		len += digits;
		/* Now reverse the temporary buffer into output buffer  */
		/* Translate only the last 15 digits                    */
		/* The others are beyond double precision limit!        */
		for (l = digits - 1; l >= max(digits - 15, 0); l--)
			*buffer++ = tmp[l];
		if (l >= 0)
			for (l = digits - 16; l >= 0; l--)
				*buffer++ = '0';
		*buffer = 0;
		/* If IP == 0 -> just put in a 0 */
	} else {
		*buffer++ = '0';
		len++;
	}
	/* Process fractionary part according to width specification */
	/* If RESPECT_WIDTH is set, scan until you reach wanted precision */
	/* Else scan until you find a not 0 digit */
	if (fp > 1e-307 && len < width + 1) {
		*buffer++ = '.';
		len++;
		if (flag & RESPECT_WIDTH)
			got_a_digit = 1;
		else
			got_a_digit = 0;
		exceeded = 0;
		digits = 1;
		while (!exceeded) {
			/* The DBL_EPSILON here is used to correct rounding errors */
			fp = (fp + DBL_EPSILON) * 10.0;
			fp = modf(fp, &ip);
			conv = (int) (ip);
			if (conv != 0 && !got_a_digit)
				got_a_digit = 1;
			*buffer++ = todigit(conv);
			len++;
			digits++;
			if (got_a_digit && (digits > prec))
				exceeded = 1;
			if (width < len)
				exceeded = 1;
		}
		/* Drop trailing zeros after decimal point */
		while (*--buffer == '0' && *(buffer - 1) != '.')
			len--;
		buffer++;
	}
	*buffer = 0;
	return (len);
}




unsigned gcvt(double v, char *buffer, int width, int prec, int flag)
{
	double v_abs;
	int exponent;
	/* Manage Inf & NaN */
	if (isspecial(v, buffer))
		return (strlen(buffer));
	/* Extract absolute value */
	if (v < 0.0)
		v_abs = -v;
	else
		v_abs = v;
	/* Zero check! */
	if (v_abs < DBL_MIN) {
		*buffer++ = '0';
		*buffer = 0;
		return (1);
	}
	exponent = 0;
	/* Evaluate exponent */
	if (v_abs < 1.0) {
		while (v_abs < 1) {
			v_abs *= 10.0;
			exponent--;
		}
	} else {
		while (v_abs >= 10.0) {
			v_abs /= 10.0;
			exponent++;
		}
	}
	/* Choose shortest format on exponent value */
	if (exponent > 7 || exponent < -7)
		return (ecvt(v, buffer, width, prec, flag));
	else
		return (fcvt(v, buffer, width, prec, flag));
}



unsigned ucvt(unsigned long v, char *buffer, int base, int width, int flag)
{
	register int i = 0, j;
	unsigned ret = 0, abs_base;
	unsigned long abs_v;
	char tmp[12];
	/* Extract the less significant digit */
	/* Put it into temporary buffer       */
	/* It has to be local to have         */
	/* reentrant functions                */
	/*
	 * MG: fix to handle zero correctly
	 * if (v == 0) {
	 * *buffer++ = '0';
	 * *buffer = 0;
	 * return(1);
	 * }
	 */

	/* MG: is this required? */
	/* the vsprintf() function seems to call this procedure with */
	/* this flag inverted */
	flag ^= LEFT_PAD;

	abs_base = (base >= 0) ? base : -base;
	if (base < 0)
		abs_v = ((long) (v) < 0) ? -v : v;
	else
		abs_v = v;
	/* Process left space-padding flags */
	if (flag & ADD_PLUS || ((base < 0) && ((long) (v) < 0))) {
		ret++;
	}
	/* MG: fix to handle zero correctly */
	if (v == 0)
		tmp[i++] = '0';
	else
		while (abs_v > 0) {
			tmp[i++] = todigit(abs_v % abs_base);
			abs_v = abs_v / abs_base;
		}
	ret += i;
	if ((flag & LEFT_PAD) && (flag & SPACE_PAD)) {
		j = ret;
		while (j < width) {
			*buffer++ = ' ';
			ret++;
			j++;
		}
	}
	/* Add plus if wanted */
	if (base < 0) {
		if (((long) (v)) < 0)
			*buffer++ = '-';
		else if (flag & ADD_PLUS)
			*buffer++ = '+';
	} else if (flag & ADD_PLUS)
		*buffer++ = '+';
	/* Process left zero-padding flags */
	if ((flag & LEFT_PAD) && (flag & ZERO_PAD)) {
		j = ret;
		while (j < width) {
			*buffer++ = '0';
			ret++;
			j++;
		}
	}
	/* Reverse temporary buffer into output buffer */
	/* If wanted left pad with zero/space; anyway only one at once is ok */
	for (j = i - 1; j >= 0; j--)
		*buffer++ = tmp[j];
	if ((flag & (SPACE_PAD)) && !(flag & LEFT_PAD)) {
		/* If wanted, pad with space to specified width */
		j = ret;
		while (j < width) {
			*buffer++ = ' ';
			ret++;
			j++;
		}
	}
	/* Null terminate the output buffer */
	*buffer = 0;
	return (ret);
}

unsigned dcvt(long v, char *buffer, int base, int width, int flag)
{
	return (ucvt((unsigned long) (v), buffer, -base, width, flag));
}



double strtod(char *s, char **scan_end)
{
	int sign, i;
	double result = 0;
	double value;
	double mantissa = 0, divisor = 1;
	unsigned short power = 0;
	/* Evaluate sign */
	if (*s == '-') {
		sign = -1;
		s++;
	} else
		sign = 1;
	/* Skip trailing zeros */
	while (*s == '0')
		s++;
	/* Convert integer part */
	while (*s <= '9' && *s >= '0') {
		value = *s++ - '0';
		result *= 10.0;
		result += value;
	}
	/* Detect floating point & mantissa */
	if (*s == '.') {
		s++;
		while (*s <= '9' && *s >= '0') {
			value = *s++ - '0';
			mantissa *= 10.0;
			mantissa += value;
			divisor *= 10.0;
		}
	}
	mantissa /= divisor;
	/* Adjust result */
	result += mantissa;
	/* Adjust sign */
	result *= sign;
	/* Detect exponent */
	if (*s == 'e' || *s == 'E') {
		s++;
		if (*s == '-') {
			sign = -1;
			s++;
		} else if (*s == '+') {
			sign = 1;
			s++;
		} else
			sign = 1;
		while (*s <= '9' && *s >= '0') {
			value = *s++ - '0';
			power *= 10.0;
			power += value;
		}
	}
	/* Adjust result on exponent sign */
	if (sign > 0)
		for (i = 0; i < power; i++)
			result *= 10.0;
	else
		for (i = 0; i < power; i++)
			result /= 10.0;
	if (scan_end != 0L)
		*scan_end = s;
	return (result);
}



long int strtoi(char *s, int base, char **scan_end)
{
	int sign, value, overflow = 0;
	long int result = 0, oldresult;
	/* Evaluate sign */
	if (*s == '-') {
		sign = -1;
		s++;
	} else if (*s == '+') {
		sign = 1;
		s++;
	} else
		sign = 1;
	/* Skip trailing zeros */
	while (*s == '0')
		s++;
	/* Convert number */
	while (isnumber(*s, base)) {
		value = tonumber(*s++);
		if (value > base || value < 0)
			return (0);
		oldresult = result;
		result *= base;
		result += value;
		/* Detect overflow */
		if (oldresult > result)
			overflow = 1;
	}
	if (scan_end != 0L)
		*scan_end = s;
	if (overflow)
		result = INT_MAX;
	result *= sign;
	return (result);
}


long strtol(const char *nptr, char **endptr, int base)
{
	register const char *s = nptr;
	register unsigned long acc;
	register unsigned char c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? -(unsigned long) LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long) base;
	cutoff /= (unsigned long) base;
	for (acc = 0, any = 0;; c = *s++) {
/*		if (!isascii(c))	*/
		if (!isalnum(c))
			break;
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = neg ? LONG_MIN : LONG_MAX;
	} else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);
	return (acc);
}


long unsigned strtou(char *s, int base, char **scan_end)
{
	int value, overflow = 0;
	long unsigned result = 0, oldresult;
	/* Skip trailing zeros */
	while (*s == '0')
		s++;
	if (*s == 'x' && base == 16) {
		s++;
		while (*s == '0')
			s++;
	}
	/* Convert number */
	while (isnumber(*s, base)) {
		value = tonumber(*s++);
		if (value > base || value < 0)
			return (0);
		oldresult = result;
		result *= base;
		result += value;
		/* Detect overflow */
		if (oldresult > result)
			overflow = 1;
	}
	if (scan_end != 0L)
		*scan_end = s;
	if (overflow)
		result = INT_MAX;
	return (result);
}


unsigned long strtoul(const char *nptr, char **endptr, int base)
{
	register const char *s = nptr;
	register unsigned long acc;
	register unsigned char c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * See strtol for comments as to the logic used.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;
	cutoff = (unsigned long) ULONG_MAX / (unsigned long) base;
	cutlim = (unsigned long) ULONG_MAX % (unsigned long) base;
	for (acc = 0, any = 0;; c = *s++) {
/*		if (!isascii(c))*/
		if (!isalnum(c))
			break;
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = ULONG_MAX;
	} else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);
	return (acc);
}
