#include <stdio.h>
#include <video.h>

void text_colore(void)
{
	unsigned char colore;
	printf("\n Inserisci il numero del  colore desideri\n Numerori validi da 0 a 15: ");
	colore = get_number();
	if (colore <= 15)
		set_text_color(colore);
	else
		printf("\nERRORE: Colore non valido\n");
	printf("\n");
	return;
}

void back_colore(void)
{
	unsigned char colore;
	printf("\n Insrisci il numero del colore che desideri\n Numeri validi da 0 a  7: ");
	colore = get_number();
	if (colore <= 7) {
		set_back_color(colore);
		cls();
	} else {
		printf("\nERRORE: colore non valido\n");
	}
	return;
}



void list_color(void)
{
	printf("\n");
	printf("\n               NERO\t\t0");
	printf("\n               BLU\t\t1");
	printf("\n               VERDE\t\t2");
	printf("\n               CELESTE\t\t3");
	printf("\n               ROSSO\t\t4");
	printf("\n               VIOLA\t\t5");
	printf("\n               MARRONE\t\t6");
	printf("\n               BIANCO_SPORCO\t\t7");
	printf("\n               GRIGIO\t\t8");
	printf("\n               BLU_CHIARO\t\t9");
	printf("\n               VERDE_CHIARO\t\t10");
	printf("\n               CELESTINO\t\t11");
	printf("\n               ARANCIONE\t\t12");
	printf("\n               ROSA\t\t13");
	printf("\n               GIALLO\t\t14");
	printf("\n               BIANCO\t\t15");
	return;
}
