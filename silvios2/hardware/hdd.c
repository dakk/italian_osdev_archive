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

#include <video.h>
#include <stdio.h>
#include <io.h>
#include <hdd.h>



char *getascii(unsigned short in_data[], unsigned short off_start, unsigned
	       short off_end)
{
	static char ret_val[255];
	int loop, loop1;

	for (loop = off_start, loop1 = 0; loop <= off_end; loop++) {
		ret_val[loop1++] = (char) (in_data[loop] / 256);	/* Get High byte */
		ret_val[loop1++] = (char) (in_data[loop] % 256);	/* Get Low byte */
	}
	ret_val[loop1] = '\0';	/* Make sure it ends in a NULL character */
	return (ret_val);
}


/************************************************************/
/* A procedure to check wether any hdds exist in the system */
/*							    */
/* Input: None						    */
/* Output Integer, The number of hdds present in the system */
/************************************************************/
int check_hdds()
{
	/*int hdd_num;
	 * hdd_num=0;
	 * outportb(0X70, 0x12);
	 * hdd_num = (int *)inportb(0x70);
	 * return hdd_num; */

	unsigned short int dd[256];
	unsigned short int dd_off;
	unsigned short int drive;
	unsigned long int wait_loop;
	unsigned short int base = 0;
	unsigned short int in_val;

	for (drive = 0; drive < 8; drive++) {	/* ciclo per tutti i driver possibili */
		/* vedo da dove inizia il dirver */

		switch (drive / 2) {
		case 0:
			base = 0x1f0;
			break;
		case 1:
			base = 0x170;
			break;
		case 2:
			base = 0x1e8;
			break;
		case 3:
			base = 0x168;
			break;
		}

		/* aspetto che il controller non è occupato */
		wait_loop = 100000;
		while (--wait_loop > 0) {
			in_val = inportb(base + 7);
			if (((in_val & 0x40) == 0x40)
			    || ((in_val & 0x00) == 0x00))
				break;
		}

		if (wait_loop < 1)
			continue;

		outportb(base + 6, ((drive % 2) == 0 ? 0xA0 : 0xB0));	/* vedo se è master/slave */

		outportb(base + 7, 0xEC);	/* prendo infor azioeni sui dati */

		/* aspetto che i dati sono pronti */
		wait_loop = 100000;
		while (--wait_loop > 0) {
			in_val = inportb(base + 7);
			if ((in_val & 0x48) == 0x48)
				break;
			if ((in_val & 0x01) == 0x01)	/* errore nel driver */
				break;
		}

		if ((wait_loop < 1) || ((in_val & 0x01) == 0x01))	/* errore di time out */
			continue;

		for (dd_off = 0; dd_off != 256; dd_off++)	/* leggo tutti i dati */
			dd[dd_off] = inportw(base);


		switch (drive / 2) {
		case 0:
			printf("\nPrimario ");
			break;
		case 1:
			printf("\nSecondario ");
			break;
		case 2:
			printf("\nTerziario ");
			break;
		case 3:
			printf("\nQuaternario ");
			break;
		}

		printf("Controller - ");

		switch (drive % 2) {
		case 0:
			printf("Master");
			break;
		case 1:
			printf("Slave");
			break;
		}

		printf(" drive\n");

		printf("\nModel Number______________________: %s",
		       getascii(dd, 27, 46));
		printf("\nSerial Number_____________________: %s",
		       getascii(dd, 10, 19));
		printf("\nController Revision Number________: %s",
		       getascii(dd, 23, 26));
		if (dd[80] != 0 && dd[80] != 0xFFFF)
			printf("\nATA Version Number________________: %d.%d",
			       dd[80], (dd[81] == 0xFFFF ? 0 : dd[81]));
		printf("\nTrasferimneto di una doppia word___: %s",
		       (dd[48] == 0 ? "No" : "Si"));
		printf("\nGrandezza del buffer dell'hd(bytes)____: %d",
		       dd[21] * 512);
		printf("\nVelocita' di trasferimento____________________: ");
		if (dd[0] & 0x0400)
			printf("> 10 Mbit/sec");
		if (dd[0] & 0x0200)
			printf("5-10 Mbit/sec");
		if (dd[0] & 0x0100)
			printf("< 5 Mbit/sec");
		printf("\nTipo di driver________________________: ");
		if (dd[0] & 0x0080)
			printf("Rimovibile");
		if (dd[0] & 0x0040)
			printf("Fisso");
		printf("\nSupporto modalita' LBA________________: %s",
		       ((dd[49] & 0x0200) ? "Si" : "No"));
		printf("\nSupporto DMA_____________________: %s",
		       ((dd[49] & 0x0100) ? "Si" : "No"));
		/*printf("Number of ECC bytes transferred___: %d\n", dd[22]); */
		printf("\nSettori trasferiti ad ogni interruzione___: %d",
		       dd[47] % 256);
		printf("\nNumero di ciclindri_______________: %d", dd[1]);
		printf("\nNumero di testine___________________: %d", dd[3]);
		printf("\nNumberi di Settori per Traccia_______: %d", dd[6]);
		//printf("\nDimensione di tutto il disko_______: %d bytes\n", dd[6]*dd[3]*dd[1]*512);
	}

	return 1;
}
