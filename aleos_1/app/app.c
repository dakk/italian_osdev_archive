#include <app.h>
#include <io.h>
#include <sys.h>

void app()
{
	clear_screen();
	write_str("------------------------------------------------------------\n");
	write_str("Benvenuto in AleOS!!! Questa e' un'applicazione di prova!\n");
	write_str("------------------------------------------------------------\n");
	write_newline();
	text_attr(0x03);
	write_str("Ecco un altro colore!\n\n");
	text_attr(WHITE_TXT);
	write_str("Adesso si puo' spegnere il computer\n");
	halt();
} 
