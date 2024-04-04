/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: syscall.cpp			|
	| Date: 09/07/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <syscall.h>
#include <kservices/kout.h>
#include <hardware/cpu/idt.h>

#include <kdef.h>



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////

//Tabella delle SysCall
void (*SysCall::syscalltable[256])(dword ebx, dword ecx, dword edx);

/*
  SysCall nulla
*/
void null_syscall_handler(dword ebx, dword ecx, dword edx){
	kservices::KOut::WriteLine("NULL SYSCALL");
}

/*
  SysCall per scrivere a video
*/
void write(dword ebx, dword ecx, dword edx){
	kservices::KOut::WriteLine("WRITE");	
}

/*
  Handler delle SysCall
*/
void SysCall::syscall(){
	asm("cli");
	asm("pusha");

	volatile dword eax, ebx, ecx, edx;

	asm("movl %%eax, %0 " : "=m"(eax));
	asm("movl %%ebx, %0 " : "=m"(ebx));
	asm("movl %%ecx, %0 " : "=m"(ecx));
	asm("movl %%edx, %0 " : "=m"(edx));

	syscalltable[eax](ebx,edx,ecx);

	asm("popa");
	asm("sti");
}

/*
  Inizializzo le SysCall
*/
void SysCall::Init(){
	int i = 0;
	for(i = 0; i <= 255; i++) Add(null_syscall_handler, i);
	Add(write, 4);
	hardware::cpu::IDT::Add(syscall, 128);
}

/*
  Aggiungo una SysCall alla tabella delle SysCall
*/
void SysCall::Add(void (*func)(dword ebx, dword ecx, dword edx), size_t index){
	SysCall::syscalltable[index] = func;
}
