/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Kenneth Almquist.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. <BSD Advertising Clause omitted per the July 22, 1999 licensing change
 *		ftp://ftp.cs.berkeley.edu/pub/4bsd/README.Impt.License.Change>
 *
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)echo.c	8.1 (Berkeley) 5/31/93
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

char process_escape_sequence(const char **ptr)
{
	static const char charmap[] = {
		'a',  'b',  'f',  'n',  'r',  't',  'v',  '\\', 0,
		'\a', '\b', '\f', '\n', '\r', '\t', '\v', '\\', '\\' };

	const char *p;
	const char *q;
	int num_digits;
	unsigned int n;

	n = 0;
	q = *ptr;

	for ( num_digits = 0 ; num_digits < 3 ; ++num_digits) {
		if ((*q < '0') || (*q > '7')) { /* not a digit? */
			break;
		}
		n = n * 8 + (*q++ - '0');
	}

	if (num_digits == 0) {	/* mnemonic escape sequence? */
		for (p=charmap ; *p ; p++) {
			if (*p == *q) {
				q++;
				break;
			}
		}
		n = *(p+(sizeof(charmap)/2));
	}

	   /* doesn't hurt to fall through to here from mnemonic case */
	if (n > UCHAR_MAX) {	/* is octal code too big for a char? */
		n /= 8;			/* adjust value and */
		--q;				/* back up one char */
	}

	*ptr = q;
	return (char) n;
}

int main(int argc, char** argv)
{
	int nflag = 0;
	int eflag = 0;

	/* Skip argv[0]. */
	argc--;
	argv++;

	while (argc > 0 && *argv[0] == '-')
	{
		register char *temp;
		register int ix;

		/*
		 * If it appears that we are handling options, then make sure
		 * that all of the options specified are actually valid.
		 * Otherwise, the string should just be echoed.
		 */
		temp = argv[0] + 1;

		for (ix = 0; temp[ix]; ix++)
		{
			if (strrchr("neE", temp[ix]) == 0)
				goto just_echo;
		}

		if (!*temp)
			goto just_echo;

		/*
		 * All of the options in temp are valid options to echo.
		 * Handle them.
		 */
		while (*temp)
		{
			if (*temp == 'n')
				nflag = 1;
			else if (*temp == 'e')
				eflag = 1;
			else if (*temp == 'E')
				eflag = 0;
			else
				goto just_echo;

			temp++;
		}
		argc--;
		argv++;
	}

just_echo:
	while (argc > 0) {
		const char *arg = argv[0];
		register int c;

		while ((c = *arg++)) {

			/* Check for escape sequence. */
			if (c == '\\' && eflag && *arg) {
				if (*arg == 'c') {
					/* '\c' means cancel newline. */
					nflag = 1;
					arg++;
					continue;
				} else {
					c = process_escape_sequence(&arg);
				}
			}

			putchar(c);
		}
		argc--;
		argv++;
		if (argc > 0)
			putchar(' ');
	}
	if (!nflag)
		putchar('\n');

	return EXIT_SUCCESS;
}
