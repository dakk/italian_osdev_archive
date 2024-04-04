/*!	\file include/stdio.h
 *	\brief POSIX standard I/O.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-01-27
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <sys/sys.h>

//! Define a standard length for put a limit for
//! dangerous functions like gets(), scanf(), etc...
#define STR_MAX_LENGTH	1024

#ifndef EOF
//! End of file.
#define EOF	-1
#endif

extern int getchar(void);
extern int putchar(int c);

extern int printf (const char *__restrict __format, ...);
extern int puts(const char *__s);

extern int snprintf(char *__restrict __s, size_t __maxlen, const char *__restrict __format, ...);
extern int vsnprintf(char *__restrict __s, size_t __maxlen, const char *__restrict __format, va_list __arg);

extern int sprintf(char * buf, const char *fmt, ...);

extern char *getline(char *__s, int __size);
extern char *gets(char *__s);

extern int scanf (const char *__restrict __format, ...);
extern int sscanf(const char *__restrict __buf, const char *__restrict __fmt, ...);
extern int vsscanf(const char *__restrict __s, const char *__restrict __format, va_list __arg);

#endif
