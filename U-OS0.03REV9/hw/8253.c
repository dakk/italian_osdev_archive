   #include <8253.h>
   #include <io.h>
   #include <irq.h>
   #include <idt.h>
   #include <8259.h>
   #include <video.h>
   #include <scheduler.h>

   int counter_0;
   int quants;

   void InitPIT()
   {
       disable_interrupts();
       counter_0 = 65535;
       outportb ( PIT_TIMER0_CW, PIT_CONTROL);
       outportb (counter_0 & 0xFF, PIT_TIMER0);
       outportb (counter_0 >> 8, PIT_TIMER0);
       start_PIT();
       enable_interrupts();
   }

   void start_PIT()
   {
       quants=0;
       enable_irq(0xFC, MASTER2_PIC);
       add_irq(PIT_irq, 0);
   }

   void PIT_irq()
   {  
    if(quants<QUANTS) quants++;
       else {
           quants=0;
           preSchedule();
       }
    }

    void maxQuants() {quants = QUANTS;}
