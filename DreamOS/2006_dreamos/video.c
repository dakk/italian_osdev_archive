#include <video.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
// GLOBALS
char *VIDEO_MEM = (char *) 0xb8000,
  *VIDEO_PTR = (char *) 0xb8000,
  VIDEO_CLR = 0x7,
  VIDEO_BUFUP[SCR_BUF], // First scroll buffer
  VIDEO_BUFDOWN[SCR_BUF], // Second scroll buffer
  VIDEO_BACKUP[SCR_BUF], // Video backup
  VIDEO_BACKUP_UP[SCR_BUF], // First scroll buffer backup
  VIDEO_BACKUP_DOWN[SCR_BUF]; // Second scroll buffer backup

int _kscrolled=0, // Newlines
  _kscrollup=0, // Scrolled rows up
  _kscrolldown=0, // Scrolled rows down
  _kscrolling=0; // Scrolling?

/**
 *   Put a char
 */
void _kputc (char c)
{
  *VIDEO_PTR++ = c;
  *VIDEO_PTR++ = VIDEO_CLR;
  
  _ksetcursauto();
}

/**
 *   Put a string
 */
void _kputs (char *s)
{
  while(*s != 0) {
    if(*s == '\n') // newline
      _knewline();
    else if(*s == '\033') // color escape
      _kcolor(*(++s));
    else if(*s == '\t') //Tab escape
      _ktab();
    else
      _kputc(*s);

    s++;
  }
  _kshiftAll ();
  _ksetcursauto();
}

/**
 *   Clear video memory (clear screen)
 */
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

/**
 *   Select current color
 */
void _kcolor(char c)
{
  VIDEO_CLR = c;
}

/**
 *   Simulate a newline character
 */
void _kvbuf_scroll_up() {
  // Scroll the buffer up
  char *buf = VIDEO_BUFUP;
  memcpy(buf, buf+(SCR_W*2), SCR_BUF-(SCR_W*2));
  memcpy(buf+(SCR_BUF-(SCR_W*2)), VIDEO_MEM, SCR_W*2);
}

void _kvbuf_scroll_down() {
  // Scroll the buffer down
  char tmp[SCR_BUF];
  char *buf = VIDEO_BUFDOWN;
  memcpy(&tmp, buf, sizeof(tmp));
  memcpy(buf+(SCR_W*2), tmp, SCR_BUF-(SCR_W*2));
  memcpy(buf, VIDEO_MEM+(SCR_BUF-SCR_W*2), SCR_W*2);
}

void _krbuf_scroll_up() {
  // Scroll video up
  char tmp[SCR_BUF];
  char *buf = VIDEO_BUFUP;
  if (_kscrollup > 24) return;
  _kvbuf_scroll_down();
  memcpy(&tmp, VIDEO_MEM, sizeof(tmp));
  memcpy(VIDEO_MEM+(SCR_W*2), tmp, SCR_BUF-(SCR_W*2));
  memcpy(VIDEO_MEM, buf+(SCR_BUF-SCR_W*2), SCR_W*2);
  memcpy(&tmp, buf, sizeof(tmp));
  memcpy(buf+(SCR_W*2), tmp, SCR_BUF-(SCR_W*2));
  _kscrollup++;
  _kscrolldown++;
}

void _krbuf_scroll_down() {
  // Scroll video down
  char *buf = VIDEO_BUFDOWN;
  if (!_kscrolldown) return;
  _kvbuf_scroll_up();
  memcpy(VIDEO_MEM, VIDEO_MEM+(SCR_W*2), SCR_BUF-(SCR_W*2));
  memcpy(VIDEO_MEM+(SCR_BUF-SCR_W*2), buf, SCR_W*2);
  memcpy(buf, buf+(SCR_W*2), SCR_BUF-(SCR_W*2));
  _kscrolldown--;
  _kscrollup--;
}

/*
 * Restore the video memory.
 * Backup the video before calling this function
 */
