/*!	\file apps/src/usleep/usleep.c
 *	\brief Sleep for some number of microseconds.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-12-12\n
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROG_NAME	"usleep"

char help_msg[] =
		"sleeps some number of microseconds. The default is 1.\n"
		"--help display this help and exit";

int main(int argc, char **argv)
{
	char tmp[1024];
	long usec;

	if (argc < 1) {
		usec = 1;
	} else if (strncmp(argv[1], "--help", sizeof("--help")) == 0) {
		printf("Usage: %s [number]\n", PROG_NAME);
		printf("%s\n", help_msg);
		exit(EXIT_SUCCESS);
	}

	strncpy(tmp, argv[1], sizeof(tmp));
	usleep(atoi(tmp));

	return EXIT_SUCCESS;
}
