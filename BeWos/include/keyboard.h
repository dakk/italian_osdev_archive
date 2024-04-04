#define BUF_SIZE 1024

void isr_kbd(void);
void disabilita_kbd(void);
void abilita_kbd(void);
void initKBD(void);
char buffer[BUF_SIZE];
char block_clear_status;
char *block_video_mem;
int pos;


       /*TASTIERA ITALIANA*/

static unsigned char key_map[] = {
       0,   27,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0', '\'',    0,  127,    9,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p',    0,  '+', '\n',    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l',    0,
       0, '\\',    0,    0,  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '-',    0,  '*',
       0,   32,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0,    0,    0,    0,  '<',    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0 };

static unsigned char shift_map[] = {
       0,   27,  '!',  '"',    0,  '$',  '%',  '&',
     '/',  '(',  ')',  '=',  '?',  '^',  127,    9,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',    0,  '*',   13,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',    0,
       0,  '|',  '0',    0,  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  ';',  ':',  '_',    0,    0,
       0,   32,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,  '>',    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0 };



       

      /*TASTIERA INGLESE */

/*static unsigned char key_map[] = {
       0,   27,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=',  127,    9,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p',  '[',  ']', '\n',    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',
    '\'',  '`',    0, '\\',  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '/',    0,  '*',
       0,   32,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0,    0,    0,    0,  '<',    0,
       0,    0,    0,    0,    0,    0,    0,    0,
	   0 };

static unsigned char shift_map[] = {
       0,   27,  '!',  '@',  '#',  '$',  '%',  '^',
     '&',  '*',  '(',  ')',  '_',  '+',  127,    9,
	 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
	 'O',  'P',  '{',  '}',   13,    0,  'A',  'S',
	 'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',
     '"',  '~',  '0',  '|',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  '<',  '>',  '?',    0,  '*',
       0,   32,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0,    0,    0,    0,  '>',    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0 };*/



       
