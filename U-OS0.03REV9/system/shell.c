#include <video.h>
#include <scheduler.h>
#include <keyboard.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <signal.h>
#include <clock.h>
#include <dma.h>
#include <paging.h>
#include <floppy.h>
#include <kernel.h>
#include <kprg.h>

void shell()
{
   char st[255] = {0};
   int p = 0;

   _kputs("\nStarting " SHELLVERSION " ...\n"
          "Type 'help' for available commands.\n\n");

   for(;;)
   {
      _kputs("uos# ");

      for(;;)
      {
         int c = _kgetch();

          if(c!=-1) {
            if (p < 253 && (c >= 32 && c <= 126)) {
               st[p++] = c;
               _kputc(c);
            }

            if (c == 13)
            {
               st[p] = 0;
               p = 0;
               _kputs("\n");

               if (_kstrncmp(st, "tlist", 5) == 0)
               {
                  int i;

                  _kputs("Task list:\n\n");
                  _kputs("PID  Name\n");
                  for(i=0; i<next_pid; i++)
                     if(array_task[i].name!=NULL)
                     printf("%2d   %s\n", array_task[i].PID, array_task[i].name);
               }
               else if (_kstrncmp(st, "ver", 3) == 0)
                  _kputs("\n" SHELLVERSION "\n" OSNAME OSVERSION "\n\n");
               else if (_kstrncmp(st, "clear", 5) == 0)
                  _kclear();
               else if (_kstrncmp(st, "help", 4) == 0)
                  _kputs("\n\033\006COMMAND LIST:\033\007\n\n"
                         "\033\003help         \033\007 -   Show this text\n"
                         "\033\003ver          \033\007 -   Show shell version\n"
                         "\033\003clear        \033\007 -   Clear screen\n"
                         "\033\003tlist        \033\007 -   Show task list\n"
                         "\033\003date         \033\007 -   Show the date\n"
			 "\033\003time         \033\007 -   Show the time\n"
			 "\033\003rtc          \033\007 -   Show the rtc\n"
                         "\033\003prova        \033\007 -   Test - semafori\n"
                         "\033\003floppydetect \033\007 -   Detect floppy\n"
                         "\033\003startfloppy  \033\007 -   Start floppy-motor\n"
                         "\033\003stopfloppy   \033\007 -   Stop floppy-motor\n"
//"getstring    -   Input String and Output it\n"
                         "\033\003sumprg       \033\007 -   Simple program that executes the sum of 2 numbers\n"
			 "\033\003halt         \033\007 -   Halt your PC\n\n");
               else if (_kstrncmp(st, "prova", 4) == 0) prova();
	       else if (_kstrncmp(st, "rtc", 4) == 0) readrtc();
	       else if (_kstrncmp(st, "date", 4) == 0) readdate();
	       else if (_kstrncmp(st, "time", 4) == 0) readtime();
               else if (_kstrncmp(st, "floppydetect", 12) == 0) detect_floppy();
               else if (_kstrncmp(st, "startfloppy", 11) == 0) start_motor();
               else if (_kstrncmp(st, "stopfloppy", 10) == 0) stop_motor();
               else if (_kstrncmp(st, "sumprg", 6) == 0) sumprg();
               else if (_kstrncmp(st, "getstring", 9) == 0) provagets();
	       else if (_kstrncmp(st, "halt", 4) == 0) halt();
               else {
                  printf("Unknown command '%s'\n", st);
               }
               break;
            }
            else if (c == 8) //backspace
            {
               if (p > 0) {
                  p--;
                  st[p] = 0;
                  _kbackspace();
               }
            }
         }
      }
   }
}
