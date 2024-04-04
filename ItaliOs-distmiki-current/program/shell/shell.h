#ifndef _SHELL_H_
#define  _SHELL_H_


/* Lo storico dei comandi viene fatto attraverso una lista doppia
quindi con puntatori sia in avanti che all'indietro
il comando e' mallocato dinamicamente per evitare sprechi di memoria
*/
struct command_history
{
	char *comando;
	struct command_history *back;
	struct command_history *next;
};	


void shell();

#endif
