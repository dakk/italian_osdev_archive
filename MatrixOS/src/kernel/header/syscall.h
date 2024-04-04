/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: syscall.h			|
	| Date: 09/07/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _SYSCALL_H_
#define _SYSCALL_H_


/*
	**************************
	**	 Include	**
	**************************
*/

#include <kdef.h>

class SysCall{
public:
	static void (*syscalltable[256])(dword ebx, dword ecx, dword edx);
	static void Init();
	static void syscall();
	static void Add(void (*func)(dword ebx, dword ecx, dword edx), size_t index);
};

#endif
