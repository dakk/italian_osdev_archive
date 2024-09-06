/*
 * Dreamos
 * floppy.c
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

#include <floppy.h>
#include <stdio.h>
#include <idt.h>
#include <irq.h>
#include <8259.h>
#include <io.h>

int motor;
char* floppy_strings[6] = {"Nessun Floppy presente","360kb da 5,25","1,2mb da 5,25","720kb da 3,5","1,44mb da 3.5", "2,88mb da 3,5"};

void init_floppy() {
//Prepara il lettore floppy per l'esecuzione
    outportb(0x00, FDC_DOR); //Spengo il controller
    motor= FALSE;   //Motore spento
    outportb(0x0c, FDC_DOR); //Avvio il controller
    enable_irq(0xBC, MASTER2_PIC);
    add_irq(floppy_irq, 6);
    check_version();
}

void check_version() {
//Sto controllando che versione di controller floppy ho
        int info;
        sendbyte(FLOPPY_INFOS);
        info = getbyte();
        if(info == 0x80)
            printf("\n     Rilevato controller nec765....\n");
        else if(info==0x90)
            printf("\n     Rilevato controller floppy esteso...\n");
        else
            printf("\n     Controller Sconosciuto\n");
}
void detect_floppy()
{
//Rileva la presenza di massimo 2 lettori floppy. Il vettore contiene le possibilita' offerte , verra' selezionata quella che riguarda il nostro lettore floppy.
//Le informazioni sono rilevate dal cmos
    unsigned char floppy_info;
    outportb(FLOPPY_INFOS, 0x70);
    floppy_info = inportb(0x71);
    printf("\nInformazioni su floppy 1: %s", floppy_strings[floppy_info>>4]);
    printf("\nInformazioni su floppy 2: %s\n", floppy_strings[floppy_info&0x0F]);
}

void floppy_irq() {
    //printf("E' arrivato qualcosa dal floppy\n");
}

void start_motor()
{
//Avvia il motore del floppy
    if(motor==FALSE) {
        outportb(0x1C,FDC_DOR);
        motor=TRUE;
    }
}

void stop_motor()
{
//ferma il motere del floppy
    if(motor==TRUE) {
        outportb(0x0c,FDC_DOR);
        motor=FALSE;
    }
}

int sendbyte(int databyte) {
//Spedisce un byte al controller
    int timeout;
    int msrbyte;
    for(timeout=0; timeout<128; timeout++) {  //E' un evento di timeout tutti usano 128 quindi anche io uso 128 :)
        msrbyte = inportb(FDC_MSR);   //Leggi il MSR
        if((msrbyte & 0xc0) == 0x80) { //E' disponibile il controller?
            outportb(databyte, FDC_DATA);  //Spedisci il dato
            return 0;
        }
    }
    inportb(0x80);
    return -1;
}

int getbyte(){
    int timeout;
    int msrb;
    for(timeout=0; timeout<128; timeout++) {
        msrb=inportb(FDC_MSR); //Leggi MSR
        if((msrb & 0xc0) == 0xc0) //E' disponibile il controller?
            return inportb(FDC_DATA);  //Restituisci il contenuto del data register
        inportb(0x80);  //piccolo delay
    }
    return -1;   //Timeout di lettura
}

int calibrate() {
    outportb(0x1c, FDC_DOR);
}
