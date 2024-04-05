#include "kbd_shortcuts.h"

unsigned char lastShortcut = 0;

/*Questa va portata fuori!!! Deve essere accessibile a tutti!!!*/

struct shortcut shortcut_table[MAX_SHORTCUTS];

/*Aggiunge uno shortcut alla tabella degli shortcut*/
unsigned char Kbd_add_shortcut(unsigned long codice, shortcut_t func)
{
	shortcut_table[lastShortcut].codice = codice;
	shortcut_table[lastShortcut].funzione = func;
	lastShortcut++;
	return lastShortcut;
}

/*Per vedere tutti gli shortcut gia' registrati*/

bool Kbd_print_shortcuts()
{
	int i;
	kprint("Sono presenti %d shortcuts.\n",lastShortcut);
	kprint("Combo\t\tfunzione\n");

	for(i=0;i<=lastShortcut;i++)
	{
		kprint("%x\t\t%x\n",shortcut_table[i].codice,shortcut_table[i].funzione);
	}
	return TRUE;
}

/*Ritorna la lunghezza (attuale) dell'array che contiene i combo*/
unsigned char Kbd_getShortcutsNum()
{
	return lastShortcut;
}

bool Kbd_findCombo(unsigned long combo)
{
	int i;
	//Faccio la scansione dell'array dei combo e se trovo una corrispondenza
	//chiamo la funzione associata

	//kprint("Mi hai passato %x (%x)\n",combo, &combo);

	for(i=0;i<Kbd_getShortcutsNum();i++)
	{
		//kprint("tabella codici[%d] %x, codice corrente %x",i,shortcut_table[i],combo);
		if(combo==shortcut_table[i].codice)
		{
			//kprint("Funzione trovata!!!\n");
			shortcut_table[i].funzione();
			return TRUE;
		}
		//kprint("Funzione non trovata\n");
	}
	return FALSE;
}

bool Kbd_shortcuts_init()
{
	int i;
	for(i=0;i<MAX_SHORTCUTS;i++)
	{
		shortcut_table[i].codice = 0;
		shortcut_table[i].funzione = 0;
	}

	return TRUE;
}
