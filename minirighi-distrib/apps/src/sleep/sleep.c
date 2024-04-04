/*!	\file apps/src/sleep/sleep.c
 *	\brief Delay for a specified amount of time
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-12-12\n
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROG_NAME	"sleep"

char help_msg[] =
		"Pause for NUMBER seconds.  SUFFIX may be `s' for seconds (the default),\n"
		"`m' for minutes, `h' for hours or `d' for days.  Unlike most  implemen-\n"
		"tations  that require NUMBER be an integer, here NUMBER may be an arbi-\n"
		"trary floating point number.\n\n"
		"--help display this help and exit";

int main(int argc, char **argv)
{
	char tmp[1024];

	if (argc < 1) {
		printf("%s: too few arguments\nUse `%s --help' for more informations.\n",
			PROG_NAME, PROG_NAME);
		exit(EXIT_FAILURE);
	}
	if (strncmp(argv[1], "--help", sizeof("--help")) == 0) {
		printf("Usage: %s NUMBER[SUFFIX]\n", PROG_NAME);
		printf("%s\n", help_msg);
		exit(EXIT_SUCCESS);
	}

	strncpy(tmp, argv[1], sizeof(tmp));

	if (tmp[strnlen(tmp, sizeof(tmp)) - 1] == 's') {
		tmp[strnlen(tmp, sizeof(tmp)) - 1] = '\0';
		(void)sleep(atoi(tmp));
	} else if (tmp[strnlen(tmp, sizeof(tmp)) - 1] == 'm') {
		tmp[strnlen(tmp, sizeof(tmp)) - 1] = '\0';
		(void)sleep(atoi(tmp) * 60);
	} else if (tmp[strnlen(tmp, sizeof(tmp)) - 1] == 'h') {
		tmp[strnlen(tmp, sizeof(tmp)) - 1] = '\0';
		(void)sleep(atoi(tmp) * 3600);
	} else if (tmp[strnlen(tmp, sizeof(tmp)) - 1] == 'd') {
		tmp[strnlen(tmp, sizeof(tmp)) - 1] = '\0';
		(void)sleep(atoi(tmp) * 86400);
	} else {
		(void)sleep(atoi(tmp));
	}

	return EXIT_SUCCESS;
}
