#include <stdio.h>
#include <stdlib.h>

#define PRIMO 'X'
#define SECONDO 'O'
#define VUOTO ' '

#define MAX_X 3
#define MAX_Y 3

#define casella_t char
//#define tavolo_t casella_t[MAX_X][MAX_Y]
//typedef tavolo_t casella_t[MAX_X][MAX_Y] ;

void disegna_campo(casella_t tabel[MAX_X][MAX_Y]){
	printf("\033[2J");
	int i, j;
	printf("\n ");
	for(j=0; j < MAX_Y; j++)
		printf("\t%d\t", j);		
	for(i=0; i < MAX_X; i++){
		printf("\n  |");
		for(j=0; j < 53; j++)
			printf("%c", '-');
		printf("|\n%d |", i);
		for(j=0; j < MAX_Y; j++)
			printf("\t%c\t|", tabel[i][j]);		
	}
			printf("\n  |");
		for(j=0; j < 53; j++)
			printf("%c", '-');
		printf("|\n");
}

void inizializza(casella_t tabel[MAX_X][MAX_Y]){
		int i, j;
	for(i=0; i < MAX_X; i++)
		for(j=0; j < MAX_Y; j++)
			tabel[i][j] = VUOTO;
}

int cerca_vittoria(casella_t tabel[MAX_X][MAX_Y]){
	int simbolo;
	int winner, i, j ;
	//cerco prima in orizontale
	for(i=0; i < MAX_X; i++){
		winner = ~0;
		for(j=0; j < MAX_Y; j++){
			winner &= tabel[i][j];
		}
		if((winner == PRIMO) || (winner==SECONDO))
			return winner;
	}	
	//cerco in verticale
	for(i=0; i < MAX_X; i++){
		winner = ~0;
		for(j=0; j < MAX_Y; j++){
			winner &= tabel[j][i];
		}
		if((winner == PRIMO) || (winner==SECONDO))
			return winner;
	}	
	
	//cerco in obliquo
	if((tabel[0][0] == tabel[1][1]) &&  (tabel[1][1] == tabel[2][2]) && (tabel[1][1] != VUOTO))
		return tabel[0][0];
	if ((tabel[0][2] == tabel[1][1]) &&  (tabel[1][1] == tabel[2][0])  && (tabel[2][0] != VUOTO))
		return tabel[0][2];
	return 0;
}

int vedi_se_tutto_pieno(casella_t tabel[MAX_X][MAX_Y]){
	int i, j;
	for(i=0; i < MAX_X; i++)
		for(j=0; j < MAX_Y; j++)
			if(tabel[i][j] == VUOTO)
				return 0;
	return 1;
}

int main(){	
	casella_t tabel[MAX_X][MAX_Y];
	char gioc[2][20];
	int riga, colonna;
	char turno;
	int vuoto;
	char vinto = 0;
	inizializza(tabel);
	printf("\nInserisci il nome del primo giocatore: ");
	scanf("%s", gioc[0]);
	printf("\nContro chi gioca %s? ", gioc[0]);
	scanf("%s", gioc[1]);
	turno = 0;
	while(!vinto){
		disegna_campo(tabel);
		vuoto = 0;
		do{
			do{
				printf("%s inserisci la riga: ", gioc[turno]);
				scanf("%d", &riga);
			}while(riga >= MAX_X);
			do{
				printf("%s inserisci la colonna: ", gioc[turno]);
				scanf("%d", &colonna);
			}while(colonna >= MAX_Y);
		}while(tabel[riga][colonna] != VUOTO);
		if(turno == 0)
			tabel[riga][colonna] = PRIMO;
		else
			tabel[riga][colonna] = SECONDO;
		vinto = cerca_vittoria(tabel);
		if(vedi_se_tutto_pieno(tabel)){
			printf("\nnon ha vinto nessuno!!!\n");
			return 0;
		}
		turno = !turno;
	}
	disegna_campo(tabel);
	printf("\nHa vinto ");
	if(vinto == PRIMO)
		printf("%s", gioc[0]);
	else
		printf("%s", gioc[1]);
	printf("!!!\n");
	return 0;
}
