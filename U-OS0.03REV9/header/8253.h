   #ifndef  __PIT_H_
   #define __PIT_H_

   #define PIT_FREQ 1193181
   #define PIT_TIMER0 0x40
   #define PIT_TIMER1 0x41
   #define PIT_TIMER2 0x42
   #define PIT_CONTROL 0x43
   #define PIT_TIMER0_CW 0x37
   #define QUANTS 5

   extern int counter_0;

   void InitPIT();
   void PIT_irq();
   void start_PIT();
   void maxQuants();

   #endif

