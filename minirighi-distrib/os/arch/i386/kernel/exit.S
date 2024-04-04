/*!	\file arch/i386/kernel/exit.S
 *	\brief Exit point for task & threads.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-12\n
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

.file	"exit.S"

.section .text.exit_point
.align 4096

; // Exit point for task and threads.
; // This is the only page of the kernel that must be
; // available for execution from the user privilege level.
.global __task_exit_point
__task_exit_point:
	pushl	%eax		; // Push the return value.
1:	xorl	%eax, %eax	; // Invoke the exit syscall.
	int	$0x80
	jmp	1b

.align 4096
__task_exit_point_end:
