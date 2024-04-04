#include <cpu_oper.h>

dword get_flag(){
	dword result;
	__asm__ __volatile__("pushfl");
	__asm__ __volatile__("cli");
	__asm__ __volatile__("popl %eax");
	__asm__ __volatile__("movl %%eax,%0\n":"=r"(result): :"eax");
	return (result);
};


bool check_fpu(){
	int result;
	__asm__ __volatile__("movl %cr0,%eax");
	__asm__ __volatile__("movl %eax,%edi");
	__asm__ __volatile__("andl $0x0FFFFFFEF,%eax");
	__asm__ __volatile__("movl %eax,%cr0");
	__asm__ __volatile__("movl %cr0,%eax");
	__asm__ __volatile__("xchgl %edi,%eax");
	__asm__ __volatile__("movl %eax,%cr0");
	__asm__ __volatile__("movl %edi,%eax");
	__asm__ __volatile__("andl $0x10,%eax");
	__asm__ __volatile__("shrb $4,%al");
	__asm__ __volatile__("movl %%eax,%0":"=r"(result) : :"eax");
	return result;
}
