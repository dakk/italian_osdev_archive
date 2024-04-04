#include <task.h>
#include <messaggi.h>
#include <mem.h>
#include <cstring>
#include <stack>
#include <video.h>

namespace {
	enum account_state {
		APERTO,
		CHIUSO,
		NON_CREATO
	};
	const casella_postale max_caselle = 1000;
	struct messaggio {
		void *msg;
		size_t size;
		casella_postale mittente;
	};
	struct account {
		account_state stato;
		  stack < struct messaggio >casella;
	};
	struct account ufficio_postale[max_caselle];
	//ritorna -1 se non trova + caselle
	int cerca_primo_libero() {
		casella_postale cod;
		for (cod = 0; cod < max_caselle; cod++)
			if (ufficio_postale[cod].stato == NON_CREATO)
				return cod;
		return -1;
	}

	//verifica se è stato registrato l'account cod ed è aperto
	bool esistente(casella_postale cod) {
		if (cod >= max_caselle)
			return false;
		return (ufficio_postale[cod].stato == APERTO) ? true : false;
	}
};

casella_postale apri_account()
{
	int cod = cerca_primo_libero();
	ufficio_postale[cod].stato = APERTO;
	return cod;
};

int chiudi_account(casella_postale cod)
{
	if (esistente(cod)) {
		ufficio_postale[cod].stato = CHIUSO;
		return 0;
	}
	return -1;
};

int send(casella_postale mittente, casella_postale destinatario,
	 void *messaggio, size_t lunghezza)
{
	disattiva_scheduler();
	//kout << "1";
	if (esistente(mittente) && esistente(destinatario)) {
		struct messaggio msg;
		msg.msg = kmalloc(lunghezza);
		msg.size = lunghezza;
		msg.mittente = mittente;
		memcpy(msg.msg, messaggio, lunghezza);
		ufficio_postale[destinatario].casella.push(msg);
		attiva_scheduler();
		return 0;
	}
	attiva_scheduler();
	if (!esistente(destinatario))
		return -1;
	else
		return -2;
};

//restiuisce il mittente del messaggio
casella_postale receive(casella_postale mio_codice, void *messaggio)
{
	disattiva_scheduler();
	//kout << "2";
	if (esistente(mio_codice)) {
		if (ufficio_postale[mio_codice].casella.size()) {
			struct messaggio mymsg;
			mymsg = ufficio_postale[mio_codice].casella.pop();
			memcpy(messaggio, mymsg.msg, mymsg.size);
			kfree(mymsg.msg);
			attiva_scheduler();
			return mymsg.mittente;
		}
		/*      messaggio = 0;
		 * return; */
	}
	attiva_scheduler();
	messaggio = 0;
	return 0;

};


int stato_casella(casella_postale cod)
{
	if (esistente(cod))
		return ufficio_postale[cod].casella.size();
	return -1;
};


void init_msg()
{
	for (size_t i = 0; i < max_caselle; i++) {
		ufficio_postale[i].stato = NON_CREATO;
		ufficio_postale[i].casella.init();
	}
	kout << "Inizializzazione del gestore dei messaggi completata" <<
		endl;

}
