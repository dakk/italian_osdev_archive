#ifndef _CONSOLE_
#define _CONSOLE_

#include "config.h"
#include "graphics.h"
#include "keyboard.h"
#include "fs_console.h"
#include "syscall.h"

//Caratteri con funzioni speciali
#define FRECCIA_SX 174
#define FRECCIA_DX 175
#define FRECCIA_SU 176
#define FRECCIA_GIU 177
#define CANC 24
#define HOME 30

//Caratteri per modificare i colori
#define CON_BLACK 178
#define CON_DKBLUE 179
#define CON_DKGREEN 180
#define CON_DKCYAN 181
#define CON_DKRED 182
#define CON_DKPURPLE 183
#define CON_DKBROWN 184
#define CON_WHITE_PLAIN 185
#define CON_LTGRAY 185
#define CON_BLACK_BLINK 186
#define CON_DKGRAY 186
#define CON_BLUE_BLINK 187
#define CON_LTBLUE 187
#define CON_GREEN_BLINK 188
#define CON_LTGREEN 188
#define CON_CYAN_BLINK 189
#define CON_LTCYAN 189
#define CON_RED_BLINK 190
#define CON_LTRED 190
#define CON_PURPLE_BLINK 191
#define CON_LTPURPLE 191
#define CON_BROWN_BLINK 192
#define CON_LTBROWN 192
#define CON_WHITE_BLINK 193
#define CON_WHITE 193
#define CON_REVERSE 194
#define CON_BLINK 195

/*Pulisce lo schermo*/
bool Console_clrScr(unsigned char cons);

/*Sposta il cursore in una posizione assoluta*/
bool Console_gotoxy(unsigned char x, unsigned char y, unsigned char cons);

/*Scrolla di una riga*/
bool Console_scroll(unsigned char cons);

/*Stampa un carattere nella posione di memoria della console specificata*/
bool Console_printCh(unsigned char cons, unsigned char c);

/*Incrementa il cursore di una console*/
bool Console_incrementaCursore(unsigned char cons);

/*Imposta il colore della console*/
bool Console_setColors(unsigned char cons, unsigned char bg, unsigned char fg);

/*Stampa una stringa su console*/
bool Console_printStr(unsigned char cons, const char* stringa);

/*Prende una stringa da tastiera*/
int Console_getString(unsigned int cons, unsigned char charNum, char* buffer);

/*Copia una console sulla memoria video*/
bool Console_toScreen(unsigned char cons);

/*Restituisce quale console e' attualmente presente in memoria video*/
unsigned char Console_getConsoleOnScreen();

/*Wrapper di Console_toScreen con l'argomento corretto per la console.
  Servono per quando si registra una funzione*/
void Console_1Up();
void Console_2Up();
void Console_3Up();
void Console_4Up();

void Console_updateScreen();

/*Copia la console a schermo per intero oppure lasciando 3 righe per vedere l'eventuale output
  delle funzioni*/
void Console_mostraMessaggi();

void Console_colorByChar(unsigned char c);

void Console_blink(unsigned char cons);

/*SPOSTATE IN FS_CONSOLE*/
/*Inizializza la console*/
//bool Console_init();

/*Ferma la console*/
//bool Console_shutdown();

#endif
