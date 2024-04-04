
.text
.code16

.global _start
_start:
	mov    %cs,%ax
	mov    %eax,%ds
	mov    %eax,%es
	mov    %cs,%bx
	add    $0x20,%bh
	mov    %ebx,%ds
	mov    $0x13,%ax
	int    $0x10
Main:
	push   %ds
	pop    %es
	in     $0x40,%ax
	shl    $0x4,%ax
	mov    %ax,%di
	mov    $0xff,%al
	mov    $0x32,%bx
pl:
	add    $0x10e,%di
	mov    $0x32,%cx
	repz stos %al,%es:(%di)
	dec    %bx
	jne    pl
	mov    $0xfa,%bh
Smudge:
	mov 1(%bx),%al
	mov -1(%bx),%cl
	add    %cx,%ax
	mov -320(%bx),%cl
	add    %cx,%ax
	mov 320(%bx),%cl
	add    %cx,%ax
	shr    $0x2,%ax
	mov %al,(%bx)
	dec    %bx
	jne    Smudge
	mov    $0xa000,%ax
	mov    %eax,%es
	mov    $0xfa,%ch
	xor    %di,%di
	xor    %si,%si
	repz movsb %ds:(%si),%es:(%di)
	in     $0x60,%al
	dec    %al
	jne    Main
	mov    $0x3,%ax
	int    $0x10
	xor    %eax,%eax
	int    $0x80
