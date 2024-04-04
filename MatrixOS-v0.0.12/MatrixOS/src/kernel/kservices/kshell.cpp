/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: klog.cpp			|
	| Date: 31/12/2004			|
	| Authors: Davide B.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/
#include <kservices/kshell.h>
#include <hardware/driver/video.h>
#include <kservices/kout.h>
#include <hardware/driver/keyboard.h>
#include <hardware/io.h>
#include <string.h>

#include <kdef.h>


/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace kservices;



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////


/*
  Avvio la shell
*/
void KShell::Start(){

	kservices::KOut::WriteLine("Digitare help per visualizzare i comandi disponibili.");
	
	char str[225];
	while(1){
		kservices::KOut::Write("MatrixKSH>");

		hardware::driver::keyboard::Keyboard::getStr(str,225);

		if(!strcmp(str, "help")){
			kservices::KOut::WriteLine("reboot");
			kservices::KOut::WriteLine("clear");
			kservices::KOut::WriteLine("authors");
			kservices::KOut::WriteLine("version");

		}else if(!strcmp(str, "reboot")){
			kservices::KOut::WriteLine("Rebooting...");
			//Disabilito gli interrupt
			asm("cli");
			//Mando un segnale di reboot alla tastiera
			outportb(0x64,0xFE);
			//Entro in un ciclo infinito
			while(1);

		}else if(!strcmp(str, "clear")){
			hardware::driver::video::Video::Clear();

		}else if(!strcmp(str, "authors")){
			kservices::KOut::WriteLine("Davide B. 'WindowsUninstall'\nEnrico S. 'Ada Byron'\nMichele 'jockerfox'\nGiuseppe M. 'Denever'\nRingrazio anche Silvio Abruzzo e Ivan Gualandri.");


		}else if(!strcmp(str, "version")){
			kservices::KOut::WriteLine("0.0.12 - " __DATE__ " " __TIME__);
		}else if(!strcmp(str,"")){
		
		}else{
			kservices::KOut::Write("MatrixKSH: ");
			kservices::KOut::Write(str);
			kservices::KOut::WriteLine(": command not found");
		}

		for (int stri = 0; stri <255;stri++) str[stri] = '\0';
	}
}

