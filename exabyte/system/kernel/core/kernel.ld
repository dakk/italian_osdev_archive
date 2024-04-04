OUTPUT_FORMAT("elf32-i386")
/* no leading underscores for symbols handled in asm: */
ENTRY(arch_start)
load_adr = 0x100000;	/* 1 meg = physical address */
virt_adr = 0x100000;	/* 1 meg = virtual address */
SECTIONS
{
    .text virt_adr : AT(load_adr)
    {
/* discardable kernel code */
	g_d_code = .; _g_d_code = .; g_start = .; _g_start = .;
	*(.dtext)
	. = ALIGN(4096);
/* kernel code */
	g_code = .; _g_code = .;
	*(.text*)
/* ELF constant data section(s)
Thanks to Jeroen van Bemmel for helping with this. */
	*(.rodata*)
	. = ALIGN(4096);
/* fill empty space at end of section with INT 3 (breakpoint) */
    } = 0xCCCC

/* sezione per il boot degli ap a 16-bit */
    .ap_boot : AT(load_adr + (g_ap_boot - g_d_code))
    {
	g_ap_boot = .; _g_ap_boot = .;
	*(.ap_boot)
	g_ap_boot_end = .; _g_ap_boot_end = .;
	. = ALIGN(4096);
} = 0xCCCC

    .data : AT(load_adr + (g_d_data - g_d_code))
    {
/* discardable kernel data */
	g_d_data = .; _g_d_data = .;
	*(.ddata)
	. = ALIGN(4096);
/* kernel data */
	g_data = .; _g_data = .;
	*(.data*)
	. = ALIGN(4096);
    } = 0xCCCC
    .bss : AT(load_adr + (g_d_bss - g_d_code))
    {
/* discardable kernel BSS */
	g_d_bss = .; _g_d_bss = .;
	*(.dbss)
	. = ALIGN(4096);
/* kernel BSS */
	g_bss = .; _g_bss = .;
	*(.bss*)
	*(COMMON) /* "common" variables */
	. = ALIGN(4096);
    } = 0xCCCC
/* end of kernel */
    g_end = .; _g_end = .;
}
