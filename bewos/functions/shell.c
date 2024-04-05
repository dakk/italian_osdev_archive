#include<io.h>
#include<keyboard.h>
#include<string.h>
/********************************************************************************
********************************************************************************/
void shell(void)
{

block_clear_status = 1;

if(stringcmp(buffer,"help",strlen("help")) == 0)
{
printk("\n\t|Comando|\t\t"); printk("\t\t |Descrizione|\n");
printk("   -------------------------------------------------------------------------\n");
printk("\n\tbewos -v\t\t\tVisualizza la versione di B&Wos");
printk("\n\tideatori\t\t\tVisualizza il nome degli ideatori");
printk("\n\tclear\t\t\t   Pulisce lo schermo");
printk("\n\ttnx\t\t\t     Ringraziamenti");
printk("\n\treboot\t\t\t  Riavvia il computer");
printk("\n\n   -------------------------------------------------------------------------");
printk("\n\n");
}


else if(stringcmp(buffer,"bewos -v",strlen("bewos -v")) == 0)
printk("\n\t\t   Black & White Operating System v. 0.1b\n\n\n");



else if(stringcmp(buffer,"ideatori",strlen("ideatori")) == 0)
printk("\n\t\t\tBlack & White - www.bewos.tk ^_^\n\n\n");



else if(stringcmp(buffer,"clear",strlen("clear")) == 0)
klear();


else if(stringcmp(buffer,"tnx",strlen("tnx")) == 0)
printk("\nVogliamo ringraziare tutti coloro che ci hanno aiutato e che ci aiuteranno nell'impresa. Thanks !!\n\n\n");




else if(stringcmp(buffer,"reboot",strlen("reboot")) == 0)
{
  asm("sti"); //disabilito gli interrupt

/* comando che riavvia il computer */
	outportb(0xFE,0x64);
/* e se non si riavvia per qualche problema... entriamo in un ciclo infinito */
	while(1);
}



else if(pos != 0 && videomem != block_video_mem)  /*Poichè voglio che "comando non trovato" appaia solo se si digita un comando
						  non valido e non quando si preme soltanto invio senza aver digitato nessun
						  carattere.... implemento questo if*/
printk("\nComando non trovato. Digitare 'help' per l' elenco dei comandi disponibili !!\n\n\n");




printk("BewoSH:> ");

}
/********************************************************************************
********************************************************************************/
