#include <exec.h>
#include <syscall.h>
#include <cstdio>
#include <video.h>
#include <idt.h>
#include <mem.h>
#include <cstring>
#include <key_traslation.h>
#include <irq.h>
#include <tastiera.h>
#include <stdarg.h>
#include <var_amb.h>
#include <floppy.h>

extern "C"
{

  void null_syscall (dword indice, dword ebx, dword ecx, dword edx)
  {
    kout << "\nSONO UNA SYSCALL INESISTENTE RIPROVA E SARAI + FORTUNATO";
    kout << "\nIndice = " << indice;
    kout << "ebx = " << ebx;
    kout << "ecx = " << ecx;
    kout << "edx = " << edx;
    while (1);
  }

  extern void internal_syscall_handler (void);


  int very_syscall_handler (dword indice, dword ebx, dword ecx, dword edx)
  {
    asm ("sti");
// kout << "\nallora mi è arrivata una richiesta di syscall\n";
    switch (indice)
      {
//---->FUNZIONI PER LA GESTIONE DELLA MEMORIA<-------
      case SYS_ALLOC:		//è una malloc trovo la quatità di ram da allocare in ebx
		return (int) kmalloc (ebx);
      case SYS_FREE:		//è una free()
		kfree ((void *) ebx);
		return 0;
      case SYS_REALLOC:
		return (int) krealloc ((void *) ebx, (size_t) ecx);
      case SYS_CALLOC:
		return (int) kcalloc (ebx, ecx);

//----->FUNZIONI PER LA GESTIONE DELL'OUTPUT A SCHERMO<--------
      case SYS_PUTC:
		kout << char (ebx);
		return 0;
      case SYS_PUTS:
		kout << (char *) ebx;
		return strlen ((char *) ebx);
      case SYS_SETTXTCOLOR:
		kout.set_text_color (ebx);
		return 0;
      case SYS_SETBACKCOLOR:
		kout.set_back_color (ebx);
		return 0;
      case SYS_GETTXTCOLOR:
		return kout.get_text_color ();
      case SYS_GETBACKCOLOR:
		return kout.get_back_color ();
      case SYS_GETSCRPOS:
		return kout.get_pos ();

//------->FUNZIONI PER LA GESTIONE DELL'INPUT VIA TASTIERA<----
      case SYS_GETC:
		return sys_getchar ();
      case SYS_GETS:
		sys_gets ((char *) ebx);
		return 0;
      case SYS_ENABLEIRQ:
		return (int) enable_irq (ebx);
      case SYS_DISABLEIRQ:
		return (int) disable_irq (ebx);
      case SYS_KBHIT:
		return kbpress ();
	  
//---------->FUNZIONI PER LA GESTIONE DEI TASK <-----------------	
	  case SYS_READ_ENV_VAR:
		  return (int)leggi_valore((char*)ebx, (char*)ecx);
	  case SYS_WRITE_ENV_VAR:
		  setta_valore((char*)ebx, (char*)ecx);
		  return 0;
	  //---------->FUNZIONI DELLA LIBC<----------------
	  case LIBC_VSPRINTF:
		  return vsprintf((char*)ebx, (char*)ecx, (va_list)edx);
	  //       int vsscanf(const char *str, const char *format, va_list ap);
	  case LIBC_VSSCANF:
		  return vsscanf((char*)ebx, (char*)ecx, (va_list)edx);		  
	  case SYS_EXEC_CCC:
		  return (int)exec_CCC((char*)ebx, (char*)ecx, (unsigned char*)edx);
	  case SYS_READ_BLOCK:
		  return (unsigned char)read_block((int)ebx, (unsigned char *)ecx, (int)edx);
	  case SYS_WRITE_BLOCK:
		  return (unsigned char)write_block((int)ebx, (unsigned char *)ecx, (int)edx);
		  /*	  case SYS_FLOPPY_R_BUFF:
             	  return (int)floppy_r_buff((int)ebx,(byte *)ecx,(int)edx);
	  case SYS_FLOPPY_W_BUFF:
	  return (int)floppy_w_buff((int)ebx,(byte *)ecx,(int)edx);*/
	  case SYS_EXEC:
		  return (int)exec((char*)ebx, (char*)ecx);
      default:
		  null_syscall (indice, ebx, ecx, edx);
      }
    return -1;
  }


  asm (".globl internal_syscall_handler\n"
       "internal_syscall_handler: \n"
       "pushl %edx\n"
       "pushl %ecx\n"
       "pushl %ebx\n"
       "pushl %eax\n"
       "call very_syscall_handler\n" "addl $16, %esp\n" "iret \n");

}

void
init_syscall ()
{
  kout << "Inzializzazione delle syscall all'interrupt " << 85 << endl;;
  idt::add (internal_syscall_handler, 85);
  return;
}
