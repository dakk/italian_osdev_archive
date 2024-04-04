#include <forme_vga.h>


void rettangolo(int x1, int y1, int x2, int y2, int color)
{
	linea(x1, y1, x1, y2, color);
	linea(x1, y1, x2, y1, color);
	linea(x1, y2, x2, y2, color);
	linea(x2, y2, x2, y1, color);
}

void rettangolo_pieno(int x1, int y1, int x2, int y2, int color)
{
	int i;
	for (i = 0; i < y2 - y1; i++) {
		linea(x1, y1 + i, x2, y1 + i, color);
	}
}

//stampa una linea a video
void linea(int x1, int y1, int x2, int y2, int color)
{
	int i, deltax, deltay, numpixels,
		d, dinc1, dinc2, x, xinc1, xinc2, y, yinc1, yinc2;

	/* Calcoliamo il deltax ed il deltay della linea, ovvero il numero di pixel presenti a livello
	 * orizzontale e verticale. Utilizziamo la funzione abs() poichè a noi interessa il loro
	 * valore assoluto. */
	deltax = abs(x2 - x1);
	deltay = abs(y2 - y1);

	/* Adesso controlliamo se la linea è più "orizzontale" o "verticale", ed inizializziamo
	 * in maniera appropriate le variabili di comodo. */
	if (deltax >= deltay) {
		/* La linea risulta maggiormente schiacciata sull' ascissa */
		numpixels = deltax + 1;

		/* La nostra variabile decisionale, basata sulla x della linea */
		d = (2 * deltay) - deltax;

		/* Settiamo gli incrementi */
		dinc1 = deltay * 2;
		dinc2 = (deltay - deltax) * 2;
		xinc1 = xinc2 = 1;
		yinc1 = 0;
		yinc2 = 1;
	} else {
		/* La linea risulta maggiormente schiacciata sull' ordinata */
		numpixels = deltay + 1;

		/* La nostra variabile decisionale, basata sulla y della linea */
		d = (2 * deltax) - deltay;

		/* Settiamo gli incrementi */
		dinc1 = deltax * 2;
		dinc2 = (deltax - deltay) * 2;
		xinc1 = 0;
		xinc2 = 1;
		yinc1 = yinc2 = 1;
	}

	/* Eseguiamo un controllo per settare il corretto
	 * andamento della linea */
	if (x1 > x2) {
		xinc1 = -xinc1;
		xinc2 = -xinc2;
	}
	if (y1 > y2) {
		yinc1 = -yinc1;
		yinc2 = -yinc2;
	}

	/* Settiamo le coordinate iniziali  */
	x = x1;
	y = y1;

	/* Stampiamo la linea */
	for (i = 1; i < numpixels; i++) {
		putpixel(x, y, color);

		/* Scegliamo l' incremento del "passo" a seconda dellla
		 * variabile decisionale */
		if (d < 0) {
			d = d + dinc1;
			x = x + xinc1;
			y = y + yinc1;
		} else {
			d = d + dinc2;
			x = x + xinc2;
			y = y + yinc2;
		}
	}
}