void _krbuf_restore() {
  if (_kscrolling) {
    memcpy(VIDEO_MEM, VIDEO_BACKUP, SCR_BUF); // Restore the video
    memcpy(VIDEO_BUFUP, VIDEO_BACKUP_UP, SCR_BUF); // Restore the first scroll buffer
    memcpy(VIDEO_BUFDOWN, VIDEO_BACKUP_DOWN, SCR_BUF); // Restore the second scroll buffer
    _kscrolling = _kscrollup = _kscrolldown = 0; // I'm not scrolling
    _ksetcursor_state(CUR_ON); // Show the cursor
  }
}

/*
 * Backup the video memory.
 * It can be restore by using the above function
 */
void _krbuf_backup() {
  if (!_kscrolling) {
    memcpy(VIDEO_BACKUP, VIDEO_MEM, SCR_BUF); // Backup the video
    memcpy(VIDEO_BACKUP_UP, VIDEO_BUFUP, SCR_BUF); // Backup the first scroll buffer
    memcpy(VIDEO_BACKUP_DOWN, VIDEO_BUFDOWN, SCR_BUF); // Restore the second scroll buffer
    _kscrolling = 1; // I'm scrolling
    _ksetcursor_state(CUR_OFF); // Hide the cursor
  }
}

void _knewline(void)
{
  /*if (_kscrolled > SCR_H) _kvbuf_scroll_up(); // If the screen scrolled enough, then scroll the buffer up
  else _kscrolled++;*/
  VIDEO_PTR = VIDEO_MEM + ((((VIDEO_PTR - VIDEO_MEM) / (SCR_W * 2)) + 1) * (SCR_W * 2));
  _kshiftAll ();
  
  _ksetcursauto();
}

/**
 * Steps Forward of 7 characters.
 */
void _ktab(void)
{
    VIDEO_PTR = VIDEO_PTR + 7;
	_kshiftAll ();
   
   _ksetcursauto();
}

/**
 *   Print string [ok]
 */

void _kprintDone(void)
{
    _kputs("\t\t\t\t\t\t[\033\002 OK \033\007]\n");
	_kshiftAll ();
   
}

/**
 *   Goto position x,y on video memory
 *   x and y are zero-based
 */
void _kgoto(int x, int y)
{
  VIDEO_PTR = VIDEO_MEM + ((y * SCR_W * 2) + (x * 2));
  _ksetcursauto();
}

/**
 *   Simula la pressione del tasto backspace
 */
void _kbackspace(void)
{
  VIDEO_PTR -= 2;
  *VIDEO_PTR = 0x20;
  
  _ksetcursauto();
}

/**
 *   When the pointer reaches the last line
 *   the whole screen shifts up one line
 */
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

/*
 * Get the current cursor state
 */
unsigned char _kgetcursor_state() {
  unsigned char cursor;
  asm(
      "mov $0x03d4,%%dx\n"
      "mov $0x0a,%%al\n"
      "out %%al,%%dx\n"
      "inc %%dx\n"
      "in %%dx,%%al\n"
      "mov %%al,%0\n"
      : "=g"(cursor)
      );
  return cursor;
}

/*
 * Set the cursor state.
 * Read video.h to see what states can be set
 */
void _ksetcursor_state(enum curstate state) {
  unsigned char cursor = _kgetcursor_state();
  switch(state) {
  case CUR_ON:
    cursor &= 0x0df;
    break;
  case CUR_OFF:
    cursor |= 0x20;
    break;
  default:
    return;
  }
  asm(
      "mov $0x03d5,%%dx\n"
      "mov %0,%%al\n"
      "out %%al,%%dx\n"
      : : "g"(cursor)
      );
}

void _ksetcursauto()
{
   _ksetcursor( ((VIDEO_PTR - VIDEO_MEM) / 2) / SCR_W,
                ((VIDEO_PTR - VIDEO_MEM) / 2) % SCR_W );
}
