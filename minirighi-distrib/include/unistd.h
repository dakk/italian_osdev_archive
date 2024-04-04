/*!     \file include/unistd.h
 *      \brief POSIX unistd.
 *      \date Last update: 2004-12-12
 */

#ifndef UNISTD_H
#define UNISTD_H

#include <sys/sys.h>

unsigned int sleep(unsigned int seconds);
void usleep(unsigned long usec);

#endif
