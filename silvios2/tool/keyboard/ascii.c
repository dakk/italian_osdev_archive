#include <stdio.h>

int main(){
	int ch; 
	ch = 1;
	while(ch < 256){
		printf("\n%c = %d", ch, ch);
		ch++;
		printf("\t %c = %d", ch, ch);
		ch++;
		printf("\t %c = %d", ch, ch);
		ch++;
		printf("\t %c = %d", ch, ch);
		ch++;
		printf("\t %c = %d", ch, ch);
		ch++;
	}
	printf("\n");
		
	return 0;
}
