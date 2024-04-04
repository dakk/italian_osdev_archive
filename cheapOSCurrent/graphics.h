#ifndef _GRAPHICS_
#define _GRAPHICS_

#include "config.h"
#include "kconsole.h"
#include "io.h"
#include "mem.h"

#define VIDEO_CRTC_REG_ID 0x3d4
#define VIDEO_CRTC_REG_DATA 0x3d5
#define VIDEO_TOT_COLS 0x01
#define VIDEO_TOT_ROWS_L 0x12

#define VIDEO_CURSOR_START 0x0a
#define VIDEO_CURSOR_END   0x0b
#define VIDEO_CURSOR_H     0x0e
#define VIDEO_CURSOR_L     0x0f

//Macro per le opzioni disponibili per lo schermo
#define VIDEO_OPT_RIS 0		//Usata per leggere/modificare la risoluzione (per ora solo leggere!)
#define VIDEO_OPT_CUR 1		//Usata per leggere/impostare l'altezza del cursore
#define VIDEO_OPT_BLN 2		//Usata per leggere/impostare il lampeggiamento dei caratteri.

#define VIDEO_OPT_BIGCUR 9	//Usato per impostare l'altezza del cursore a 9 pixel
#define VIDEO_OPT_SMLCUR 4	//Usato per impostare l'altezza del cursore a 4 pixel
#define VIDEO_OPT_BLNON  2	//Usato per far lampeggiare i caratteri
#define VIDEO_OPT_BLNOFF 3	//Usato per non far lampeggiare i caratteri

#define VIDEO_RES_80x25  1
#define VIDEO_RES_80x25m 2
#define VIDEO_RES_80x30  3

//Macro che identificano le funzioni extra per la memoria video
#define VIDEO_MEM_COPY   0	//Copia l'originale in una nuova locazione
#define VIDEO_MEM_MOVE   1	//Copia l'originale in una nuova locazione e cancella l'originale

#define TAB_SIZE 3

//Definisco macro per i colori
#define BLACK    0x0
#define DKBLUE   0x1
#define DKGREEN  0x2
#define DKCYAN   0x3
#define DKRED	 0x4
#define DKPURPLE 0x5
#define DKBROWN  0x6
#define LTGRAY	 0x7
#define DKGRAY   0x8
#define LTBLUE   0x9
#define LTGREEN  0xA
#define LTCYAN   0xB
#define LTRED	 0xC
#define LTPURPLE 0xD
#define LTBROWN  0xE
#define WHITE    0xF

struct Video_Caret
{
	unsigned long X;		//Ascissa del cursore
	unsigned long Y;		//Ordinata del cursore
	unsigned long posVM;		//Posizione del cursore all'interno della memoria video
};

//Imposta la risoluzione dello schermo
bool Video_getRes();

//Stampa a schermo la risoluzione (modalita' caratteri) dello schermo
bool Video_printRes();

//Abilita il lampeggiamento dei caratteri a discapito di soli 8 (invece di 16) colori di sfondo
bool Video_enableBlink();

//Disabilita il lampeggiamento dei caratteri e permette di usare 16 colori di sfondo
bool Video_disableBlink();

//Imposta le dimensioni del cursore. Due dimensioni possibili: passando 's' il cursore
//sara' grande 4 pixel, mentre passando 'b' sara' 9 pixel
bool Video_setCaretDim(char s_or_b);

//Ritorna l'altezza del cursore
int Video_getCaretDim();

//Aggiorna le coordinate del cursore
bool Video_updateCaret(unsigned long x, unsigned long y);

//Legge la posizione attuale del cursore
void Video_locateCaret();

//Seleziona la coppia (sfondo,primo piano) di colori del pennello
bool Video_setColor(unsigned char bg, unsigned char fg);

//Scrolla il video di 1 riga
void Video_scroll();

//Cancella lo schermo
void Video_clrScr();

//Stampa il carattere passato (il colore e' definito dal valore attuale del pennello)
void Video_printCh(char ch);

//Incrementa di una posizione il cursore
void Video_incrementCaret();

//Inizializza il video
bool Video_init();

//Non fa nulla, ma serve perche' e' parte dell'interfaccia comune dei driver
bool Video_shutdown();

#endif
