; // This is a simple v86 mode program.

.text
.code16

.global _start
_start:
	mov	%cs, %ax
	mov	%eax, %ds
	mov	%eax, %es

	; // Switch into video mode 13.
	mov	$0x13, %ax
	int	$0x10

	; // Print the welcome message.
	mov	$welcome_msg, %esi
	call	print

	; // Delay ~5 seconds.
	movl	$(100*5), %ecx
1:	hlt
	decl	%ecx
	cmpl	$0, %ecx
	jnz	1b

	; // Restore video text mode.
	mov	$0x3,%ax
	int	$0x10

	; // Back to minirighi.
	xor	%eax, %eax
	int	$0x80

; // Print the message pointed by %si.
print:
	lodsb
	cmpb	$0, %al
	je	2f
	movb	$0x0e, %ah
	movw	$0x07, %bx
	int	$0x10
	jmp	print
2:
	ret

welcome_msg:
	.ascii "\n\rWelcome in v86 mode!!!\n\rWait 5 seconds..."
	.byte 0
