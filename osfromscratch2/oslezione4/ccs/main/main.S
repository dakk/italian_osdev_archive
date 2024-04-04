	.file	"main.cc"
	.text
	.align 2
.globl _Z6_startv
	.type	_Z6_startv,@function
_Z6_startv:
	subl	$12, %esp
	call	_Z17__do_global_ctorsv
	call	main
.L5:
	jmp	.L5
.Lfe1:
	.size	_Z6_startv,.Lfe1-_Z6_startv
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"\nItalios avviato"
.LC1:
	.string	"\t\t\t\tItaliOs "
	.section	.rodata.str1.32,"aMS",@progbits,1
	.align 32
.LC2:
	.string	"Benvenuto nell' Italia Operating System"
	.text
	.align 2
.globl main
	.type	main,@function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	andl	$-16, %esp
	subl	$8, %esp
	pushl	$10
	subl	$12, %esp
	pushl	$.LC0
	pushl	$debug
	call	_ZN5DebuglsEPKc
	addl	$20, %esp
	pushl	%eax
	call	_ZN5DebuglsEc
	addl	$8, %esp
	pushl	$10
	subl	$12, %esp
	pushl	$10
	subl	$12, %esp
	pushl	$.LC1
	subl	$12, %esp
	pushl	$10
	subl	$12, %esp
	pushl	$10
	pushl	kout
	call	_ZN5VideolsEc
	addl	$20, %esp
	pushl	%eax
	call	_ZN5VideolsEc
	addl	$20, %esp
	pushl	%eax
	call	_ZN5VideolsEPKc
	addl	$20, %esp
	pushl	%eax
	call	_ZN5VideolsEc
	addl	$20, %esp
	pushl	%eax
	call	_ZN5VideolsEc
	addl	$8, %esp
	pushl	$10
	subl	$12, %esp
	pushl	$.LC2
	pushl	kout
	call	_ZN5VideolsEPKc
	addl	$20, %esp
	pushl	%eax
	call	_ZN5VideolsEc
	call	_Z8init_gdtv
	call	_ZN3idt4initEv
	addl	$8, %esp
	pushl	$49
	pushl	$_Z16tastiera_handlerv
	call	_ZN3idt3addEPFvvEj
	movl	$1, (%esp)
	call	_Z10enable_irqt
	movl	$0, %eax
	leave
	ret
.Lfe2:
	.size	main,.Lfe2-main
	.section	.rodata.str1.1
.LC3:
	.string	"\350 stato premuto un tasto"
	.text
	.align 2
.globl _Z16tastiera_handlerv
	.type	_Z16tastiera_handlerv,@function
_Z16tastiera_handlerv:
	subl	$20, %esp
#APP
	pusha
#NO_APP
	movl	$96, %edx
#APP
	inb %dx,%al
#NO_APP
	pushl	$10
	subl	$12, %esp
	pushl	$.LC3
	pushl	kout
	call	_ZN5VideolsEPKc
	addl	$20, %esp
	pushl	%eax
	call	_ZN5VideolsEc
	movl	$32, %edx
	movb	$32, %al
#APP
	outb %al,%dx
	popa
	iret
#NO_APP
	addl	$28, %esp
	ret
.Lfe3:
	.size	_Z16tastiera_handlerv,.Lfe3-_Z16tastiera_handlerv
	.align 2
.globl _Z17__do_global_ctorsv
	.type	_Z17__do_global_ctorsv,@function
_Z17__do_global_ctorsv:
	pushl	%esi
	pushl	%ebx
	subl	$4, %esp
	movl	$__CTOR_LIST__+4, %esi
	movl	__CTOR_LIST__, %ebx
	decl	%ebx
	cmpl	$-1, %ebx
	je	.L17
.L15:
	call	*(%esi)
	addl	$4, %esi
	decl	%ebx
	cmpl	$-1, %ebx
	jne	.L15
.L17:
	addl	$4, %esp
	popl	%ebx
	popl	%esi
	ret
.Lfe4:
	.size	_Z17__do_global_ctorsv,.Lfe4-_Z17__do_global_ctorsv
	.ident	"GCC: (GNU) 3.2.2"
