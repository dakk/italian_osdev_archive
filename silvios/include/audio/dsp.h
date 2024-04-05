/*
ItalianOs

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

#ifndef _DSP_H

#define _DSP_H

unsigned short int check_dsp();
//restituisce l'indirizzo base della scheda e lo resetta

unsigned short int reset_dsp(const unsigned short int);
//resetta il chip e restituisce 1 se è stato resettato il parametro da passargli
//è l'indirizzo base della porta

unsigned short int read_dsp(const unsigned short int);
//legge dal dsp. parametro indirizzo base della porta

void write_dsp(unsigned short int, const unsigned short int);
//scrive sul dsp. Parametri dato da scrivere (8 o 16 bit) e indirizzo base

unsigned short int get_interrupt_status(const unsigned short int);
//legge lo stato degli interrupt della scheda
//parametro indirizzo base della scheda

//funzioni per l'ack degl'interrupt
void acknowledge_mpu(const unsigned short int);//indirizzo della porta mpu 401
void acknowledge_dma_16(const unsigned short int);//base della scheda
void acknowledge_dma_8(const unsigned short int);//base della scheda
void init_audio(void);

//comandi per il DSP
//speaker (casse)
void speakerON(const unsigned short int);//base
void speakerOFF(const unsigned short int);//base
unsigned short int speaker_status(const unsigned short int);//base
//fine speaker (casse)


float dsp_version(const unsigned short int);


void send_8bit_dir_data(unsigned short int, unsigned short int);

void send_8bit_single_cycle_data(unsigned short int, unsigned short int);

void auto_init_DMA_8bit_out(unsigned short int);

unsigned short int receive_8bit_dir_data(unsigned short int);

unsigned short int receive_8bit_single_cycle_data(unsigned short int);

void auto_init_DMA_8bit_in(unsigned short int);

//fine comandi
#endif
