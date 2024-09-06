/*
 * Dreamos
 * misc.h
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

#include <misc.h>
#include <video.h>

// Eflags functions
unsigned int get_eflags() {
  /* Return the EFLAGS register value */
  int eflags;
  asm(
      "pushfl\n"
      "popl %0\n"
      : "=r"(eflags)
      );
  return eflags;
}

void set_eflags(int eflags) {
  /* Set the EFLAGS register value with the given one */
  asm(
      "pushl %0\n"
      "popfl\n"
      : : "g"(eflags)
      );
}

unsigned int chk_eflags(int flag) {
  /* Check if a given flag is set or cleared in the EFLAGS register
   * The function won't modify the EFLAGS value at the end
   */
  int eflags = get_eflags();
  int dflags;
  set_eflags(eflags|flag);
  dflags = get_eflags();
  set_eflags(eflags);
  if (dflags & flag) return 1;
  else return 0;
}

// Internal kernel messages
void _kerror(char *error) {
  /* Just write an error on the screen related to the kernel */
  _kcolor(COL_RED);
  _kputs("[KERNEL ERROR] ");
  _kcolor(COL_WHITE);
  _kputs(error);
}

void _kwarning(char *warning) {
  /* Just write a warning on the screen related to the kernel */
  _kcolor(COL_ORANGE);
  _kputs("[KERNEL WARNING] ");
  _kcolor(COL_WHITE_L);
  _kputs(warning);
}
