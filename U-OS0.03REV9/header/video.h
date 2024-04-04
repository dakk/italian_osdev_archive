
#ifndef __VIDEO_H
#define __VIDEO_H

#define SCR_W 80
#define SCR_H 25

void _kputc(char);
void _kputs(char *);
void _kclear(void);
void _kcolor(char);
void _knewline(void);
void _kgoto(int, int);
void _kprintDone(void);
void _ktab(void);
void _kshiftAll(void);
void _kntos(char *buffer, int num, int base);
void _kbackspace(void);
void _ksetcursor(unsigned int x, unsigned int y);
void _ksetcursauto(void);

#endif
