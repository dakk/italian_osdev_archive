#include <mydef.h>
#include <video.h>
#include <bitops.h>

int find_first_bit(unsigned short int irq_mask)
{
   int i=0;
   for (i=0; i<8;i++) if ((1<<i)&irq_mask) break;
       return i;
}

void print_bin(unsigned short int number)
{
   int i=0;
   byte mask=1;
   mask<<=7;
   for(i=1; i<=8; i++)
   {
       (number & mask) ? _kputc('1') : _kputc('0');
       number <<=1;
   }
}
