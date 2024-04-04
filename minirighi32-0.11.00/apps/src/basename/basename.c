/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const char usage_string[] = "NAME [SUFFIX]";

void show_usage(char *name)
{
	const char *format_string = "Usage: %s %s\n";
	printf(format_string,
			name, usage_string);

	exit(EXIT_FAILURE);
}

/* Set to 1 if you want basename() behavior for NULL or "". */
/* WARNING!!! Doing so will break basename applet at least! */
#define EMULATE_BASENAME	0

char *get_last_path_component(char *path)
{
#if EMULATE_BASENAME
	static const char null_or_empty[] = ".";
#endif
	char *first = path;
	char *last;

#if EMULATE_BASENAME
	if (!path || !*path) {
		return (char *) null_or_empty;
	}
#endif

	last = path - 1;

	while (*path) {
		if ((*path != '/') && (path > ++last)) {
			last = first = path;
		}
		++path;
	}

	if (*first == '/') {
		last = first;
	}
	last[1] = 0;

	return first;
}

int main(int argc, char **argv)
{
	int m, n;
	char *s;

	if ((argc < 2) || (**(argv + 1) == '-')) {
		show_usage(argv[0]);
	}

	argv++;

	s = get_last_path_component(*argv);

	if (argc>2) {
		argv++;
		n = strlen(*argv);
		m = strlen(s);
		if (m>n && strncmp(s+m-n, *argv, n)==0)
			s[m-n] = '\0';
	}
	printf("%s\n", s);
	return EXIT_SUCCESS;
}
