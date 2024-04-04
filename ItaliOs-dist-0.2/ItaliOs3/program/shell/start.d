extern int main(int argc, char** argv);

void spacca(char* str, int* num, char** parametri);

void _start(char* linea){
	int num;
	char* par[50];
	spacca(linea, &num, par);
	main(num, par);
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void spacca(char* str, int* num, char** parametri){
	int len_str = strlen(str);
	*num = 0;
	char* wstr = (char*) malloc(sizeof(char) * len_str + 1);
	strcpy(wstr, str);
	char* temp_str = (char*) malloc(sizeof(char) * len_str + 1);
	while(1){
		sscanf(wstr, "%s", temp_str);
		wstr =strstr(wstr, temp_str) ;
		if(!strlen(temp_str))
			break;
		*(parametri+(*num)) =(char*) malloc(sizeof(char) * strlen(temp_str) + 1);
		strcpy(parametri[*num], temp_str);
		(*num)++;
		wstr += strlen(temp_str) + 1;
	}
	return;
}
