
#include "keyboard.h"
//Queste sono le combinazioni di o un solo tasto o al massimo tasto+shift.
//Per le funzioni speciali tipo CTRL+TASTO ALT+TASTO CTRL+ALT+TASTO si usa un array a parte!!!


struct variantiTasto Kbd_layout[MAX_ASCII_CHARS]=
{
	//Uso i caratteri ascii in ordine (ho la tabella davanti!!!) e gli associo i tasti
	//della tastiera.

	//ALCUNI DEI CARATTERI ASCII NON ASSOCIATI A NESSUN TASTO LI
	//UTILIZZO PER MAPPARE IL TASTIERINO NUMERICO

/*----------------------------------------------------------*/
	{0x0000,0},	//Null [NON ASSOCIATO A NESSUN TASTO]
	{0x0000,1},	//Start of heading [NON ASSOCIATO A NESSUN TASTO]
	{0x007B,2},	//Start of text [ASSOCIATO A INS]
	{0x007A,3},	//End of text [ASSOCIATO A END]
	{0x0000,4},	//End of transmission [NON ASSOCIATO A NESSUN TASTO]
	{0x0000,5},	//Enquiry [NON ASSOCIATO A NESSUN TASTO]
	{0x0000,6},	//Acknowledge
	{0x0000,7},	//Bell [NON ASSOCIATO A NESSUN TASTO]
	{0x000E,'\b'},	//Backspace
	{0x000F,'\t'},	//Tab orizzontale
	{0x000,10},	//Newline [NON ASSOCIATO A NESSUN TASTO]
	{0x0000,11},	//Vertical tab [NON ASSOCIATO A NESSUN TASTO]
	{0x0000,12},	//Form feed [NON ASSOCIATO A NESSUN TASTO]
	{0x001C,'\n'},	//Carriage return
	{0x0000,14},	//Shift out [NON ASSOCIATO A NESSUN TASTO]
	{0x0000,15},	//Shift in [NON ASSOCIATO A NESSUN TASTO]
	{0x0000,16},	//Data link escape [NON ASSOCIATO A NESSUN TASTO]
	{0x0000,17},	//Freccetta sinistra
	{0x0000,18},	//Device control 2 [NON ASSOCIATO A NESSUN TASTO]
	{0x0000,19},	//Device control 3 [NON ASSOCIATO A NESSUN TASTO]
	{0x0000,20},
	{0x0000,21},	//
	{0x0000,22},	//Syncronous idle [NON ASSOCIATO A NESSUN TASTO]
	{0x0000,23},	//End of transition block [NON ASSOCIATO A NESSUN TASTO]
	{0x0079,24},	//Cancel
	{0x0000,25},	//
	{0x0000,26},	//Substitute [NON ASSOCIATO A NESSUN TASTO]
	{0x0064,27},	//Esc
	{0x0000,28},	//Freccetta in basso
	{0x0000,29},	//Group separator [NON ASSOCIATO A NESSUN TASTO]
	{0x007F,30},	//Home
	{0x0000,31},	//Unit separator [NON ASSOCIATO A NESSUN TASTO]
	{0x0037,'+'},	//+ del tastierino numerico
	{0x0039,' '},	//Spazio
	{0x0102,'!'},
	{0x0128,'"'},
	{0x0104,'#'},	//Altgr+tasto
	{0x0105,'$'},
	{0x0106,'%'},
	{0x0108,'&'},
	{0x0075,'\''},
	{0x010A,'('},
	{0x010B,')'},
	{0x0109,'*'},
	{0x010D,'+'},
	{0x0033,','},
	{0x000C,'-'},
	{0x0034,'.'},
	{0x0035,'/'},
	{0x000B,'0'},
	{0x0002,'1'},
	{0x0003,'2'},
	{0x0004,'3'},
	{0x0005,'4'},
	{0x0006,'5'},
	{0x0007,'6'},
	{0x0008,'7'},
	{0x0009,'8'},
	{0x000A,'9'},
	{0x0127,':'},
	{0x0027,';'},
	{0x0133,'<'},
	{0x000D,'='},
	{0x0134,'>'},
	{0x0135,'?'},
	{0x0103,'@'},
	{0x011E,'A'},
	{0x0130,'B'},
	{0x012E,'C'},
	{0x0120,'D'},
	{0x0112,'E'},
	{0x0121,'F'},
	{0x0122,'G'},
	{0x0123,'H'},
	{0x0117,'I'},
	{0x0124,'J'},
	{0x0125,'K'},
	{0x0126,'L'},
	{0x0132,'M'},
	{0x0131,'N'},
	{0x0118,'O'},
	{0x0119,'P'},
	{0x0110,'Q'},
	{0x0113,'R'},
	{0x011F,'S'},
	{0x0114,'T'},
	{0x0116,'U'},
	{0x012F,'V'},
	{0x0111,'W'},
	{0x012D,'X'},
	{0x0115,'Y'},
	{0x012C,'Z'},
	{0x001A,'['},
	{0x0075,'\\'},
	{0x001B,']'},
	{0x0107,'^'},
	{0x010C,'_'},
	{0x0000,'`'},	//[NON ASSOCIATO A NESSUN TASTO]
	{0x001E,'a'},
	{0x0030,'b'},
	{0x002E,'c'},
	{0x0020,'d'},
	{0x0012,'e'},
	{0x0021,'f'},
	{0x0022,'g'},
	{0x0023,'h'},
	{0x0017,'i'},
	{0x0024,'j'},
	{0x0025,'k'},
	{0x0026,'l'},
	{0x0032,'m'},
	{0x0031,'n'},
	{0x0018,'o'},
	{0x0019,'p'},
	{0x0010,'q'},
	{0x0013,'r'},
	{0x001F,'s'},
	{0x0014,'t'},
	{0x0016,'u'},
	{0x002F,'v'},
	{0x0011,'w'},
	{0x002D,'x'},
	{0x0015,'y'},
	{0x002C,'z'},
	{0x011A,'{'},
	{0x0175,'|'},
	{0x011B,'}'},
	{0x0129,'~'},
	{0x0000,127},	//Delete [NON ASSOCIATO A NESSUN TASTO]

