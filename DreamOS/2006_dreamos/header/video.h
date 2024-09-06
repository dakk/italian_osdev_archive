/*
 * Video.h
 * Libreria per la gestione dell'output su schermo
 * Autore: Sgurtz
 */

#ifndef __VIDEO_H
#define __VIDEO_H

#define SCR_W 80
#define SCR_H 25
#define SCR_BUF (SCR_W*2*SCR_H)

// BUILTINS VIDEO FUNCTIONS

enum {
  COL_BLACK,
  COL_BLUE,
  COL_GREEN,
  COL_CYAN,
  COL_RED,
  COL_VIOLET,
  COL_BROWN,
  COL_WHITE_L,
  COL_GREY,
  COL_BLUE_L,
  COL_GREEN_L,
  COL_CYAN_L,
  COL_ORANGE,
  COL_RED_L,
  COL_YELLOW,
  COL_WHITE,
  MAX_COLOR = 15,
  INTERMIT = 128
};

enum curstate {
  CUR_OFF,
  CUR_ON,
};

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
void _krbuf_scroll_up(void);
void _krbuf_scroll_down(void);
void _krbuf_restore(void);
void _krbuf_backup(void);
unsigned char _kgetcursor_state(); // Get the cursor state
void _ksetcursor_state(enum curstate state); // Set the cursor state

#endif
