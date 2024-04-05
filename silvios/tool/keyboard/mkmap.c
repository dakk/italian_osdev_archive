#include <stdio.h>
#include <stdlib.h>

#define SVUOTA_BUF while(getchar() != '\n')

#define SCANCODE 103
#define COMBINAZIONI 10

unsigned char map[SCANCODE+1][COMBINAZIONI+1];

#define set_vet(colonna, valore) {int i; for(i=1; i<=10;i++) map[colonna][i] = valore;} 

int main(){
	char nome[10];
	FILE *mappa;
	int i,j, k;
	unsigned int sc_int[] = {16,25,30,38,44,50}; //importante: cambiare la linea che specfica il numero di elementi + in basso
	unsigned int sc[] = {86, 55, 12, 13, 26, 27, 39, 40, 41, 51, 52, 53};
	unsigned char inizio, fine;
	unsigned char map[SCANCODE+1][COMBINAZIONI+1];
	printf("\nCiao, benvenuto nell'utility del silvios per creare mappe dalla tastiera");
	printf("\nInserisci il nome per la nuova mappa della tastiera:");
	scanf("%s", &nome);
	SVUOTA_BUF;
	mappa = fopen(nome, "w");
	if (mappa == NULL){
		fprintf(stderr, "\nERRORE: impossibile creare o aprire il file");
		return -1;
	}
	//inizializzo la matrice della tastiera con gli elementi tutti a zero
	for (i = 1; i <= SCANCODE; i++)
		for (j = 1; j <= COMBINAZIONI; j++)
			map[i][j] = 0;

	


	printf("\nOra devi inserire il tasto corriscondente allo scancode.\nDopo per perfezionare la mappa modifica direttamente il file"); 

	//inizio con i caratteri
	for (i = 0; i < 6; ){
		inizio =  sc_int[i++];
		fine = sc_int[i++];
		for (j= inizio; j <= fine; j++){
			printf("\n\nLo scancode in questione e' il %d", j);

			printf("\nNORMALE: ");
			scanf("%c", &map[j][1]);
			SVUOTA_BUF;

			printf("\nSHIFT: ");
			scanf("%c", &map[j][2]);
			SVUOTA_BUF;

			map[j][3] = map[j][4] = map[j][7] = map[j][10] = map[j][1];

			map[j][5] =	map[j][6] = map[j][8]= map[j][9] = map[j][2];

		}
	}

	//poi faccio i numeri
	for (j= 2; j <= 11; j++){
			printf("\n\nLo scancode in questione e' il %d", j);

			printf("\nNORMALE: ");
			scanf("%c", &map[j][1]);
			SVUOTA_BUF;

			printf("\nSHIFT: ");
			scanf("%c", &map[j][2]);
			SVUOTA_BUF;

			map[j][3] = map[j][4] = map[j][7] = map[j][9] = map[j][1];

			map[j][5] =	map[j][6] = map[j][8]= map[j][10] = map[j][2];
	}

	//poi il tastierino
	for (j= 71; j <= 83; j++){
		printf("\n\nLo scancode in questione e' il %d", j);

		printf("\nNORMALE: ");
		scanf("%c", &map[j][1]);
		SVUOTA_BUF;

		map[j][3] = map[j][4] = map[j][7] = map[j][9] =	map[j][5] =	map[j][6] = map[j][8]= map[j][10] = map[j][2] = map[j][1];
	}

	//infine tutti gli altri tasti	
	for (i = 0; i < 12; ){
			j = sc[i++];
			printf("\nLo scancode in questione e' il %d", j);

			printf("\nNORMALE: ");
			scanf("%c", &map[j][1]);
			SVUOTA_BUF;

			printf("\nSHIFT: ");
			scanf("%c", &map[j][2]);
			SVUOTA_BUF;

			printf("\nALT: ");			
			scanf("%c", &map[j][4]);
			SVUOTA_BUF;

			map[j][9] = map[j][1];

			map[j][10] = map[j][2];
	}

	//invio
	set_vet(28, 10);

	//back-space
	set_vet(14, 8);

	//tab
	set_vet(15, '\t');

	//space
	set_vet(57, ' ');


	fprintf(mappa, "\n/*file mappa tastiera autogenerato*/");
	fprintf(mappa, "\nunsigned char %s[%d][%d];", nome, SCANCODE+1, COMBINAZIONI+1);
	fprintf(mappa, "\n/*	1	codice ascii*/");
	fprintf(mappa, "\n/*	2	shift*/");
	fprintf(mappa, "\n/*	3	ctrl*/");
	fprintf(mappa, "\n/*	4	alt*/");
	fprintf(mappa, "\n/*	5	shift+alt*/");
	fprintf(mappa, "\n/*	6	shift+ctrl*/");
	fprintf(mappa, "\n/*	7	ctrl+alt*/");
	fprintf(mappa, "\n/*	8	shift+ctrl+alt*/");
	fprintf(mappa, "\n/*	9	caps-lock*/");
	fprintf(mappa, "\n/*	10	shift+caps-lock*/");
	fprintf(mappa, "\n\n\n\nvoid init_map(void){");
	for (i=1; i<= SCANCODE; i++){
		fprintf(mappa, "\n");
		for( j=1; j <= COMBINAZIONI; j++)
			fprintf(mappa,"\n	%s[%d][%d]=%d;", nome, i, j, map[i][j]);
	}
	fprintf(mappa, "\n	return;\n}\n");
	fclose(mappa);

	return 0;
}



		

