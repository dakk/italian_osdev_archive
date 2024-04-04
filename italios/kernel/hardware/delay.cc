/*
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

#include <irq.h>
#include <pit.h>
#include <cstddef>
#include <delay.h>

static dword delay_count = 1;


void __delay(dword loops)
{
	dword c;
	for (c = 0; c < loops; c++);
}

void delay(dword milliseconds)
{
	__delay(milliseconds * delay_count);
}



#define MILLISEC 10		/* Prendo 10 ms come rate del timer */
#define FREQ (1000/MILLISEC)	/* frequenza del timer */

static volatile dword ticks = 0;

void delayCalibInt(void)
{
	kout << "\nprima\n";
	ticks++;		/* Incremento il contatore dei tick */
	kout << "\ndopo\n";
}


#define PRECISION 23		/* Calibration precision          */

dword calibrateDelayLoop(void)
{
	dword prevtick;
	dword i;
	dword calib_bit;

	kout << "\nsto per aggiungere l'irq\n";
	add_irq(delayCalibInt, 0);
	kout << "\ncreo l'oggetto del pit\n";
	pit_count mypit;
	kout << "\nsetto la frequenza\n";
	mypit.set_frequenza(FREQ);
	kout << "\nlo lancio\n";
	mypit.carica();
	kout << "\nabilito l'irq\n";
	enable_irq(0);

	// Fase 1:  Calibrazione grossolana
	kout << "\nPrima fase\n";

	do {
		delay_count <<= 1;	// il successivo valore di delay_count da provare

		prevtick = ticks;	// Aspetto che passa arriva il prossimo interrupt
		while (prevtick == ticks);

		prevtick = ticks;	// Da qui inizia la misurazione
		__delay(delay_count);	// faccio il delay
	} while (prevtick == ticks);	// aspetto che il delay è abbastanza grande

	delay_count >>= 1;	// prendo il valore inferiore

	kout << "\nseconda fase\n";

	// Fase 2:  Raffiniamo il calibramento

	calib_bit = delay_count;	// I bit che useremo per il test

	for (i = 0; i < PRECISION; i++) {
		calib_bit >>= 1;	// Il successivo bit da calibrare
		if (!calib_bit)
			break;	// Se i bit sono finit usciamo

		delay_count |= calib_bit;	// setto i bit in delay_count     

		prevtick = ticks;	// aspetto il prossimo interrupt
		while (prevtick == ticks);

		prevtick = ticks;	// Ora inizio la misurazione
		__delay(delay_count);	// Faccio il delay

		if (prevtick != ticks)	// Se un tick è passato 
			delay_count &= ~calib_bit;	// metto i nuovi bit calibrati a 0

	}

	// Bene abbiamo finito

	disable_irq(0);		//disababilitiamo il pit

	delay_count /= MILLISEC;	// Calcoliamo il delay_count per 1 ms

	return delay_count;
}
