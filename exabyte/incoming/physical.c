/* physical.c */
/******************************************************************************
* Exabyte Operating System                                                    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; either version 2              *
* of the License, or (at your option) any later version.                      *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License           *
* along with this program; if not, write to the Free Software                 *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
*******************************************************************************/

#include <kernel.h>
#include <arch-ia32/mm/mem.h>
#include <mm/physical.h>

#define PAGE_SIZE				4096

#define DMA_MEMORY_SIZE			0x00100000 // 1 mb

unsigned long total_mem;
unsigned long total_pages;
unsigned long bitmap_pages;
unsigned long bitmap_longs;
unsigned long *bitmap;
unsigned long *bitmap_upper;
unsigned long dma_pages_used;
unsigned long dma_pages_free;
unsigned long pages_used;
unsigned long pages_free;

void mm_physical_init() {
	total_mem=ia32_get_total_mem();
	kprint("mm: found %u mb of memory\n",total_mem>>20);
	total_pages=total_mem/PAGE_SIZE;
	bitmap_pages=(total_pages/8)/PAGE_SIZE;
	if ((total_pages/8)%PAGE_SIZE)
		bitmap_pages++;
	bitmap_longs=total_pages/32;
	bitmap=(unsigned long *)g_end;
	bitmap_upper=(unsigned long *)((unsigned long)bitmap+(DMA_MEMORY_SIZE/PAGE_SIZE/8));
	kprint("mm: bitmap at 0x%08x, %u pages\n",(unsigned long)bitmap,bitmap_pages);
	unsigned int i;
	for (i=0; i<bitmap_longs; i++)
		*(unsigned long *)((unsigned long)bitmap+i)=0;
	pages_used=0;
	pages_free=total_pages-(DMA_MEMORY_SIZE/PAGE_SIZE);
	dma_pages_used=0;
	dma_pages_free=DMA_MEMORY_SIZE/PAGE_SIZE;
	mm_physical_reserve((unsigned long)g_start,(unsigned long)g_end-(unsigned long)g_start); // kernel
	mm_physical_reserve((unsigned long)bitmap,bitmap_pages*PAGE_SIZE); // bitmap
	mm_physical_reserve(0x400,0xFF); // bda
	mm_physical_reserve(0x9FC00,0x603FF); // ebda, vga framebuffers, video bioe e motherboard bios
	mm_physical_reserve(0xFEC00000,0x13FFFFF); // altre robe della motherboard
}

void mm_physical_reserve(unsigned long start,unsigned long size) {
	if (start>=total_mem)
		return;
	unsigned long start_page=start/PAGE_SIZE;
	unsigned char *pointer=(unsigned char *)((unsigned long)bitmap+(start_page/8));
	unsigned long cur_bit=start_page%8;
	unsigned long i;
	for (i=0; i<(size/PAGE_SIZE); i++) {
		*pointer|=(1<<cur_bit);
		cur_bit++;
		if (cur_bit>7) {
			pointer++;
			cur_bit=0;
		}
	}
}

