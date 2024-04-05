#ifndef _TASK_H_
#define _TASK_H_

/* TSS definition */
struct TSS {        /* TSS for 386+ */
  unsigned int link;
  unsigned int esp0;
  unsigned int ss0;
  unsigned int esp1;
  unsigned int ss1;
  unsigned int esp2;
  unsigned int ss2;
  unsigned int cr3;
  unsigned int eip;
  unsigned int eflags;
  unsigned int eax;
  unsigned int ecx;
  unsigned int edx;
  unsigned int ebx;
  unsigned int esp;
  unsigned int ebp;
  unsigned int esi;
  unsigned int edi;
  unsigned int es;
  unsigned int cs;
  unsigned int ss;
  unsigned int ds;
  unsigned int fs;
  unsigned int gs;
  unsigned int ldt;
  unsigned int trapbit;
  unsigned int iomapbase;
  };
  
  #define MAX_NAME_CHAR 20
  
  #define AVVIABILE 0
  #define MORTO 1
  #define SOSPESO 2
  
  
  struct TASK{
  	unsigned char nome[MAX_NAME_CHAR+1];
	unsigned short int pid; //process id
	unsigned char stato;
  };
  
void add_task(void* eip, unsigned char *nome);
void ps();
void omicide(unsigned short int pid); /* ^__^ */
void suicide();
void init_task();
void scheduler();
void get_curtask_status();

  
#endif
