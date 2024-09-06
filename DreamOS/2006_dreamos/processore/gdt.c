/*
 * Dreamos
 * gdt.c
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

/** TODO: CONTROLLARE struttura dati del gdt descriptor*/

#include <gdt.h>
#include <stdio.h>
#include <mydef.h>

gdt_desc gdt_table[256];

void init_GDT(){
/** @author Ivan ^Inuyasha^ Gualandri
 *  @param none.
 *  !Inizializza una GDT di default.
 */
    setDescriptor(0, 0x00);
    setDescriptor(1, 0x120);
    setDescriptor(2, 0x140);
    setDescriptor(3, 0x160);
    printf("Value of gdt_table[0]: %d : %d\n", gdt_table[0].gdt_descriptor[0], gdt_table[0].gdt_descriptor[1]);
    printf("Value of gdt_table[1]: %d : %d\n", gdt_table[1].gdt_descriptor[0], gdt_table[1].gdt_descriptor[1]);

    /*Dovro' mettere un data segment e un code segment. Quindi 2 descrittori*/
}

void load_GDT(){
 /** @author Ivan ^Inuyasha^ Gualandri
   * @param none.
   * !Carico una GDT
   */
}

void setDescriptor(int desc_number, unsigned int limit){
/** @author Ivan ^Inuyasha^ Gualandri
 *  @param desc_number: Numero del descrittore
 *  @param limit: limite di memoria.
 *  !Questa funzione modifica un descrittore della GDT.
 */
 gdt_table[desc_number].gdt_descriptor[0]=limit;
 gdt_table[desc_number].gdt_descriptor[0]=limit;
}

gdt_desc makeNullDesc(int desc_number){
    gdt_desc tmp_desc;
    tmp_desc.gdt_descriptor[0] = NULL;
    tmp_desc.gdt_descriptor[1] = NULL;
    return tmp_desc;
}

void makeGDTDescriptor(unsigned int base, unsigned int limit, unsigned char DPL) {

}