unsigned long mm_physical_alloc_page(int dma) {
	unsigned long *pointer;
	unsigned long i;
	unsigned long max;
	unsigned long base;
	if (dma) {
		pointer=bitmap;
		base=0;
		max=DMA_MEMORY_SIZE/PAGE_SIZE/32;
	} else {
		pointer=bitmap_upper;
		base=DMA_MEMORY_SIZE;
		max=(total_mem-DMA_MEMORY_SIZE)/PAGE_SIZE/32;
	}
	i=0;
	while (i<max) {
		if (pointer[i]!=0xFFFFFFFF) {
			if (dma) {
				dma_pages_used++; dma_pages_free--;
			} else {
				pages_used++; pages_free--;
			}
			if (pointer[i]&0xFFFF0000) {
				if (pointer[i]&0xFF000000) {
					if (pointer[i]&0xF0000000) {
						if (pointer[i]&0xC0000000) {
							if (pointer[i]&0x80000000) {
								pointer[i]|=0x80000000;
								return (i*32+31)*PAGE_SIZE+base;
							} else { // 0x40000000
								pointer[i]|=0x40000000;
								return (i*32+30)*PAGE_SIZE+base;
							}
						} else { // 0x30000000
							if (pointer[i]&0x20000000) {
								pointer[i]|=0x20000000;
								return (i*32+29)*PAGE_SIZE+base;
							} else { // 0x10000000
								pointer[i]|=0x10000000;
								return (i*32+28)*PAGE_SIZE+base;
							}
						}
					} else { // 0x0F000000
						if (pointer[i]&0x0C000000) {
							if (pointer[i]&0x08000000) {
								pointer[i]|=0x08000000;
								return (i*32+27)*PAGE_SIZE+base;
							} else { // 0x04000000
								pointer[i]|=0x04000000;
								return (i*32+26)*PAGE_SIZE+base;
							}
						} else { // 0x03000000
							if (pointer[i]&0x02000000) {
								pointer[i]|=0x02000000;
								return (i*32+25)*PAGE_SIZE+base;
							} else { // 0x01000000
								pointer[i]|=0x01000000;
								return (i*32+24)*PAGE_SIZE+base;
							}
						}
					}
				} else { // 0x00FF0000
					if (pointer[i]&0x00F00000) {
						if (pointer[i]&0x00C00000) {
							if (pointer[i]&0x00800000) {
								pointer[i]|=0x00800000;
								return (i*32+23)*PAGE_SIZE+base;
							} else { // 0x00400000
								pointer[i]|=0x00400000;
								return (i*32+22)*PAGE_SIZE+base;
							}
						} else { // 0x00300000
							if (pointer[i]&0x00200000) {
								pointer[i]|=0x00200000;
								return (i*32+21)*PAGE_SIZE+base;
							} else { // 0x00100000
								pointer[i]|=0x00100000;
								return (i*32+20)*PAGE_SIZE+base;
							}
						}
					} else { // 0x000F0000
						if (pointer[i]&0x000C0000) {
							if (pointer[i]&0x00080000) {
								pointer[i]|=0x00080000;
								return (i*32+19)*PAGE_SIZE+base;
							} else { // 0x00040000
								pointer[i]|=0x00040000;
								return (i*32+18)*PAGE_SIZE+base;
							}
						} else { // 0x00030000
							if (pointer[i]&0x00020000) {
								pointer[i]|=0x00020000;
								return (i*32+17)*PAGE_SIZE+base;
							} else { // 0x00010000
								pointer[i]|=0x00010000;
								return (i*32+16)*PAGE_SIZE+base;
							}
						}
					}
				}
			} else { // 0x0000FFFF
				if (pointer[i]&0x0000FF00) {
					if (pointer[i]&0x0000F000) {
						if (pointer[i]&0x0000C000) {
							if (pointer[i]&0x00008000) {
								pointer[i]|=0x00008000;
								return (i*32+15)*PAGE_SIZE+base;
							} else { // 0x00004000
								pointer[i]|=0x80000000;
								return (i*32+14)*PAGE_SIZE+base;
							}
						} else { // 0x00003000
							if (pointer[i]&0x00002000) {
								pointer[i]|=0x00002000;
								return (i*32+13)*PAGE_SIZE+base;
							} else { // 0x00001000
								pointer[i]|=0x00001000;
								return (i*32+12)*PAGE_SIZE+base;
							}
						}
					} else { // 0x00000F00
						if (pointer[i]&0x00000C00) {
							if (pointer[i]&0x00000800) {
								pointer[i]|=0x00000800;
								return (i*32+11)*PAGE_SIZE+base;
							} else { // 0x00000400
								pointer[i]|=0x00000400;
								return (i*32+10)*PAGE_SIZE+base;
							}
						} else { // 0x00000300
							if (pointer[i]&0x00000200) {
								pointer[i]|=0x00000200;
								return (i*32+9)*PAGE_SIZE+base;
							} else { // 0x00000100
								pointer[i]|=0x00000100;
								return (i*32+8)*PAGE_SIZE+base;
							}
						}
					}
				} else { // 0x000000FF
					if (pointer[i]&0x000000F0) {
						if (pointer[i]&0x000000C0) {
							if (pointer[i]&0x00000080) {
								pointer[i]|=0x00000080;
								return (i*32+7)*PAGE_SIZE+base;
							} else { // 0x00000040
								pointer[i]|=0x00000040;
								return (i*32+6)*PAGE_SIZE+base;
							}
						} else { // 0x00000030
							if (pointer[i]&0x00000020) {
								pointer[i]|=0x00000020;
								return (i*32+5)*PAGE_SIZE+base;
							} else { // 0x00000010
								pointer[i]|=0x00000010;
								return (i*32+4)*PAGE_SIZE+base;
							}
						}
					} else { // 0x0000000F
						if (pointer[i]&0x0000000C) {
							if (pointer[i]&0x00000008) {
								pointer[i]|=0x00000008;
								return (i*32+3)*PAGE_SIZE+base;
							} else { // 0x00000004
								pointer[i]|=0x00000004;
								return (i*32+2)*PAGE_SIZE+base;
							}
						} else { // 0x00000003
							if (pointer[i]&0x00000002) {
								pointer[i]|=0x00000002;
								return (i*32+1)*PAGE_SIZE+base;
							} else { // 0x00000001
								pointer[i]|=0x00000001;
								return (i*32)*PAGE_SIZE+base;
							}
						}
					}
				}
			}
		}
		i++;
	}
	// per ora va in panic
	asm("cli");
	kprint("\npanic: out of physical memory\n");
	while (1);
}

void mm_physical_free_page(unsigned long pointer) {
	unsigned ptr_dword=pointer/PAGE_SIZE/32;
	unsigned ptr_bit=(pointer/PAGE_SIZE)%32;
	bitmap[ptr_dword]&=~(1<<ptr_bit);
}

unsigned long mm_physical_get_free_mem() { return pages_free*PAGE_SIZE; }
unsigned long mm_physical_get_used_mem() { return pages_used*PAGE_SIZE; }
unsigned long mm_physical_get_free_dma_mem() { return dma_pages_free*PAGE_SIZE; }
unsigned long mm_physical_get_used_dma_mem() { return dma_pages_used*PAGE_SIZE; }
