/*!	\file apps/src/fork/fork.c
 *	\brief Fork system call test.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-12-08\n
 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/sys.h>

#define BUF_SIZE 100

int data[BUF_SIZE];

int main(int argc, char **argv)
{
	pid_t pid;
	int i;

	printf("This is a simple test for the fork() system call...\n");
	pid = fork();
	if (!pid) {
		// The body of the child.
		printf("I am the child! fork() returns: %u\n", pid);
		for(i = 0; i < sizeof(data); i++) {
			data[i] = i;
		}
		printf("data[0] = %i data[1] = %i data[2] = %i ... data[n] = %i\n",
			data[0], data[1], data[2], data[sizeof(data) - 1]);
		// Child has done.
		return EXIT_SUCCESS;
	}
	// The body of the father.
	printf("I am the father. fork() returns: %u\n", pid);
	for (i = 0; i < sizeof(data); i++) {
		data[i] = sizeof(data) - i;
	}
	printf("data[0] = %i data[1] = %i data[2] = %i ... data[n] = %i\n",
		data[0], data[1], data[2], data[sizeof(data) - 1]);
	// Father has done.
	return EXIT_SUCCESS;
}