	//------------------------------------------------------

	{0x0052,'0'},	//0 del tastierino numerico
	{0x004F,'1'},	//1 del tastierino numerico
	{0x0050,'2'},	//2 del tastierino numerico
	{0x0051,'3'},	//3 del tastierino numerico
	{0x004B,'4'},	//4 del tastierino numerico
	{0x004C,'5'},	//5 del tastierino numerico
	{0x004D,'6'},	//6 del tastierino numerico
	{0x0047,'7'},	//7 del tastierino numerico
	{0x0048,'8'},	//8 del tastierino numerico
	{0x0049,'9'},	//9 del tastierino numerico
	{0x0053,'.'},	//. del tastierino numerico
	{0x0045,'/'},	/// del tastierino numerico
	{0x0046,'*'},	//* del tastierino numerico
	{0x0054,'-'},	//- del tastierino numerico
	{0x0037,'+'},	//+ del tastierino numerico
	{0x0056,174},	//Freccetta sinistra
	{0x007D,175},	//Freccetta destra
	{0x0078,176},	//Freccetta alto
	{0x0055,177},	//Freccetta in basso
	//COLORI DEL TESTO (COMBO CTRL-X o CTRL-SHIFT-X dove X e' una cifra esadecimale
	//(si usano i tasti numerici + le lettere da '1' a '8' per i colori, '9' e '0' per funzioni extra))
	{0x0202,178},	//Ctrl-1 = BLACK
	{0x0203,179},	//Ctrl-2 = DKBLUE
	{0x0204,180},	//Ctrl-3 = DKGREEN
	{0x0205,181},	//Ctrl-4 = DKCYAN
	{0x0206,182},	//Ctrl-5 = DKRED
	{0x0207,183},	//Ctrl-6 = DKPURPLE
	{0x0208,184},	//Ctrl-7 = DKBROWN
	{0x0209,185},	//Ctrl-8 = LTGRAY		(WHITE se blink on)
	{0x0302,186},	//Ctrl-shift-1 = DKGRAY		(BLACK blinking)
	{0x0303,187},	//Ctrl-shift-2 = LTBLUE		(BLUE blinking)
	{0x0304,188},	//Ctrl-shift-3 = LTGREEN	(GREEN blinking)
	{0x0305,189},	//Ctrl-shift-4 = LTCYAN		(CYAN blinking)
	{0x0306,190},	//Ctrl-shift-5 = LTRED		(RED blinking)
	{0x0307,191},	//Ctrl-shift-6 = LTPURPLE	(PURPLE blinking)
	{0x0308,192},	//Ctrl-shift-7 = LTBROWN	(BROWN blinking)
	{0x0309,193},	//Ctrl-shift-8 = WHITE		(WHITE blinking)
	{0x020A,194},	//Ctrl-9 REVERSE (indica alternativamente se viene modificato il bg  il fg)
	{0x020B,195},	//Ctrl-0 Abilita/disabilita il blinking
	//FINE COLORI
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
	{0x0000,0},
};
