#ifndef _KEYBOARD_
#define _KEYBOARD_

#include "config.h"

#include "io.h"
#include "kconsole.h"
#include "isr.h"
#include "cpu.h"
#include "irq.h"
//#include "graphics.h"
//#include "ddi.h"
#include "floppy.h"
#include "keycoda.h"
#include "kbd_shortcuts.h"
#include "semaphoro.h"
//Da togliere in seguito
#include "console.h"


#define KBD_60 0x60
#define KBD_64 0x64

#define SCROLL_LOCK 1
#define NUM_LOCK    2
#define CAPS_LOCK   4

//Tasti modificatori
#define KBD_MOD_SHIFT  1
#define KBD_MOD_CTRL   2
#define KBD_MOD_ALT    4
#define KBD_MOD_MENU   8
#define KBD_MOD_WIN   16
#define KBD_MOD_ALTGR 32

//#define KBD_BUFFER_LENGHT 20
//#define KBD_ULTIMO_ELEMENTO_BUFFER KBD_BUFFER_LENGHT+1

#define KBD_OPT_LEDS 0
#define KBD_OPT_SSET 1
/*
#define KBD_OPT_BUFFER_STATUS 2
#define KBD_RES_BUFFER 3
#define KBD_WRITE_BUFFER 4
#define KBD_READ_BUFFER 5
*/
#define KBD_LAYOUT_ITA 0
#define KBD_LAYOUT_USA 1
#define KBD_LAYOUT_DVO 2

#define MAX_ASCII_CHARS 256
#define MAX_KBD_SHORTCUTS 32

/*
struct elementoBuffer
{
	char codice;		//Codice del carattere
	unsigned int next;	//Indice dell'elemento successivo. Se -1 e' l'ultimo.
	unsigned int prev;
	bool libera;		//Se true e' inutilizzata, se false contiene un elemento.
};
*/
struct variantiTasto
{
	unsigned short codice;
	char asciiChar;
} __attribute__ ((packed));

//struct ddi_interface Kbd_interface;

//L'argomento si esprime come somma dei led che si vogliono accesi
bool Kbd_setLeds(unsigned char leds);

bool Kbd_scanSet(int sset);

//Aspetta l'aknowledge (comando ricevuto) dalla tastiera
inline void Kbd_waitAck();

//Aspetta prima di scrivere dati sulla porta 60
bool Kbd_waitWrite();

bool Kbd_bufferStatus(unsigned long param);

int Kbd_getScanSet();

bool Kbd_setLayout(int layout);

int Kbd_getLayout();

bool Kbd_init();

bool Kbd_shutdown();

char Kbd_getCh(int console);

#endif
