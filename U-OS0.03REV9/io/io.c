 #include <io.h>
 
 byte inportb  (word numport)
 {
     byte portvalue;
     __asm__  ("inb %%dx,%%al;outb %%al,$0x80" :"=a" (portvalue) : "d"(numport));
     return portvalue;
 }
 
 void outportb (word portdata, word portnum)
 {
 asm volatile ("outb %%al,%%dx;outb %%al,$0x80": :"d" (portnum), "a"(portdata));
 }

