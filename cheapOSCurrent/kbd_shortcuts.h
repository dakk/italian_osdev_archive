#ifndef _KBD_SHORTCUTS_
#define _KBD_SHORTCUTS_

#define MAX_SHORTCUTS 20

#include "config.h"
#include "graphics.h"

typedef void (*shortcut_t)(void);

struct shortcut
{
	unsigned long codice;
	shortcut_t funzione;
};

unsigned char Kbd_add_shortcut(unsigned long codice, shortcut_t func);

bool Kbd_print_shortcuts();

int Kbd_getShotrcutsNum();

bool Kbd_findCombo(unsigned long combo);

bool Kbd_shortcuts_init();

#endif
