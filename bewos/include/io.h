#define SCR_C 80 //Definisco le colonne dello schermo
#define SCR_R 25 //Definisco le righe dello schermo
#define DIM_TAB 8 //definisco la dimensione del tab(tasto della tastiera) in spazi

int colore, caratteri_stampati;                //mi indica il numero dei caratteri stampati in una riga
unsigned long int caratteri_stampati_assoluti; //mi indica il numero di tutti i caratteri stampati a video
char *videomem;
void putk(char carattere);
void printk(char *stringa);
void newline(void);
void klear(void);
void printab(void); 
void scroll(void);
void printok(void);

void init_videomem(void);
