/*
 * Dreamos
 * floppy.h
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
  * Autore Ivan Gualandri
  * Prima versione : 24/07/2004
  */

#ifndef __FLOPPY_H
#define __FLOPPY_H

#define FLOPPY_INFOS 0x10
#define START_MOTOR 0x14
#define STOP_MOTOR 0x00

#define FDC_DOR 0x3f2     //Digital Output register
#define FDC_DRS  (0x3f4)	/* Data Rate Select Register (output) */
#define FDC_MSR 0x3f4
#define FDC_DATA 0x3f5

void detect_floppy();
void start_motor();
void stop_motor();
void floppy_irq();
void init_floppy();
void check_version();
int sendbyte(int);
int getbyte();
#endif
