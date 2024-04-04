/*
ItaliOs
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <magic_code.h>
#include <string.h>

static struct magic_code code[20];	//vettore dei codici
static int num_code;		//indica il numero di codici carivcati

/*
   struct magic_code{
   int number;
   char descrizione[10];
   }
 */

void init_magic_code(void)
{
	code[1].number = DIRECTORY;
	strcpy(&(code[1].descrizione), "CARTELLA");

	code[2].number = ESEGUIBILE;
	strcpy(&(code[2].descrizione), "PROGRAMMA");

	code[0].number = SCONOSCIUTO;
	strcpy(&(code[0].descrizione), "SCONOSCIUTO");

	code[3].number = ASCII;
	strcpy(&(code[3].descrizione), "ASCII");

	num_code = 4;

	return;
}

/*unsigned char *get_desc_code(unsigned int number)
   {
   int i;
   if (number >= num_code)
   return "SCONOSCIUTO";
   i = 0;
   while (code[i].number != number && ++i < num_code);
 */
