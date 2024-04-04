
char *videomem = (char *) 0xb8000;	/*da dove inizia la memoria della scheda video*/

int main();

/*è la prima funzione che sarà chiamata!!!
IMPORTANTE: deve essere anche la prima implementata nel file dove c'è il main*/
void _start(){
	main();
	while(1);
}
	

void putc(char carattere){
	*videomem++ = carattere;	/*imposto il carattere*/
	*videomem++ = 0x7;			/*imposto gli attributi del carattere*/
}

void puts(char *stringha){
        int i;
	while(*stringha){
		putc(*stringha);	
		stringha++;
		putc('a');
	}
}
	
	
int main(){
	puts("Ciao sono il tuo sistema operativo");
	return 0;
}
