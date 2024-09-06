/*
 * Dreamos
 * apm.h
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

/*
 * Author: Lethalman
 */

#ifndef __APM_H_
#define __APM_H_

void init_apm(); //Inizialize apm_setup values
void _kapm_reboot(); //Reboot the machine
void _kapm_shutdown(); //Shutdown the machine

struct {
  unsigned char memory_check; //Enable or disable memory check
  unsigned char kb_reboot; //If true, try to reboot by pulse reset low
} apm_setup;

#endif
