	.file	"8259.cc"
.globl _ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE
	.bss
	.align 2
	.type	_ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE,@object
	.size	_ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE,2
_ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE:
	.zero	2
	.text
	.align 2
.globl _Z8init8259v
	.type	_Z8init8259v,@function
_Z8init8259v:
	movl	$32, %edx
	movb	$17, %al
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	movl	$160, %edx
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	movl	$33, %edx
	movb	$48, %al
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	movl	$161, %edx
	movb	$56, %al
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	movl	$33, %edx
	movb	$4, %al
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	movl	$161, %edx
	movb	$3, %al
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	movl	$33, %edx
	movb	$1, %al
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	movl	$161, %edx
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	movw	$-1, _ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE
	movl	$33, %edx
	movb	_ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE, %al
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	movl	$161, %edx
	movb	$-1, %al
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	ret
.Lfe1:
	.size	_Z8init8259v,.Lfe1-_Z8init8259v
	.align 2
.globl _Z10enable_irqt
	.type	_Z10enable_irqt,@function
_Z10enable_irqt:
	movl	4(%esp), %ecx
	cmpw	$15, %cx
	ja	.L13
	movl	$-2, %eax
	roll	%cl, %eax
	andw	_ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE, %ax
	movw	%ax, _ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE
	cmpw	$7, %cx
	jbe	.L14
	andl	$-5, %eax
	movw	%ax, _ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE
.L14:
	movl	$33, %edx
	movb	_ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE, %al
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	movl	$161, %edx
	movb	_ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE+1, %al
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	movl	$1, %eax
	ret
.L13:
	movl	$0, %eax
	ret
.Lfe2:
	.size	_Z10enable_irqt,.Lfe2-_Z10enable_irqt
	.align 2
.globl _Z11disable_irqt
	.type	_Z11disable_irqt,@function
_Z11disable_irqt:
	movl	4(%esp), %ecx
	cmpw	$15, %cx
	ja	.L18
	movl	$1, %eax
	sall	%cl, %eax
	movw	_ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE, %dx
	orl	%eax, %edx
	movw	%dx, _ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE
	movl	%edx, %eax
	movb	$0, %al
	cmpw	$-256, %ax
	jne	.L19
	movl	%edx, %eax
	orl	$4, %eax
	movw	%ax, _ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE
.L19:
	movl	$33, %edx
	movb	_ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE, %al
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	movl	$161, %edx
	movb	_ZN24_GLOBAL__N_8259.ccQAkSjb8irq_maskE+1, %al
#APP
	outb %al,%dx;outb %al,$0x80
#NO_APP
	movl	$1, %eax
	ret
.L18:
	movl	$0, %eax
	ret
.Lfe3:
	.size	_Z11disable_irqt,.Lfe3-_Z11disable_irqt
	.align 2
.globl _Z11get_cur_irqv
	.type	_Z11get_cur_irqv,@function
_Z11get_cur_irqv:
	pushl	%edi
	pushl	%ebx
	subl	$4, %esp
	movl	$32, %edx
	movb	$11, %al
#APP
	outb %al,%dx;outb %al,$0x80
	inb %dx,%al;outb %al,$0x80
#NO_APP
	movb	%al, 3(%esp)
	testb	%al, %al
	jne	.L25
	movl	$160, %edx
	movb	$11, %al
#APP
	outb %al,%dx;outb %al,$0x80
	inb %dx,%al;outb %al,$0x80
#NO_APP
	movb	%al, 3(%esp)
.L25:
	leal	3(%esp), %edi
	movl	$1, %ecx
	movl	%edi, %ebx
#APP
	xorl %eax,%eax
	repe; scasl
	jz 1f
	leal -4(%edi),%edi
	bsfl (%edi),%eax
1:	subl %ebx,%edi
	shll $3,%edi
	addl %edi,%eax
#NO_APP
	addl	$4, %esp
	popl	%ebx
	popl	%edi
	ret
.Lfe4:
	.size	_Z11get_cur_irqv,.Lfe4-_Z11get_cur_irqv
	.ident	"GCC: (GNU) 3.2.2"
