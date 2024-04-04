#include <video.h>
char *VIDEO_MEM = (char *) 0xb8000,
     *VIDEO_PTR = (char *) 0xb8000,
      VIDEO_CLR = 0x7;

void _kputc (char c)
{
  *VIDEO_PTR++ = c;
  *VIDEO_PTR++ = VIDEO_CLR;
  
  _ksetcursauto();
}

void _kputs (char *s)
{
  while(*s != 0) {
    if(*s == '\n')
      _knewline();
    else if(*s == '\033')
      _kcolor(*(++s));
    else if(*s == '\t')
      _ktab();
    else
      _kputc(*s);

    s++;
  }
  _kshiftAll ();
  _ksetcursauto();
}

void _kclear(void)
{
  int i;

  VIDEO_PTR = VIDEO_MEM;

  i=0;
  while(i < SCR_H * SCR_W *2) {
    *VIDEO_PTR++ = ' ';
    *VIDEO_PTR++ = 0x7;
    i+=2;
  }

  VIDEO_PTR = VIDEO_MEM;
  
  _ksetcursauto();
}

void _kcolor(char c)
{
  VIDEO_CLR = c;
}

void _knewline(void)
{
  VIDEO_PTR = VIDEO_MEM + ((((VIDEO_PTR - VIDEO_MEM) / (SCR_W * 2)) + 1) * (SCR_W * 2));
  _kshiftAll ();
  
  _ksetcursauto();
}

void _ktab(void)
{
    VIDEO_PTR = VIDEO_PTR + 7;
	_kshiftAll ();
   
   _ksetcursauto();
}

void _kprintDone(void)
{
    _kputs("\t\t\t\t\t\t[\033\002 OK \033\007]\n");
	_kshiftAll ();
   
}

void _kgoto(int x, int y)
{
  VIDEO_PTR = VIDEO_MEM + ((y * SCR_W * 2) + (x * 2));
  _ksetcursauto();
}

void _kbackspace(void)
{
  VIDEO_PTR -= 2;
  *VIDEO_PTR = 0x20;
  
  _ksetcursauto();
}

void _kshiftAll (void)
{
  char *i;
  if (VIDEO_PTR >= VIDEO_MEM + ((SCR_H) * SCR_W * 2))
    {
    for (i=VIDEO_MEM ; i<= (VIDEO_MEM + ((SCR_H) * SCR_W * 2) + (SCR_W * 2));i++)
      *i = i[SCR_W * 2];
    VIDEO_PTR = VIDEO_MEM + ((((VIDEO_PTR - VIDEO_MEM) / (SCR_W * 2)) - 1) * (SCR_W * 2));
    }
}

void _ksetcursor(unsigned int x, unsigned int y)
{
   asm(
       "movl  %0, %%eax   \n"
       "movl  %1, %%ebx   \n"
       "movl  $0x50, %%ecx   \n"
       "mul   %%ecx         \n"
       "addl  %%ebx, %%eax   \n"
       "movw  $0x03d4, %%dx  \n"
       "pushl %%eax         \n"
       "movb  $0x0f, %%al    \n"
       "out   %%al, %%dx     \n"
       "popl  %%eax         \n"
       "movw  $0x03d5, %%dx  \n"
       "out   %%al, %%dx     \n"
       "shr   $0x08,%%eax      \n"
       "pushl %%eax         \n"
       "movw  $0x03d4, %%dx  \n"
       "movb  $0x0e, %%al    \n"
       "out   %%al, %%dx     \n"
       "pop   %%eax         \n"
       "movw  $0x03d5, %%dx  \n"
       "out   %%al, %%dx     \n"
      :
      : "g" (x), "g" (y)
      );
}

void _ksetcursauto()
{
   _ksetcursor( ((VIDEO_PTR - VIDEO_MEM) / 2) / SCR_W,
                ((VIDEO_PTR - VIDEO_MEM) / 2) % SCR_W );
}
