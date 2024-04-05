#include <io.h>
#include <stdio.h>
#include <delay.h>
#include <dir.h>

extern void close_fs(void);
extern struct directory *current_dir;

void reboot()
{
	printf("\nSincronizzazione dell'fs in corso.......");
	write_dir(current_dir);
	close_fs();
	printf("\nPremi un tasto ");
	getch();
	while (inportb(0x64) & 0x02);	// Riavvia il computer
	outportb(0x64, 0xfe);
	asm("cli;hlt");
	return;			/*non saràmai eseguito ma evita un warning ^___^  */
}
