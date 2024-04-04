#include <stdio.h>
#include <speaker.h>


void nota(void)
{
	int freq, tempo;
	printf("\n");
	printf("Inserisci la frequenza: ");
	freq = get_number();
	if (freq <= 0) {
		printf("\nFrequenza non valida\n");
		return;
	}
	printf("\nInserisci il tempo: ");
	tempo = get_number();
	if (tempo <= 0) {
		printf("\ntempo non valido\n");
		return;
	}
	suona(freq, 60, tempo * 250);
	printf("\n");
	return;
}
