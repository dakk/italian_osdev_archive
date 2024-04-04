#include <video.h>
#include <stdio.h>
#include <semaphore.h>
#include <keyboard.h>
#include <mydef.h>

sem_t *sem_print;

void printchar(char **str, int c)
{
	if (str) {
		**str = c;
		++(*str);
	}
	else (void)putchar(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

int prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		printchar (out, padchar);
		++pc;
	}

	return pc;
}

#define PRINT_BUF_LEN 12

int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + prints (out, s, width, pad);
}

int print(char **out, int *varg)
{
	register int width, pad;
	register int pc = 0;
	register char *format = (char *)(*varg++);
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = *((char **)varg++);
				pc += prints (out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += printi (out, *varg++, 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += printi (out, *varg++, 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X' ) {
				pc += printi (out, *varg++, 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += printi (out, *varg++, 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				scr[0] = *varg++;
				scr[1] = '\0';
				pc += prints (out, scr, width, pad);
				continue;
			}
		}
		else {
		out:
			printchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	return pc;
}


int printf(const char *format, ...)
{
	register int *varg = (int *)(&format);
	return print(0, varg);
}

int kprintf(const char *format, ...)
{
	register int *varg = (int *)(&format);
	return print(0, varg);
}
int sprintf(char *out, const char *format, ...)
{
	register int *varg = (int *)(&format);
	return print(&out, varg);
}

void putchar (char ch)
{
    if(ch=='\n') _knewline();
    else _kputc(ch);
}

unsigned scanf(char *format, ...)
{
    int car=NULL;
    int flag = TRUE;
    unsigned n=0;
    while(flag==TRUE)
    {
        car=_kgetch();
        if(car!=-1 && car!=13){
            *(format+n)=car;
            n++;
        }
        else flag=FALSE;
    }
    return n;
}

 int getchar()
{
	int tmpchar;
	do{
		tmpchar = _kgetch();
	}while(tmpchar == -1);

	return tmpchar;
}

char* gets()
{
	unsigned int cchar;
	int tmpchar;
        char *str;
        cchar=0;
	do{
		tmpchar  = getchar();
                if(tmpchar!=13) putchar(tmpchar);
		if (tmpchar == 8 && cchar != 0){
			cchar--;
		}else if (tmpchar != 8 && tmpchar != 13){
			str[cchar] = tmpchar;
		}

		cchar++;
	}while (tmpchar != 13);

	str[cchar] = '\0';
return str;
}
