#include <keyboard.h>
#include <mydef.h>
#include <bitops.h>
#include <irq.h>
#include <io.h>
#include <8259.h>
#include <video.h>
#include <scheduler.h>
#include <stdio.h>

#define CODE_ESCAPE  0xe0
#define CODE_REL     0x80

#define CODE_LSHIFT  0x2a
#define CODE_RSHIFT  0x36
#define CODE_CPSLCK  0x3a

#define NULLENTRY    -1
#define KEY_ENTER    13
#define KEY_ESC      27
#define KEY_TAB       9
#define KEY_BKSPC     8

int def_norm_keytbl[] =
 { //ITALIAN ASCII TABLE
    -1,  27,  49,  50,  51,  52,  53,  54,  55,  56,  57,  48, //11
    39, 141,   8,   9, 113, 119, 101, 114, 116, 121, 117, 105, //23
   111, 112, 138,  43,  13,  -1,  97, 115, 100, 102, 103, 104, //35
   106, 107, 108, 149, 133,  92,  -1, 151, 122, 120,  99, 118, //47
    98, 110, 109,  44,  46,  45,  -1,  42,  -1,  32,  -1,  -1, //59
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  55, //71
    56,  57,  45,  52,  53,  54,  43,  49,  50,  51,  48,  46, //83
    -1,  -1,  60,  -1,  -1                                     //88
 };

int def_alt_keytbl[] =
 { //ITALIAN ALTERNATIVE ASCII TABLE
    -1,  -1,  33,  34, 156,  36,  37,  38,  47,  40,  41,  61, //11
    63,  94,  -1,  -1,  81,  87,  69,  82,  84,  89,  85,  73, //23
    79,  80, 130,  42,  -1,  -1,  65,  83,  68,  70,  71,  72, //35
    74,  75,  76, 128, 167, 124,  -1,  -1,  90,  88,  67,  86, //47
    66,  78,  77,  59,  58,  95,  -1,  -1,  -1,  32,  -1,  -1, //59
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, //71
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, //83
    -1,  -1,  62,  -1,  -1                                     //88
 };
 

int *norm_keytbl = def_norm_keytbl;
int *alt_keytbl = def_alt_keytbl;
int *cur_keytbl = def_norm_keytbl;

#define BUFSIZE 256
#define STEP(x) ( (x) == BUFSIZE-1 ? 0 : x+1 )
static int cbuffer[BUFSIZE];
static int buf_r, buf_w, cpslock;

void initKEYBOARD()
{
    enable_irq(0xFD, MASTER2_PIC);
    add_irq(KEYBOARD_irq, 1);
    buf_r = 0;
    buf_w = 0;
    cpslock = 0;
}

void KEYBOARD_irq()
{
    unsigned char sc;

LOOP:
    sc = inportb(0x60);
    
    if (sc == CODE_LSHIFT || sc == CODE_RSHIFT) {
       cur_keytbl = alt_keytbl;
       goto LOOP;
    }
    else if (sc == CODE_CPSLCK) {
       cpslock = !cpslock;
       _ksetleds( (cpslock ? LED_CAPS : 0) );
       return;
    }

    if (sc <= 127 && sc != CODE_ESCAPE)
    {
       if (sc <= 88 && cur_keytbl[sc] != NULLENTRY)
       {
          if (STEP(buf_w) == buf_r)
             buf_r = STEP(buf_r);
          
          cbuffer[buf_w] = cur_keytbl[sc];
          
          if (cpslock && (cbuffer[buf_w]>=97 && cbuffer[buf_w]<=122))
             cbuffer[buf_w] -= 32;
          
          buf_w = STEP(buf_w);
       }
       else
          printf("\nscancode: %xh\n", sc);
    }
    else if (sc != CODE_ESCAPE) {
       if (sc == (CODE_LSHIFT | CODE_REL) || sc == (CODE_RSHIFT | CODE_REL))
          cur_keytbl = norm_keytbl;
    }
    else
    {
       _kputs("\n***ESCAPE*** ");
       goto LOOP;
    }
}

int _kgetch()
{
   int c = -1;
   
   if (buf_r != buf_w) {
      c = cbuffer[buf_r];
      buf_r = STEP(buf_r);
   }
   
   return c;
}

void _ksetleds(char b)
{
__asm__ __volatile__(

       "  mov  $0xed, %%al   \n"
       "  out  %%al, $0x60   \n"
       "  mov  %0, %%al      \n"
       "  out  %%al, $0x60   \n"
      :
      : "g" (b)
      : "ax", "cx", "memory"
      );
}
