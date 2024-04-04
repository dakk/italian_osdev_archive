/*
 * OSSO - A Micro Kernel OS
 * Copyright (c) 2000 Alessandro Iurlano.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */


#include <printf.h>
#include <processes.h>
#include <memory.h>

int main (void) {
  meminfo * mi;
  mi=get_mem_info();
  printf("Global System Mem : %d bytes (%dMB)\n",mi->total,mi->total/1048576);
  printf("Total Memory      : %d bytes (%dMB)\n",mi->num_pages*4096,mi->num_pages*4/1024);
  printf("Physical Memory   : %d bytes (%dMB)\n",mi->mem_pages*4096,mi->mem_pages*4/1024);
  printf("Swap Memory       : %d bytes (%dMB)\n",mi->swap_pages*4096,mi->swap_pages*4/1024);
  printf("Free Memory       : %d bytes (%dMB)\n",mi->free_pages*4096,mi->free_pages*4/1024);
  freeosso(mi);
  
}
