#include <idt.h>
#include <syscall.h>
#include <stdio.h>
#include <scheduler.h>
#include <signal.h>
#include <apm.h>

extern void syscall_handler_main (void);

int syscall_handler(dword eax, dword ebx, dword ecx, dword edx) {
  /* This function calls the syscalls
   * eax = syscall number; ebx, ecx, edx = parameters
   * Syscall numbers are defined into syscall.h
   */
  
  asm("sti");
  switch(eax) {
  case SC_PUTCH:
    putchar(ebx);
    return 0;
  case SC_GETCH:
    return getchar();
  case SC_SIGNAL:
    return _kipc_signal(ebx, ecx);
  case SC_FORK:
    return fork();
  case SC_REBOOT:
    _kapm_reboot();
    return 0;
  case SC_SHUTDOWN:
    _kapm_shutdown();
    return 0;
  }
  return -1;
}

/* Define the main syscall handler which call the other one */
asm (".globl syscall_handler_main\n"
     "syscall_handler_main:\n"
     "pushl %edx\n"
     "pushl %ecx\n"
     "pushl %ebx\n"
     "pushl %eax\n"
     "call syscall_handler\n"
     "addl $16, %esp\n"
     "iret\n"
     );

void init_syscall() {
  /* Initialize syscall interrupt 0xFF */
  add(syscall_handler_main, 0xff);
}
