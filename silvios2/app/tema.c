#include <stdio.h>
#include <video.h>

void tema(void)
{
	int numero;
	printf("\nTemi disponibili:");
	printf("\n 1  natale : sfondo rosso + testo giallo");
	printf("\n 2  standard : sfondo blu + testo giallo");
	printf("\n 3  dos : sfondo nero + testo bianco");
	printf("\n 4  commodore: sfondo verde + testo bianco");
	printf("\n 5 stavagante : sfondo viola + testo giallo");
	printf("\n 6 Professionale : sfondo nero + testo verde");
	printf("\n 7 Professionale2 : sfondo nero + testo celestino");
	printf("\nInserisci il numero del tema che ti interessa:");

	numero = get_number();
	switch (numero) {
	case 1:
		set_back_color(ROSSO);
		cls();
		set_text_color(GIALLO);
		break;
	case 2:
		set_back_color(BLU);
		cls();
		set_text_color(GIALLO);
		break;
	case 3:
		set_back_color(NERO);
		cls();
		set_text_color(BIANCO);
		break;
	case 4:
		set_back_color(VERDE);
		cls();
		set_text_color(BIANCO);
		break;
	case 5:
		set_back_color(VIOLA);
		cls();
		set_text_color(GIALLO);
		break;
	case 6:
		set_back_color(NERO);
		cls();
		set_text_color(VERDE);
		break;
	case 7:
		set_back_color(NERO);
		cls();
		set_text_color(CELESTINO);
		break;
	default:
		printf("\nTema non trovato");
	}
	return;

}
