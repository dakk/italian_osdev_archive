
;Questo Loader e' poco professionale perche' non carica sotto 1M !!!!
GLOBAL _start
GLOBAL boot_info

[EXTERN _start2]

_start:
	jmp	_start2

	; // Multiboot header (...required by GRUB!) //
	MULTIBOOT_PAGE_ALIGN equ (1 << 0)
	MULTIBOOT_MEMORY_INFO equ (1 << 1)

	MULTIBOOT_HEADER_MAGIC equ (0x1BADB002)
	MULTIBOOT_HEADER_FLAGS equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
	CHECKSUM equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

	ALIGN 4
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd CHECKSUM

	; // Linear address of a block of system bootstrap informations (from GRUB) //
	boot_info 	dd 	0x00000000
