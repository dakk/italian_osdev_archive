#ifndef _MESSAGE_H_
#define _MESSAGE_H_

typedef unsigned int msg_t;
typedef unsigned int casella_postale;

enum{
        ALL = 0        //spedire un messaggio a tutti
};

//apre una casella postale alla posta e ne restituisce il codice
casella_postale apri_account();

//chiude una casella postale
//se viene ritornato
//        0       tutto è andato bene
//	-1      qualcosa è andato male
int chiudi_account(casella_postale cod);

//spedisce un messaggio al processo 
//ATTENZIONE BISOGNA ESSERE UTENTI REGISTRATI
//cod è il valore ricevuto durante la registrazione
//se si vuole spedire un messaggio a tutti i task si deve essere un KERNEL_TASK
//per specificare + opzioni si può fare la or tra esse
//la funzione restituisce 
//      -2 se la casella del mittente non esiste
//      -1 se la casella del destinatario non esiste
//      0 se tutto è andato bene
int send(casella_postale mittente, casella_postale destinatario, void *messaggio, size_t lunghezza);

//riceve un messaggio, se non ci sono messaggi oppure non si è registrati alla posta il puntatore vale NULL
//restriusice l'indirizzo del mittente
casella_postale receive(casella_postale mio_codice, void *messaggio);

//restituisce il numero di messaggi presenti nella casella
//se non ce ne nessuno viene restituito 0
//se la casella non esiste viene restituito -1
int stato_casella(casella_postale cod);

//inizializza la gestione dei messaggi
void init_msg();

#endif
