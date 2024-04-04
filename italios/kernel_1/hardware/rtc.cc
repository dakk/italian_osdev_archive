#include <rtc.h>
#include <io.h>
#include <video.h>
#include <cstddef>

namespace {
	inline byte read_rtc_address(byte address) {
		outportb(0x70, address);
		return inportb(0x71);
	}

	inline void write_rtc_address(byte address, byte val) {
		outportb(0x70, address);
		outportb(0x71, val);
	}

	//testa se il chip è pronto ad eseguire operazioni
	inline bool pronto() {
		return !(read_rtc_address(0xa) & 1);
	}

	inline byte bcd2dec(byte bcd) {
		return (((bcd >> 4) * 0xa) + (bcd & 0xf));
	}
}

const char *giorni_della_settimana[] = { "Domenica",
	"Lunedi'",
	"Martedi'",
	"Mercoledi'",
	"Giovedi'",
	"Venerdi'",
	"Sabato"
};

//Domenica è 0                                                                    
size_t leggi_numero_giorno_della_settimana()
{
	byte giorno;
	asm("cli");
	while (!pronto());
	giorno = read_rtc_address(6);
	asm("sti");
	return giorno - 1;
}

const char *leggi_nome_giorno_della_settimana()
{
	return giorni_della_settimana[leggi_numero_giorno_della_settimana()];
}

byte leggi_giorno_del_mese()
{
	byte giorno;
	asm("cli");
	while (!pronto());
	giorno = bcd2dec(read_rtc_address(7));
	asm("sti");
	return giorno;
}


byte leggi_numero_mese()
{
	byte mese;
	asm("cli");
	while (!pronto());
	mese = bcd2dec(read_rtc_address(8));
	asm("sti");
	return mese;
}

word leggi_anno()
{
	byte anno;
	asm("cli");
	while (!pronto());
	anno = bcd2dec(read_rtc_address(9));
	asm("sti");
	//suppongo che nessuno userà l'os dopo il 2100 ;-)
	return anno + 2000;
}

byte leggi_secondi()
{
	byte second;
	asm("cli");
	while (!pronto());
	second = bcd2dec(read_rtc_address(0));
	asm("sti");
	return second;
}

byte leggi_minuti()
{
	byte minuti;
	asm("cli");
	while (!pronto());
	minuti = bcd2dec(read_rtc_address(2));
	asm("sti");
	return minuti;
}

byte leggi_ore()
{
	byte ore;
	asm("cli");
	while (!pronto());
	ore = bcd2dec(read_rtc_address(4));
	asm("sti");
	return ore;
}




void init_rtc()
{
	asm("cli");
	//setto il divisore del clocl a 2
	//non imposto il rate per l'interrupt
	write_rtc_address(0xa, 0x20);

	//non abilito nessun interrupt
	//imposto che il giorno viene contato come 24 ore
	write_rtc_address(0xb, 2);

	//leggo lo status register per svuotare il buffer
	read_rtc_address(0xc);
	kout << "Real Time Clock Inizializzato" << endl;
	asm("sti");
	kout << " Ora sono le  " << leggi_ore() << ":" << leggi_minuti() <<
		":" << leggi_secondi() << "  di   " <<
		leggi_nome_giorno_della_settimana() << "    " <<
		leggi_giorno_del_mese() << "/" << leggi_numero_mese() << "/"
		<< leggi_anno() << endl;
}
