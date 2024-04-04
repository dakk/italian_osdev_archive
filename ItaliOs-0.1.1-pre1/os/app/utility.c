#include <io.h>
#include <stdio.h>
#include <delay.h>
#include <dir.h>
#include <vga.h>
#include <forme_vga.h>

extern void close_fs(void);
extern struct directory *current_dir;

void reboot()
{
	unsigned int x, y;
	printf("\nSincronizzazione dell'fs in corso.......");
	write_dir(current_dir);
	close_fs();
	printf("\nPremi un tasto ");
	init_vga();
	clear_screen(VGA_BLU_OS);
	x = get_max_x();
	y = get_max_y();
	/*rettangolo_pieno(0,0,x/3, y-50, VGA_VERDE);
	 * rettangolo_pieno(x/3,0,(x/3)*2, y-50, VGA_BIANCO);
	 * rettangolo_pieno((x/3)*2,0,x, y-50, VGA_ROSSO); */

	rettangolo_pieno(0, 0, x / 3, y - 50, VGA_VERDE);
	rettangolo_pieno(x / 3, 0, (x / 3) * 2, y - 50, VGA_BIANCO);
	rettangolo_pieno((x / 3) * 2, 0, x, y - 50, VGA_ROSSO_BELLO);
	puts_graph("ItaliOs 0.1.1-pre1", (x / 2) - 85, y - 25, VGA_GIALLO,   VGA_BLU_OS);
	getch();
	while (inportb(0x64) & 0x02);	// Riavvia il computer
	outportb(0x64, 0xfe);
	asm("cli;hlt");
	return;			/*non sar�mai eseguito ma evita un warning ^___^  */
}