#ifndef _HMU_H_
#define  _HMU_H_

#include <cstddef>

typedef dword code_t;

/*questa struttura identifica il comando da passare al dispositivo. Il campo codice contiene un numero che identifica il significa del campo dato*/

struct comando{
	void* dato;
	code_t codice;
};

/*usato dalle funzioni di lettura e scrittura per ritornare dei dati*/
struct dato{
	void* dati;
	size_t len_dato;
	code_t codice;
};

/*serve come vettore di interrupt, il vettore index è grande number elementi*/
struct irq_hardware_t{
	word* index;
	size_t number;
};

/*questa classe identifica un interfaccia unificata per la gestione del driver*/
class dispositivo{
	public:		
			/*viene passato un vettore di comandi*/
		virtual bool init(comando* cmd, size_t num_comandi);
	
		/*chiude il dispositivo*/
		virtual bool close();
	
		/*legge un dato dal dispositivo. Il puntatore dato DEVE essere inizializzato dal chiamante*/
		virtual bool leggi(dato& res);
	
		/*scrive un dato sul dispositivo*/
		virtual bool scrivi(dato res) ;
	
		/*se qualche funzione non va a buon fine è possibile determinare tramite questa funzione la causa dell'errore*/
		virtual bool leggi_errore(dato* err) ;
		
		/*serve per spedire al dispositivo un comando diverso dalla lettura e scritture*/
		virtual bool controlla(comando cmd) ;
		
		/*restituisce il nome del driver*/
		virtual char* leggi_nome() ;
	
		//restituisce il numero di irq usati dal dispositivo
		virtual size_t leggi_no_irq() ; 
	
		//restituisce un vettore con l'elenco degli irq usati
		virtual void leggi_irq(byte* irq) ;
		
		//restituisce il numero di porte usate dal dispositivo
		virtual size_t leggi_no_porte() ;
	
		//restituisce un vettore con l'elenco delle porta usate
		virtual void leggi_porte(word* porte) ;
};

void init_hmu();

//si occupa di aggiungere il driver nella tabella dei driver, setta come occupate le porte e gli irq. 
//è compito dell'utente chiamare la init
bool add_driver(dispositivo* device, size_t index);

//fa quello che fa la add_driver ma al contrario  :-)
bool del_driver(dispositivo* device);

dispositivo* usa_dispositivo(size_t num);

#endif
