char inportb  (int numport)
{
  char valore;
  __asm__ volatile ("inb %%dx,%%al":"=a" (valore):"d" (numport));
  return valore;
 }


 void outportb (int portdata, int portnum)
 {
   __asm__ volatile ("outb %%al,%%dx"::"a" (portdata),"d" (portnum));
  return;
  }


