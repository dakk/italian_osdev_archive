#include <hmu.h>
#include <cstdlib>
#include <video.h>
#include <cstring>

#define NUM_PORTE 65536
#define NUM_IRQ 16

namespace {
	bool porte[NUM_PORTE];
	bool irq[NUM_IRQ];
	bool set_port(word porta) {
		if (porte[porta])
			return false;
		porte[porta] = true;
		return true;
	}

	bool unset_port(word porta) {
		if (!porte[porta])
			return false;
		porte[porta] = false;
		return true;
	}

	bool set_irq(word index) {
		if (index >= NUM_IRQ)
			return false;
		if (irq[index])
			return false;
		irq[index] = true;
		return true;
	}

	bool unset_irq(word index) {
		if (index >= NUM_IRQ)
			return false;
		if (!irq[index])
			return false;
		irq[index] = false;
		return true;
	}

	static const size_t max_device = 100;
	dispositivo *vett_dev[max_device];
}

void init_hmu()
{
	kout << "Inizializzo il gestore dell'hardware\n";
	for (int i = 0; i < NUM_IRQ; i++)
		irq[i] = false;
	for (int i = 0; i < NUM_PORTE; i++)
		porte[i] = false;
	return;
}

//si occupa di aggiungere il driver nella tabella dei driver, setta come occupate le porte e gli irq. 
//è compito dell'utente chiamare la init
bool add_driver(dispositivo * device, size_t index)
{
//      kout << "Aggiungo il driver: " << device->leggi_nome() << endl;
	size_t used_irq = device->leggi_no_irq();
	byte *list_irq = (byte *) malloc(sizeof(byte) * used_irq);
	device->leggi_irq(list_irq);
	/*TODO: da aggiungere il controllo se una porta è già occupata */
	kout << device->leggi_nome() << ": uso gli irq: ";
	for (size_t i = 0; i < used_irq; i++) {
		set_irq(list_irq[i]);
		kout << list_irq[i] << " ";
	}
	kout << endl;

	char numero[5];

	size_t used_port = device->leggi_no_porte();
	word *list_port = (word *) malloc(sizeof(word) * used_port);
	device->leggi_porte(list_port);
	/*TODO: da aggiungere il controllo se una porta è già occupata */
	kout << device->leggi_nome() << ": uso le porte: ";
	for (size_t i = 0; i < used_port; i++) {
		set_irq(list_port[i]);
		kout << "0x" << utoa(list_port[i], numero, 16) << " ";
	}
	kout << endl;
	free(list_port);
	free(list_irq);

	vett_dev[index] = device;

	return true;
}

//fa quello che fa la add_driver ma al contrario  :-)
//DA IMPLEMENTARE
bool del_driver(dispositivo * device)
{
	return true;
}

dispositivo *usa_dispositivo(size_t num)
{
	return vett_dev[num];
}
