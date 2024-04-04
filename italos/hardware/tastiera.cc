#include <tastiera.h>
#include <irq.h>
#include <io.h>
#include <cstddef>
#include <video.h>
#include <cstring>
#include <hmu.h>
#include <speaker.h>
#include <buffer_circolare>
#include <task.h>
#include <key_traslation.h>
#include <debug.h>
#include <sem.h>

//commentare o decommetare la righa successiva se si vuole abilitare o disabilitare il debug della tastiera
//#define DEBUG_TASTIERA

namespace {
	static char *driver_name = "tastiera.cc";
	const dword max_buffer_size = 1000;

	buffer_circolare < byte > buffer_raw;
	buffer_circolare < dword > buffer;

	pid_t pid_key;

	void key_handler() {
		byte carattere = inportb(0x60);
		if (!buffer_raw.scrivi(carattere)) {
			beep();
			return;
		} else {
			sveglia(pid_key);
#ifdef DEBUG_TASTIERA
			debug << "Sveglio il task della tastiera" << endl;
#endif
		}
#ifdef DEBUG_TASTIERA
		debug << "Sono l'handler e metto: " << carattere << endl;
#endif
	}

	byte leggi_carattere_raw() {
		byte carattere;
		while (!buffer_raw.leggi(&carattere)) {
#ifdef DEBUG_TASTIERA
			debug << "addormento il task della tastiera\n";
#endif
			addormenta(pid_key);
		}
		return carattere;
	}

	byte vedi_carattere_raw() {
		byte carattere;
		while (!buffer_raw.vedi(&carattere)) {
#ifdef DEBUG_TASTIERA
			debug << "addormento il task della tastiera\n";
#endif
			addormenta(pid_key);
		}
		return carattere;
	}

	dword leggi_carattere_normale() {
		dword carattere;
		while (!buffer.leggi(&carattere)) {
			//debug << "addormento il task della tastiera\n";
			//addormenta(get_current_task());
		}
		return carattere;
	}

	void key_task() {
		pid_key = get_current_task();
		buffer.init(1000);
		dword car;
		byte carattere = 0;
		while (1) {
			carattere = leggi_carattere_raw();
			if (carattere == 250)
				continue;
			if (carattere == 0xE0) {	//gestisco i caratteri con 2 scancode
				car = (carattere << 8) +
					leggi_carattere_raw();
				//è possibile che ci siano caratteri con 4 scancode che hanno questo formato: 0xE0 num 0xE0 num
				if (!buffer.scrivi(car)) {
					beep();
				}
				continue;
			}
			buffer.scrivi(carattere);
//      buffer[buffer_pos++]    = carattere;            
		}
	}

	class tastiera:public dispositivo {
	      public:
		virtual bool init(comando * cmd = NULL, size_t num_comandi =
				  0) {
			buffer_raw.init(100);
			/*pid_key = */ add_task(key_task, "DRIVER TASTIERA");
			add_irq(key_handler, 1);
			enable_irq(1);
			return true;
		}
		virtual bool close() {
			kout << "Sto chiudendo la tastiera" << endl;
			return true;
		}

		virtual bool leggi(dato & res) {
			dword *data = (dword *) res.dati;
			data[0] = leggi_carattere_normale();
			return true;
		}

		virtual bool scrivi(dato res) {
			return true;
		}

		virtual bool leggi_errore(dato * err) {
			return true;
		}

		virtual bool controlla(comando cmd) {
			return true;
		}

		virtual char *leggi_nome() {
			return driver_name;
		}

		virtual size_t leggi_no_irq() {
			return 1;
		}

		virtual void leggi_irq(byte * irq) {
			irq[0] = 1;
		}

		virtual size_t leggi_no_porte() {
			return 4;
		}

		virtual void leggi_porte(word * porte) {
			for (int i = 0x60; i <= 0x64; i++)
				porte[i - 0x60] = i;
		}
	};

	tastiera mykeyboard;
}

void setleds(byte led_status)
{
	while (inportb(0x64) & 2);
	outportb(0x60, 0xED);
	while (inportb(0x64) & 2);
	outportb(0x60, led_status);
}

void init_tastiera()
{
	add_driver(&mykeyboard, 1);
	mykeyboard.init();
}

dword get_char()
{
	dato my_dato;
	dword num;
	my_dato.dati = &num;
	usa_dispositivo(1)->leggi(my_dato);
	return num;
}

//DOBBIAMO ESSERE ------->PER FORZA<------ SENZA INTERRUPT
byte kbpress()
{
	asm("cli");
	dword caratteri[1000];
	size_t i = 0;
	bool result;
	//vedo se c'è un carattere valido nel buffer
	do {
		result = buffer.leggi(caratteri + i);
		i++;
#ifdef DEBUG_TASTIERA
		debug << "\nanalizzo " << caratteri[i - 1] << endl;
#endif
	} while (result
		 && ((caratteri[i - 1] > 0x80) || (caratteri[i - 1] == 0)));
	/*for(j=0; j < i; j++)
	 * buffer.scrivi(caratteri[i-j-1]); */
	if (result) {		//C'era!!!
#ifdef DEBUG_TASTIERA
		debug << "Sono la kbpress\n";
#endif
		return true;
	}
	return false;
	//return buffer_raw.vedi(&carattere);
}
