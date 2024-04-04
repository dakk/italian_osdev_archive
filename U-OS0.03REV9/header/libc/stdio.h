#ifndef __STDIO_H
#define __STDIO_H

void putchar(char);
void printchar(char **, int );
int prints(char **, const char *, int, int);
int printi(char **, int , int , int, int, int, int);
int print(char **, int *);
int printf(const char *format, ...);
int sprintf(char *, const char *, ...);
unsigned scanf(char *format, ...);
//void init_console(void);
//int kprintf(const char *format, ...);
int getchar();
char* gets();
#endif
