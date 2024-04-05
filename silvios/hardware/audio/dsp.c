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

#include <io.h>
#include <stdio.h>
#include <delay.h>
#include <audio/dsp.h>
#include <audio/const.h>
#include <8259.h>
#include <idt.h>
#include <gdt.h>

//extern void reset(void);
//per l'irq
extern void audio_ISR();

float dspversion = 0;

void init_audio()
{
	set_vector(audio_ISR, M_VEC + 5, (D_INT + D_PRESENT + D_DPL1));
	enable_irq(5);
}

//fine
void udelay()
{
	int i = 0;
	if (i)
		i *= 4;
	else
		i *= 4;
}

unsigned short int check_dsp()
{
	unsigned short int val[] =
		{ BASE2, BASE1, BASE3, BASE4, BASE5, NO_CHIP };
	static short int count = sizeof(val) / sizeof(*val) - 1;	// don't test no_chip
	unsigned short int i = 0;

	// for each possible port
	for (i = 0; i < count; i++) {
		// test it by attempting to reset the DSP
		/*printf("\nSto entrando per il resettamento del dsp...."); */
		if (reset_dsp(val[i]))
			// if found, break now
			break;
	}

	// base io address, or, if not found, NO_CHIP, since i=countS
	return val[i];
}

unsigned short int reset_dsp(const unsigned short int base)
{
	int loopc;
	outportb(base + DSP_RESET, 1);
	udelay();
	//tenmicrosec ();
	outportb(base + DSP_RESET, 0);
	/*tenmicrosec ();
	 * tenmicrosec ();
	 * tenmicrosec (); */
	udelay();
	udelay();
	udelay();

	for (loopc = 0; loopc < 1000 && !(inportb(base + DSP_READ_BUFFER_STATUS) & 0x80); loopc++);	/* Wait for data
													 * available status */

	if (inportb(base + DSP_READ_DATA_PORT) != 0xAA)
		return 0;	/* Sorry */

	return 1;

}

unsigned short int read_dsp(const unsigned short int base)
{
	while ((inportb(base + DSP_READ_BUFFER_STATUS)
		&& DATA_AVAIBLE) == 0);
	return inportb(base + DSP_READ_DATA_PORT);
}

void write_dsp(unsigned short int base, const unsigned short int dato)
{
	while ((inportb(base + DSP_WRITE_BUFFER_STATUS)
		&& DATA_AVAIBLE) != 0);
	outportb(base + DSP_WRITE_COMMAND_DATA, dato);
}

unsigned short int get_interrupt_status(const unsigned short int base)
{
	outportb(base + MIXER_CHIP_REGISTER_ADDRESS_PORT,
		 INTERRUPT_STATUS_REGISTER);
	switch (inportb(base + MIXER_CHIP_DATA_PORT)) {
	case INT_DMA8:
		return INT_DMA8;
	case INT_DMA16:
		return INT_DMA16;
	case INT_MPU4O1:
		return INT_MPU4O1;
	default:
		return NO_INT;
	}
}

void acknowledge_mpu(const unsigned short int basempu)
{
	inportb(basempu);
}

void acknowledge_dma_16(const unsigned short int base)
{
	inportb(base + DSP_ACKNOWLEDGE_DMA_16);
}

void acknowledge_dma_8(const unsigned short int base)
{
	inportb(base + DSP_READ_BUFFER_STATUS);
}



//funzione per l'IRQ 5 della scheda audio
void irq5(void)
{
	/* signal operation finished */
//   done = TRUE;

	/* EOI the PIC */
//   outportb(0x20,0x20);
}

asm(".globl audio_ISR         \n" "audio_ISR:               \n" "   pusha               \n"	/* Save all registers               */
    "   pushw %ds           \n"	/* Set up the data segment          */
    "   pushw %es           \n" "   pushw %ss           \n"	/* Note that ss is always valid     */
    "   pushw %ss           \n" "   popw %ds            \n" "   popw %es            \n" "                       \n" "   call irq5           \n" "                       \n" "   popw %es            \n" "   popw %ds            \n"	/* Restore registers                */
    "   popa                \n" "   iret                \n"	/* Exit interrupt                   */
	);



float dsp_version(const unsigned short int base)
{
	float ret;
	outportb(base + DSP_WRITE_COMMAND_DATA, DSP_VER);
	ret = (read_dsp(base) + (read_dsp(base) / 100));
	dspversion = ret;
	return ret;
}

void speakerON(const unsigned short int base)
{
	outportb(base + DSP_WRITE_COMMAND_DATA, SPKON);
}
void speakerOFF(const unsigned short int base)
{
	outportb(base + DSP_WRITE_COMMAND_DATA, SPKOFF);
}
unsigned short int speaker_status(const unsigned short int base)
{
	if (dspversion >= 2.00) {
		outportb(base + DSP_WRITE_COMMAND_DATA, SPKSTATUS);
		return (read_dsp(base) == 0xFF) ? 1 : 0;
	}
}

void send_8bit_dir_data(unsigned short int base, unsigned short int value)
{
	outportb(base + DSP_WRITE_COMMAND_DATA, SINGLEBYTEOUT);
	write_dsp(base, value);
}

void send_8bit_single_cycle_data(unsigned short int base,
				 unsigned short int value);

void auto_init_DMA_8bit_out(unsigned short int base)
{
	outportb(base + DSP_WRITE_COMMAND_DATA, AUTOINITDMAOUT);
}

unsigned short int receive_8bit_dir_data(unsigned short int base)
{
	outportb(base + DSP_WRITE_COMMAND_DATA, SINGLEBYTEIN);
	return read_dsp(base);
}

unsigned short int receive_8bit_single_cycle_data(unsigned short int base);

void auto_init_DMA_8bit_in(unsigned short int base)
{
	outportb(base + DSP_WRITE_COMMAND_DATA, AUTOINITDMAIN);
}
