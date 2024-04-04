/*!	\file arch/i386/kernel/syscall.S
 *	\brief System call handler.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-12-04 Andrea Righi:
 *			Added the sys_fork entry.\n
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

.file	"syscall.S"

; //! Include common assembler header.
#include <arch/asm.h>
#include <errno.h>

; //! Total number of system call.
#define SYS_CALL_NR	(syscall_table_size / 4)

.data

; // The system call table.
.global syscall_table
ALIGN
syscall_table:
	.long auto_kill
	.long kgetchar
	.long kputchar
	.long umalloc
	.long ufree
	.long sys_syslog
	.long sys_times
	.long sys_thread_spawn
	.long sys_getpid
	.long sys_getppid
	.long sys_time
	.long sys_socket
	.long sys_closesocket
	.long sys_bind
	.long sys_recvfrom
	.long sys_sendto
	.long sys_fork
	.long sys_nanosleep

; //! Size of the system call table in bytes.
syscall_table_size = ( . - syscall_table)

.text

; //! Minirighi system call handler.
.extern syscall_table
.global _system_call_entry
ALIGN
_system_call_entry:
	cmpl $(SYS_CALL_NR), %eax
	ja	bad_syscall
	cld
	push	%ds
	push	%es
	; // These are both the register of the task and
	; // the arguments of the system call.
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	; // Update segment registers with kernel data descriptor.
	movw	$KERNEL_DATA_SEL, %bx
	movw	%bx, %ds
	movw	%bx, %es
	; // Execute the system call.
	call	*syscall_table(,%eax,4)

;//! Return from a system call.
ret_from_syscall:
	popl	%ebx
	popl	%ecx
	popl	%edx
	popl	%esi
	popl	%edi
	popl	%ebp
	pop	%es
	pop	%ds
	iretl

; //! An unknown system call has been invoked.
bad_syscall:
	movl	$(-ENOSYS), %eax
	iretl

; //! fork() system call entry point.
sys_fork:
	movl	%esp, %ebx
	pushl	%ebx
	call	do_fork
	popl	%ebx
	ret
