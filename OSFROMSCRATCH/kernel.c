char *videomem = (char *) 0xb8000;
void putc(char carattere){
                videomem++ = carattere; /*imposto il carattere*/
                videomem++ = 0x7;       /*imposto gli attributi del carattere*/
        } 
	 
void _start(){
                main();
                while(1);
        }

int main(){
                puts("Ciao sono il tuo sistema operativo");
                return 0;
        }

