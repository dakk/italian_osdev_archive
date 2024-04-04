
#ifndef	_ISR_
#define	_ISR_

#include	"config.h"

#include	"descrittori.h"
#include	"kconsole.h"
#include	"task.h"


//Alcune Macro per gli ISR	(importantissime!!)

#define	ISR_DECLARE(nome_func)	extern void asm_ISR##nome_func()


#define	ISR_CODE_H(nome_func)			\
	asm (								\
	".globl asm_ISR"#nome_func"			\n"	\
	"asm_ISR"#nome_func":				\n"	\
											\
	"pushl	%eax					\n"	\
	"pushl	%ds						\n"	\
										\
	"movw	$0x10,%ax				\n"	\
	"movw	%ax,%ds					\n"	\
										\
	"movl	task_corrente_modo,%eax	\n"	\
	"cmp		$0,%eax					\n"	\
	"je		asm_ISRA"#nome_func"	\n"		\
										\
										\
	"movl	%ebx,task_corrente_uebx	\n"	\
	"movl	%ecx,task_corrente_uecx	\n"	\
	"movl	%edx,task_corrente_uedx	\n"	\
										\
	"movl	%ebp,task_corrente_uebp	\n"	\
	"movl	%esi,task_corrente_uesi		\n"	\
	"movl	%edi,task_corrente_uedi		\n"	\
										\
	"movl	%gs,task_corrente_ugs		\n"	\
	"movl	%fs,task_corrente_ufs		\n"	\
	"movl	%es,task_corrente_ues		\n"	\
										\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_uds		\n"	\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_ueax	\n"	\
										\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_ueip		\n"	\
	"xor		%eax,%eax				\n"	\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_ucs		\n"	\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_ueflags	\n"	\
										\
	"movl	%esp,task_corrente_uesp	\n"	\
	"xor		%eax,%eax				\n"	\
	"movw	%ss,%ax					\n"	\
	"movl	%eax,task_corrente_uss		\n"	\
										\
	"jmp		asm_ISRB"#nome_func"	\n"		\
										\
										\
	"asm_ISRA"#nome_func":			\n"	\
	\
										\
	"movl	%ebx,task_corrente_kebx	\n"	\
	"movl	%ecx,task_corrente_kecx	\n"	\
	"movl	%edx,task_corrente_kedx	\n"	\
										\
	"movl	%ebp,task_corrente_kebp	\n"	\
	"movl	%esi,task_corrente_kesi		\n"	\
	"movl	%edi,task_corrente_kedi		\n"	\
										\
	"movl	%gs,task_corrente_kgs		\n"	\
	"movl	%fs,task_corrente_kfs		\n"	\
	"movl	%es,task_corrente_kes		\n"	\
										\
	"popl	%eax					\n"	\
	"movw	%ax,task_corrente_kds		\n"	\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_keax	\n"	\
										\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_keip		\n"	\
	"xor		%eax,%eax				\n"	\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_kcs		\n"	\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_keflags	\n"	\
										\
	"movl	%esp,task_corrente_kesp	\n"	\
	"xor		%eax,%eax				\n"	\
	"movw	%ss,%ax					\n"	\
	"movl	%eax,task_corrente_kss		\n"	\
										\
										\
	"asm_ISRB"#nome_func":			\n"	\
										\
	"movw	$0x10,%ax				\n"	\
	"movw	%ax,%ds					\n"	\
	"movw	%ax,%es					\n"	\
	"movw	%ax,%fs					\n"	\
	"movw	%ax,%gs					\n"	\
										\
	"mov		$0x18,%ax				\n"	\
	"movw	%ax,%ss					\n"	\
	"movl	kernel_main_stack_top,%esp	\n"	\
										\
										\
	"calll   "#nome_func"				\n"	\
										\
										\
										\
	"movl	task_corrente_modo,%eax	\n"	\
	"cmp		$0,%eax					\n"	\
	"je		asm_ISRC"#nome_func"	\n"		\
										\
										\
	"movl	task_corrente_uebx,%ebx	\n"	\
	"movl	task_corrente_uecx,%ecx	\n"	\
	"movl	task_corrente_uedx,%edx	\n"	\
										\
	"movl	task_corrente_uebp,%ebp	\n"	\
	"movl	task_corrente_uesp,%esp	\n"	\
	"movl	task_corrente_uesi,%esi		\n"	\
	"movl	task_corrente_uedi,%edi		\n"	\
										\
	"movw	task_corrente_ugs,%gs		\n"	\
	"movw	task_corrente_ues,%es		\n"	\
	"movw	task_corrente_ufs,%fs		\n"	\
	"movw	task_corrente_uss,%ss		\n"	\
										\
	"movl	task_corrente_ueflags,%eax	\n"	\
	"pushl	%eax					\n"	\
	"movl	task_corrente_ucs,%eax		\n"	\
	"pushl	%eax					\n"	\
	"movl	task_corrente_ueip,%eax		\n"	\
	"pushl	%eax					\n"	\
										\
	"movl	task_corrente_ueax,%eax	\n"	\
	"movw	task_corrente_uds,%ds		\n"		\
										\
	"iret								\n"	\
										\
										\
										\
	"asm_ISRC"#nome_func":			\n"	\
										\
	"movl	task_corrente_kebx,%ebx	\n"	\
	"movl	task_corrente_kecx,%ecx	\n"	\
	"movl	task_corrente_kedx,%edx	\n"	\
										\
	"movl	task_corrente_kebp,%ebp	\n"	\
	"movl	task_corrente_kesp,%esp	\n"	\
	"movl	task_corrente_kesi,%esi		\n"	\
	"movl	task_corrente_kedi,%edi		\n"	\
										\
	"movw	task_corrente_kgs,%gs		\n"	\
	"movw	task_corrente_kes,%es		\n"	\
	"movw	task_corrente_kfs,%fs		\n"	\
	"movw	task_corrente_kss,%ss		\n"	\
										\
	"movl	task_corrente_keflags,%eax	\n"	\
	"pushl	%eax					\n"	\
	"movl	task_corrente_kcs,%eax		\n"	\
	"pushl	%eax					\n"	\
	"movl	task_corrente_keip,%eax		\n"	\
	"pushl	%eax					\n"	\
										\
	"movl	task_corrente_keax,%eax	\n"	\
	"movw	task_corrente_kds,%ds		\n"	\
										\
	"iret								\n"	\
	);									\
										\
void	nome_func()



#define	ISR_CODE_S(nome_func)			\
	asm (								\
	".globl asm_ISR"#nome_func"			\n"	\
	"asm_ISR"#nome_func":				\n"	\
									\
	"pushl	%eax					\n"	\
	"pushl	%ds						\n"	\
										\
	"movw	$0x10,%ax				\n"	\
	"movw	%ax,%ds					\n"	\
										\
	"movl	%ebx,task_corrente_uebx	\n"	\
	"movl	%ecx,task_corrente_uecx	\n"	\
	"movl	%edx,task_corrente_uedx	\n"	\
										\
	"movl	%ebp,task_corrente_uebp	\n"	\
	"movl	%esi,task_corrente_uesi		\n"	\
	"movl	%edi,task_corrente_uedi		\n"	\
										\
	"movl	%gs,task_corrente_ugs		\n"	\
	"movl	%fs,task_corrente_ufs		\n"	\
	"movl	%es,task_corrente_ues		\n"	\
										\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_uds		\n"	\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_ueax	\n"	\
										\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_ueip		\n"	\
	"xor		%eax,%eax				\n"	\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_ucs		\n"	\
	"popl	%eax					\n"	\
	"movl	%eax,task_corrente_ueflags	\n"	\
										\
	"movl	%esp,task_corrente_uesp	\n"	\
	"xor		%eax,%eax				\n"	\
	"movw	%ss,%ax					\n"	\
	"movl	%eax,task_corrente_uss		\n"	\
										\
										\
	"movw	$0x10,%ax				\n"	\
	"movw	%ax,%ds					\n"	\
	"movw	%ax,%es					\n"	\
	"movw	%ax,%fs					\n"	\
	"movw	%ax,%gs					\n"	\
										\
	"mov		$0x38,%ax				\n"	\
	"movw	%ax,%ss					\n"	\
	"movl	task_corrente_kstacktop,%esp \n"	\
										\
	"movl	$0,task_corrente_modo		\n"	\
										\
										\
	"calll   "#nome_func"				\n"	\
										\
										\
	"movl	task_corrente_uebx,%ebx	\n"	\
	"movl	task_corrente_uecx,%ecx	\n"	\
	"movl	task_corrente_uedx,%edx	\n"	\
										\
	"movl	task_corrente_uebp,%ebp	\n"	\
	"movl	task_corrente_uesp,%esp	\n"	\
	"movl	task_corrente_uesi,%esi		\n"	\
	"movl	task_corrente_uedi,%edi		\n"	\
										\
	"movw	task_corrente_ugs,%gs		\n"	\
	"movw	task_corrente_ues,%es		\n"	\
	"movw	task_corrente_ufs,%fs		\n"	\
	"movw	task_corrente_uss,%ss		\n"	\
										\
										\
	"movl	task_corrente_ueflags,%eax	\n"	\
	"pushl	%eax					\n"	\
	"movl	task_corrente_ucs,%eax		\n"	\
	"pushl	%eax					\n"	\
	"movl	task_corrente_ueip,%eax		\n"	\
	"pushl	%eax					\n"	\
										\
	"movl	task_corrente_ueax,%eax	\n"	\
	"movl	$1,task_corrente_modo		\n"	\
	"movw	task_corrente_uds,%ds		\n"	\
								\
	"iret								\n"	\
	);									\
										\
void	nome_func()

#define	ISR_ADDRESS(nome_func)	asm_ISR##nome_func

void isr_idt_init();									//Inizzializza gestore ISR (ovvero Interrupt Service Routines)

void	isr_setISR(unsigned char interrupt,void *handler);		//Setta una ISR abbinandola ad un certo INTerrupt
//interrupt_handler*	cpu_getISR(unsigned char interrupt);

#endif
