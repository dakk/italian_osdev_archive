/*
 * Dreamos
 * apm.c
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */ 

/* This is 16-bit protected mode code to disable paging and the cache,
 * switch to real mode and jump to the BIOS reset code.*/

/*
 * Author: Lethalman
 */

#include <apm.h>
#include <string.h>
#include <io.h>
#include <stdio.h>
#include <video.h>
#include <misc.h>

void init_apm() {
  apm_setup.memory_check = 0;
  apm_setup.kb_reboot = 0;
}

static unsigned char rmode_switch [] = {
  0x66, 0x0f, 0x20, 0xc0,     /*    movl  %cr0,%eax        */
  0x66, 0x83, 0xe0, 0x11,     /*    andl  $0x00000011,%eax */
  0x66, 0x0d, 0x00, 0x00, 0x00, 0x60, /*    orl   $0x60000000,%eax */
  0x66, 0x0f, 0x22, 0xc0,     /*    movl  %eax,%cr0        */
  0x66, 0x0f, 0x22, 0xd8,     /*    movl  %eax,%cr3        */
  0x66, 0x0f, 0x20, 0xc3,     /*    movl  %cr0,%ebx        */
  0x66, 0x81, 0xe3, 0x00, 0x00, 0x00, 0x60, /*    andl  $0x60000000,%ebx */
  0x74, 0x02,       /*    jz    f                */
  0x0f, 0x09,       /*    wbinvd                 */
  0x24, 0x10,       /* f: andb  $0x10,al         */
  0x66, 0x0f, 0x22, 0xc0      /*    movl  %eax,%cr0        */
};

static unsigned char rmode_poweroff [] = {
  // Setup the real-mode stack pointer.
  0xb8, 0x00, 0x90,     // movw $0x9000, %ax
  0x8e, 0xd0,       // movw %ax, %ss
  0xbc, 0x00, 0xf0,     // movw $0xf000, %sp
  
  // Connect to the APM BIOS.
  0xb8, 0x01, 0x53,     // mov $0x5301, %ax
  0x31, 0xdb,       // xor %bx, %bx
  0xcd, 0x15,       // int $0x15
  
  // Enable power management.
  0xb8, 0x08, 0x53,     // mov $0x5308, %ax
  0xbb, 0x01, 0x00,     // mov $0x0001, %bx
  0xb9, 0x01, 0x00,     // mov $0x0001, %cx
  0xcd, 0x15,       // int $0x15
// Enable device power management.
  0xb8, 0x0d, 0x53,     // mov $0x530d, %ax
  0xbb, 0x01, 0x00,     // mov $0x0001, %bx
  0xb9, 0x01, 0x00,     // mov $0x0001, %cx
  0xcd, 0x15,       // int $0x15

  // Driver version.
  0xb8, 0x0e, 0x53,     // mov $0x530e, %ax
  0x31, 0xdb,       // xor %bx, %bx
  0xb9, 0x02, 0x01,     // mov $0x0102, %cx
  0xcd, 0x15,       // int $0x15

  // Power down.
  0xb8, 0x07, 0x53,     // mov $0x5307, %ax
  0xbb, 0x01, 0x00,     // mov $0x0001, %bx
  0xb9, 0x03, 0x00,     // mov $0x0003, %cx
  0xcd, 0x15,       // int $0x15
};

static unsigned long long
rmode_gdt_entries [3] = {
  0x0000000000000000ULL,  /* Null descriptor */
  0x00009a000000ffffULL,  /* 16-bit real-mode 64k code at 0x00000000 */
  0x000092000100ffffULL /* 16-bit real-mode 64k data at 0x00000100 */
};

static struct {
  unsigned short       size __attribute__ ((packed));
  unsigned long long * base __attribute__ ((packed));
}
  rmode_gdt = { sizeof (rmode_gdt_entries) - 1, rmode_gdt_entries },
  rmode_idt = { 0x3ff, 0 };

static unsigned char jump_to_bios [] = {
  0xea, 0x00, 0x00, 0xff, 0xff    /*    ljmp  $0xffff,$0x0000  */
};

void _kapm_rmode(unsigned char *code, int length) {
  /* Reboot the system */
  asm("cli");
  memcpy ((void *) (0x1000 - sizeof (rmode_switch)),
	  rmode_switch, sizeof (rmode_switch));
  memcpy ((void *) (0x1000), code, length);

  __asm__ __volatile__ ( "lidt %0" : : "m"(rmode_idt) );

  // Setup GDT for real mode.
  __asm__ __volatile__ ( "lgdt %0" : : "m"(rmode_gdt) );

  // Load data segment registers.
  __asm__ __volatile__ (
    "movl $0x10, %%eax\n"
    "movl %%eax, %%ds\n"
    "movl %%eax, %%es\n"
    "movl %%eax, %%fs\n"
    "movl %%eax, %%gs\n"
    "movl %%eax, %%ss\n"
    : : : "eax"
  );
  // It disables paging and the cache, switches to real mode, and jumps to the BIOS reset entry point.
  __asm__ __volatile__ (
    "ljmp $0x0008, %0\n"
    : : "i"((void *)(0x1000-sizeof(rmode_switch)))
  );

  for(;;)
    asm("cli\nhlt\n");
}

void _kapm_reboot() {
  /* Reboot the system */
  
  asm("cli");
  if (!apm_setup.memory_check) {
    /* Write 0x1234 to absolute memory location 0x472.  The BIOS reads
     * this on booting to tell it to "Bypass memory test (also warm
     * boot)".  This seems like a fairly standard thing that gets set by
     * REBOOT.COM programs, and the previous reset routine did this
     * too. *((unsigned short *)0x472) = 0x1234;
     */
    asm("movw $0x1234,0x472");
  }

  if (apm_setup.kb_reboot) {
    /* Try to reboot the system without switch to real mode and jump to BIOS */
    int i;
    for (i=0;i<100;i++)
      outportb(0xfe,0x64); //Pulse reset low from keyboard
    _kwarning("Unable to pulse reset low. Reboot through the BIOS."); //It didn't work
  }
  _kapm_rmode(jump_to_bios, sizeof(jump_to_bios)); //Switch to real mode and jump to BIOS
}

void _kapm_shutdown() {
  /* Shutdown the system
   * It calls the power off by switching to real mode
   */
  _kapm_rmode(rmode_poweroff, sizeof(rmode_poweroff));
}
