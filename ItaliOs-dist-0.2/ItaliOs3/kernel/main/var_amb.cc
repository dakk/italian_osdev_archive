#include <var_amb.h>
#include <cstdlib>
#include <cstring>
#include <new>
#include <video.h>
#include <sem.h>

namespace{
	class env_var{
		public:
			env_var(char*nome,  char* valore){
				_nome= (char*) malloc (strlen(nome) + 1);
				strcpy(_nome, nome);
				
				_valore=(char*) malloc(strlen(valore) + 1);
				strcpy(_valore, valore);
			}
			
			void set_name(char* nome){
				free(_nome);
				_nome= (char*) malloc (strlen(nome) + 1);
				strcpy(_nome, nome);
			}
			
			char* get_name(char* nome){
				strcpy(nome, _nome);
				return nome;				
			}
			
			void set_value(char* valore){
				free(_valore);
				_valore= (char*) malloc (strlen(valore) + 1);
				strcpy(_valore, valore);				
			}
			
			char* get_value(char* valore){
				strcpy(valore, _valore);
				return valore;
			}
			
		private:
			char* _nome;
			char* _valore;
	};
	
	struct nodo{
		env_var *amb_var;
		nodo* next;
	};
	
	nodo* testa;
	void init_list(){
		testa = NULL;
	}
	
	void add_elem(char* nome,   char* valore){
		if(!testa){
			testa = new nodo;
			testa->amb_var = new env_var(nome, valore);
			testa->next = NULL;
		}else{
			nodo * temp = testa;
			while(temp->next) temp = temp->next;
			temp->next = new nodo;
			temp->next->amb_var =  new env_var(nome, valore);
			temp->next->next = NULL;
		}
	}
	
	bool cerca_var(char* nome){
		nodo* temp = testa;
		char name[1000];
		if(!temp) return false;
		while(temp->next){
			if(!strcmp(testa->amb_var->get_name(name), nome))
				return true;
			temp = temp->next;
		}
		return false;
	}
	
	char* prendi_valore(char*nome,  char* valore){
		nodo* temp = testa;
		char name[1000];
		if(!temp) return NULL;
		while(temp){
			if(!strcmp(temp->amb_var->get_name(name), nome)){
				return temp->amb_var->get_value(valore);
			}
			temp = temp->next;
		}
		return NULL;
	}
	
	sem amb_sem;
}

void init_variabili_ambiente(){
	init_list();
	kout << "Gestione delle variabili di ambiente inizializzata\n";
}



bool leggi_valore(char* nome, char*valore){
	amb_sem.prendi();
	if(!prendi_valore(nome, valore)){
		amb_sem.lascia();
		return false;
	}
	amb_sem.lascia();
	return true;
}

void setta_valore(char* nome, char* valore){
	char value[1000];
	amb_sem.prendi();
	if(!leggi_valore(nome,  value)){
		add_elem(nome,  valore);
	}
	amb_sem.lascia();
	return;
}
