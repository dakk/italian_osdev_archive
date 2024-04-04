/***************************************************************************
                 roman - roman numers conversion program for Italios
                             -------------------
    begin                : lun mar 22 13:51:03 CET 2004
    copyright            : (C) 2004 by Nicola Gigante
    email                : gigabytes@email.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>

int num(char *numeral);
int rtoi(char c);
int rcmp(char c1, char c2);

int main()
{
	char numeral[80];
	printf("ItaRoman v. 1.0\nCopyright (C) 2004 Nicola Gigante\n");
	printf("Questo e` software libero\nPuoi copiarlo e/o ridistribuirlo\nsotto i termini della GNU General Public License (GPL)\n");
	printf("Inserisci un numero romano: \n");
	printf("Prima della scanf()\n");
	scanf("%s", numeral);
	printf("Dopo la scanf()\n");
	printf("Il corrispondente decimale e`: %d\n", num(numeral));
	
	return 0;
}


int num(char *numeral)
{
  printf("Funzione num()\n");
	int res = 0;
	char *cur = numeral, *next = numeral + 1, *prev = NULL;
	
	res += rtoi(*cur);
	for(prev = cur, cur = next, next = cur + 1;
		 *cur;
		 prev = cur, cur = next, next = cur + 1)
	{
		if(rcmp(*cur, *next) == -1)
		{
			int temp = rtoi(*next);
			temp -= rtoi(*cur);
			res += temp;
			prev = cur; cur = next; next = cur + 1;
		}
		else
			res += rtoi(*cur);
	}
	
	return res;
}

int rtoi(char c)
{
  printf("Funzione rtoi()\n");
	switch(c)
	{
		case 'I':
			return 1;
		case 'V':
			return 5;
		case 'X':
			return 10;
		case 'L':
			printf("\ndentro L\n");
			return 50;
		case 'C':
			printf("\ndentro C\n");
			return 100;
		case 'D':
			return 500;
		case 'M':
			return 1000;
		default:
			return 0;
	}
}

int rcmp(char c1, char c2)
{
  printf("Funzione rcmp()\n");
	if(rtoi(c1) == rtoi(c2)) return 0;
	else if(rtoi(c1) > rtoi(c2)) return 1;
	else return -1;
}
