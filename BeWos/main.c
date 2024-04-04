#include<io.h>
#include<interrupt.h>
#include<keyboard.h>
#include<shell.h>

int main();


/********************************************************************************
********************************************************************************/
/*La prima funzione che verra' chiamata e' propria questa che segue. Deve essere
anche la prima ad essere implementata nel file dove c'è la funziona main
altrimenti non funzionerà nulla */
void _start(){
	main();
	while(1);
}
/********************************************************************************
********************************************************************************/
int main(){
int k;
init_videomem(); //inizializzo la memoria video

colore=(int)0x1E;
klear();

printk("\n\n");

printk("\t  oooooooooo             oooo     oooo                       \n");
printk("\t   888    888  ooooooooo8 88   88  88  ooooooo    oooooooo8 \n");
printk("\t   888oooo88  888oooooo8   88 888 88 888     888 888ooooooo  \n");
printk("\t   888    888 888           888 888  888     888         888 \n");
printk("\t  o888ooo888   88oooo888     8   8     88ooo88   88oooooo88  \n");
printk("\n\n");
printk("\t\t\t\twww.bewos.tk\n\n\n");


for(k=0;k<SCR_C;k++) printk("-");

printk("Inizializzazione del PIC8259...");
Init_PIC8259();
printok();

printk("Inizializazione Dell' IDT - Interrupt Descriptor Table...");
initIDT();
printok();

printk("Inizializazione Della keyboard...");
initKBD();
printok();

for(k=0;k<SCR_C;k++) printk("-");

printk("\n\n\n");
shell();

	while(1);
	return 0;
}
/********************************************************************************
********************************************************************************/
void gestisci_irq(void)
{

/* Per adesso solo un irq puo' arrivare: quello della tastiera. Per questo motivo questa funzione chiama direttamente l' isr
   della keyboard senza controllare quali irq sono stati attivati visto che solo un irq(per adesso) può essere stato attivato,
   appunto quello della tastiera */
    isr_kbd();
}

