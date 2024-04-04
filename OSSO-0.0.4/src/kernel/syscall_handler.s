
 # OSSO - A Micro Kernel OS
 # Copyright (c) 2000 Alessandro Iurlano.
 #
 # This program is free software; you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation; either version 2, or (at your option)
 # any later version.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with this program; if not, write to the Free Software
 # Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 # 02111-1307, USA.
 
.globl system_call_handler,_Syscall

system_call_handler:	
	push %ebx
	push %ecx
	push %edx
	push %edi
	push %esi
	push %ebp
	
	push %ds
	push %es
	push %fs
	push %gs
	pushl %edi
	pushl %edx		
	pushl %ecx
	pushl %ebx
	pushl %eax
	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call Syscall
	popl %ebx
	popl %ebx
	popl %ebx
	popl %ebx
	popl %ebx
	pop %gs
	pop %fs
	pop %es
	pop %ds
	
	pop %ebp
	pop %esi
	pop %edi
	pop %edx
	pop %ecx
	pop %ebx
	iret
	