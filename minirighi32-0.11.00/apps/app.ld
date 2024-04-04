/*
 *      Application linker script.
 *
 */

OUTPUT_FORMAT("elf32-i386")
TEXT_START = 0x100000;
ENTRY(main)
SECTIONS
{
	.text TEXT_START :
	{
		*(.text.*)
		*(.rodata.*)
	}

	.data :
	{
		*(.data*)
	}

	.bss :
	{
		*(.bss*)
		*(COMMON*)
	}
}
