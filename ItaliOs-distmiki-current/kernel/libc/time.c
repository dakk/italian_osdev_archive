#include <time.h>
#include <rtc.h>

//al momento metto febbraio per i non bisestili
static const size_t giorni_per_mese[] =
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//restituisce 0 se anno non è bisestile, !0 se anno è bisestile
char bisestile(size_t anno)
{
	if (!(anno % 4)) {
		if (!(anno % 400))
			return 1;
		if (anno % 100)
			return 1;
	}
	return 0;
}

time_t time(time_t * timer)
{
	//devo calcolare il numero di secondi dal 1/1/1970

	//prendo tutti i dati di questo momento
	word anno = leggi_anno();
	word mese = leggi_numero_mese();
	word giorno = leggi_giorno_del_mese();
	word ora = leggi_ore();
	word minuti = leggi_minuti();
	word secondi = leggi_secondi();
	word tot_giorni;

	//calcolo quanti secondi ci stanno in un anno non bisestile
	dword secondi_per_anno = 60 * 60 * 24 * 365;

	//calcolo in numero di secondi dal 1/1/1970 al 1/1/anno
	time_t adesso = (anno - 1970) * secondi_per_anno;
	int i;

	for (i = tot_giorni = 0; i < (mese - 1); i++) {
		tot_giorni += giorni_per_mese[i];
	}

	//aggiungo i secondi dal 1/1/anno ad adesso
	adesso +=
		secondi + minuti * 60 + (ora - 1) * 60 * 60 + ((giorno - 1) +
							       tot_giorni) *
		24 * 60 * 60;

	//gestisco i casi particolari

	//aggiungo i giorni per gli anni bisestili
	for (i = 1970; i < anno; i++) {
		if (bisestile(i)) {
			adesso += (60 * 60 * 24);	//gli aggiungo un giorno
		}
	}

	//vedo se anche quest'anno è bisestile
	if ((mese > 2) && bisestile(anno))
		adesso += (60 * 60 * 24);

	if (timer) {
		*timer = adesso;
	}

	return adesso;

}
