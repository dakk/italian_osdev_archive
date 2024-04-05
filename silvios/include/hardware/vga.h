#ifndef _VGA_H_
#define _VGA_H_


// VGA register port addresses
#define ATTRCON_ADDR             0x03C0

#define ATTRCON_DATA             0x03C1
#define MISC_ADDR                0x03C2
#define VGAENABLE_ADDR           0x03C3
#define SEQ_ADDR                 0x03C4
	#define RESETREG         0x00
	#define CLOCKMODEREG     0x01
	#define MAPMASKREG       0x02
	#define CHARMAPSELREG    0x03
	#define SEQMEMMODEREG    0x04
#define SEQ_DATA                 0x03C5
#define GRACON_ADDR              0x03CE
	#define SETRESETREG      0x00
	#define ENSETRESETREG    0x01
	#define COLORCOMPAREREG  0x02
	#define DATAROTATEREG    0x03
	#define READMAPSELREG    0x04
	#define GRAPHMODEREG     0x05
	#define MISCGRAPHREG     0x06
	#define CDCREG           0x07
	#define BITMASKREG       0x08
#define GRACON_DATA              0x03CF
#define CRTC_ADDR                0x03D4
#define STATUS_ADDR              0x03DA

#define DELAY() for(k=0;k<50;k++) __asm__("nop")

typedef struct {		/* Questa struttura ci serve per definire */
    char r, g, b;		/* la nostra palette.                     */
} TYPE_PALETTE;


typedef struct
{
	unsigned short int heigth;
	unsigned short int width;
	unsigned short int depth;
}risoluzione;

int testvga(void);
void SetMode(unsigned char *registers);
void setpal(int color, char r, char g, char b);
void set_palette(TYPE_PALETTE * palette);
void setpalette4();
void setpalette256();
void putpixel(int x, int y, int color);
void PutLine (int x1, int y1, int x2, int y2, char color);
void drawCircle(int mX, int mY, int radius, int c);
void clear(unsigned char color);
void init_vga(void);
void init_text(void);//non funziona
int test_vga(void);
#endif
