/*
 * OSSO - A Micro Kernel OS
 * Copyright (c) 2000 Alessandro Iurlano.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */


/****************************** O S S O  ***********************************
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/events.c,v $
 * Descrizione: Routines for events managing in OSSO
 ***************************************************************************

 ***************************************************************************
 * $Id: events.c,v 1.13 2001/08/09 10:37:53 iurlano Exp $
 ***************************************************************************
 *
 * $Log: events.c,v $
 * Revision 1.13  2001/08/09 10:37:53  iurlano
 * Change Creat_thread to accept an integer parameter to be passed to the thread.
 * corrected exit_thread behaviour.
 * Added format routines to the ide (WARNING VERY DANGEROUS and NOT working)
 *
 * Revision 1.12  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.11  2001/04/09 15:10:12  marcopex
 * Eventest3.c finnally works
 *
 * Revision 1.10  2001/04/09 12:01:45  marcopex
 * Eventest3 finally works!! (it seems...)
 *
 * Revision 1.9  2001/04/06 10:27:02  marcopex
 * Continue event testing...
 *
 * Revision 1.8  2001/04/03 16:50:04  marcopex
 * Some little event testing & debugging
 *
 * Revision 1.7  2001/03/12 15:09:54  marcopex
 * Finished (maybe) the compilation of Events
 *
 *
 ***************************************************************************/

#include <process.h>
#include <errors.h>
#include <events.h>
#include <printk.h>
#include <panic.h>
#include <string.h>

/*******************************************/
/* PRIVATE DEFINITIONS AND DATA STRUCTURES */
/*******************************************/


/* mi serve:
 - decidere se fottere il campo owner e usare il next x i public

 - inizializzare a NULL il campo Events_List nel descr di processo: Fatto?

 - stioccare Destroy etc da qualche parte... (anche qui volendo...)
   PERCHE NON TRA GLI EVENTI PRIVATI???????????

 - Il campo Processo_In_Esecuzione dovrebbe rimanere valido (dopotutto
   anche se la schedulazione e' fatta a livello di thread, il processo esiste,
   ma per le signal e le wait e' meglio vedere le implicazioni sui thread...)

 - Concorrenza bastarda: un processo fa una P su un evento: la puo' fare, ma
   quando rilascia la ME per eseguire la P sul semaforo associato un altro cambia
   il permesso: il primo thread se ne fotte (non ne ha coscienza) ma in realta'
   non e' semanticamente valido:
   Soluzione: da implementare una P&V atomica.
   Lo stesso problema si presenta con la Signal: la get-pidt-from-pid restituisce 
   il pid_t, ma nessuno garantisce che quando si usa il processo (di cui abbiamo il
   pidt) esista sempre...

*/


/* Array of public events */
Event_struct Public_Events[NUM_PUBLIC_EVENTS];
Bool_Semaphore Events_Sem; /* Semaphore for concurrent use of public events */

/* List of private events. The head of the list is contained in the process 
   descriptor, but the elements are part of a global structure, to minimize 
   waste of space.
*/

Event_struct events[MAX_EVENT_NUMBER]; /* Global list */
Event_struct * events_freelist;        /* List of free elements */
Bool_Semaphore Priv_Events_Sem;        /* For mutex access */
    
/*********************/
/* PRIVATE FUNCTIONS */
/*********************/

/* Initialization of EVENTS data structures */
/* There is no semaphore use inside, but it should be not necessary */
int init_Events(void){
  int i;

  /* Private event structure */
  events_freelist=&events[0];
  for(i=1;i<MAX_EVENT_NUMBER;i++)
    events[i-1].next=&events[i];
  events[MAX_EVENT_NUMBER-1].next=NULL; 

  /* Public event structure */
  for(i=0; i<NUM_PUBLIC_EVENTS; i++){
    Public_Events[i].handler=NULL;
    Public_Events[i].owner=0;
    Public_Events[i].timeout=0;
    Public_Events[i].perms=0;
    sprintf(Public_Events[i].event,"INTERRUPT%d",i);
    Init_Semaphore(&(Public_Events[i].semaphore),0);
  }
  /* Interrupt 0 is handled by the kernel */
  Public_Events[0].perms=(E_MASK_FIX | E_MASK_TAKEN);

  /* EVENTUALI WELL KNOWN SERVICES */
  printk("Inizializzazione EVENTS. (RIGUARDARE!!!)\n");

  /* Semafori di mutua esclusione */
  Init_Bool_Semaphore(&(Events_Sem),1);
  Init_Bool_Semaphore(&(Priv_Events_Sem),1);

  return E_SUCCESS;
}

/* Stopping of EVENTS (Has it a sense?) */
int kill_events(void){
  /* Simply nothing to do */
  return E_SUCCESS;
}

/* Add a new event to the head of the process list */
/* Mutexed only for global data structures */
int add_event_to_list(void){
  Event_struct * elemento;

  Bool_P(&Priv_Events_Sem);
  /* Check for free space */
  if(events_freelist==NULL){
    Bool_V(&Priv_Events_Sem);
    printk("WARNING! There is no more space for events in kernel!\n");
    return E_EVENT_NOMORE;
  }
  /* Add an element to the list */
  elemento=events_freelist;
  events_freelist=events_freelist->next;
  Bool_V(&Priv_Events_Sem);
  elemento->next=Processo_In_Esecuzione->Events_List;
  Processo_In_Esecuzione->Events_List=elemento;
  return E_SUCCESS;
}

/* This remove the requested event from the list */
/* DA RIGUARDARE ABBONDANTEMENTE LA PARTE DELL'ELIMINAZIONE!!! */ 
int free_event_in_list(EVENT evento){
  Event_struct * cursore, ** base;
  int found=0;

  //_color_printk(LightGreen,"FREE_EVENT: Cerco evento %s\n",evento);
  printk("ELIMINAZIONE EVENTO IN LISTA! DA RIGUARDARE!\n");
  Bool_P(&Priv_Events_Sem);
  cursore=Processo_In_Esecuzione->Events_List;
  base=&(Processo_In_Esecuzione->Events_List);
  while(!found && cursore!=NULL){
    //_color_printk(LightGreen,"- confronto %s con %s\n",evento,cursore->event);
    if(!strncmp((char *)evento,cursore->event,MAX_EVENT_NAME_LENGTH)){
      _color_printk(LightGreen,"FREE_EVENT: Trovato!\n");
      /* Found the evil event! */
      found++;
      *(base)=cursore->next;
      /* Add the event to the free list */
      //Bool_P(&Priv_Events_Sem);
      cursore->next=events_freelist;
      events_freelist=cursore;
      //Bool_V(&Priv_Events_Sem);      
    }
    base=&(cursore);
    cursore=cursore->next;
  };
  Bool_V(&Priv_Events_Sem);
  if(found) {
    //_color_printk(LightGreen,"FREE_EVENT: Evento eliminato!\n");
    return E_SUCCESS;
  }
  if(cursore==NULL){
    //_color_printk(LightGreen,"FREE_EVENT: Evento NON Trovato!\n");
    return E_EVENT_NOTEXIST;
  }
  panic("SOMETHING FISHY IS GOING IN THE ARTIC!\n");
  return E_SUCCESS;
}

/* Check if the event requested exist in the process table and return it */
/* Maybe in a future it has an hash table or similar to make it faster.
   Warning: it is not mutexed (it has no sense to do, but the result is
   true only if critical races are avoided!!!) */
Event_struct * searchevent_priv(EVENT evento){
  Event_struct * cursore;

  //_color_printk(LightGreen,"SEARCHEVENT_PRIV: Cerco evento %s\n",evento);
  cursore=Processo_In_Esecuzione->Events_List;
  //_color_printk(LightGreen,"- confronto %s con %s\n",evento,cursore->event);
  while(cursore!=NULL && strncmp((char *)evento,cursore->event,MAX_EVENT_NAME_LENGTH)){
    //_color_printk(LightGreen,"- confronto %s con %s\n",evento,cursore->event);
    cursore=cursore->next;
  }
  return cursore;
}

/* Check if the event requested exist in the public table and return it */
/* Maybe in a future it has an hash table or similar to make it faster */
Event_struct * searchevent_pub(EVENT evento){
  int i;

  /* Little optimization :-) */
  /* Banale: se si chiamano tutti INTERRUPTqualcosa guardare intanto se
     evento comincia x INTERRUPTqualcosa (per ora tagliato ) */
  /* if(strncmp(evento,"INTERRUPT",9)) return NULL; */

  //_color_printk(LightGreen,"SEARCHEVENT_PUB: Cerco evento %s\n",evento);
  for(i=0;i<NUM_PUBLIC_EVENTS;i++)
    //_color_printk(LightGreen,"- confronto %s con %s\n",evento,Public_Events[i].event);
    if(!strncmp((char *)evento,Public_Events[i].event,MAX_EVENT_NAME_LENGTH))
      return &(Public_Events[i]);
  return NULL;
}

/* Check if the event requested exist in the fixed events and return it */
Event_struct * searchevent_fix(EVENT evento){

  //_color_printk(LightGreen,"SEARCHEVENT_FIX: Cerco evento %s\n",evento);
  /*per ora un ce ne sono, quindi ... */
  return NULL;

  /* Well known events */
  if(!strncmp((char *)evento,"DESTROY",MAX_EVENT_NAME_LENGTH)) return NULL;
  if(!strncmp((char *)evento,"SUSPEND",MAX_EVENT_NAME_LENGTH)) return NULL;
  if(!strncmp((char *)evento,"RESTART",MAX_EVENT_NAME_LENGTH)) return NULL;
  if(!strncmp((char *)evento,"KILL",MAX_EVENT_NAME_LENGTH)) return NULL;
  /* It seems it is not here ... */
  return NULL;
}

/* Check if the event requested exist in some place */
/* Per ora non serve a niente ... */
 Event_struct * searchevent(EVENT evento){
   Event_struct * event;
   
   if((event=searchevent_fix(evento))!=NULL) return event;
   if((event=searchevent_fix(evento))!=NULL) return event;
   if((event=searchevent_fix(evento))!=NULL) return event;
   return NULL;
}


/******************************/
/* INTERFACE IMPLEMENTATIONS  */
/******************************/


/* Request the creation of an event by name EVENT */
int getevent(char * nome){
  Event_struct * evento;
  eventname requested;
  int i;

  /* Check for a valid name */
  for(i=0;i<MAX_EVENT_NAME_LENGTH && nome[i]!='\0';i++)
    requested[i]=nome[i];
  requested[i]='\0'; /* Make things secure */
  //_color_printk(LightGreen,"Pid %d require an event %s\n",Processo_In_Esecuzione->pid,&requested);
  /* Check if the event already exist, and possibly get it ... */
  /* ... in fixed events */
  if(searchevent_fix(&requested)!=NULL)return E_EVENT_EXIST;

  /* ... in public events */

  //Bool_P(&(Events_Sem));
  if((evento=searchevent_pub(&requested))!=NULL){
    //_color_printk(LightGreen,"DEBUG: Ricerca eventi pubblici completata\n");
    if(evento->perms & E_MASK_TAKEN){
      /* the event is public and already taken */
      Bool_V(&(Events_Sem));
      return E_EVENT_PUB_TAKEN;
    } else {
      /* the event is public but free: let's grab it! */
      evento->owner=Processo_In_Esecuzione->pid;
      evento->perms=(E_MASK_TAKEN | E_MASK_BOOL);
      evento->handler=NULL;
      evento->timeout=0;
      Init_Semaphore(&(evento->semaphore),0);
      Bool_V(&(Events_Sem));
      return E_SUCCESS;
    }
  } else {
    /* If we are here we not have released the mutex ... */
    Bool_V(&(Events_Sem));
    /* Chech if this damn event is private ...*/
    //_color_printk(LightGreen,"lock su %x: \n",&(Processo_In_Esecuzione->Events_Sem));
    Bool_P(&(Processo_In_Esecuzione->Events_Sem));
    if(searchevent_priv(&requested)==NULL){
      //_color_printk(LightGreen,"Evento %s non trovato: lo creo\n",requested);
      /* There is no event by that name, so we can try to create one */
      if(add_event_to_list()==E_EVENT_NOMORE){
	/* Sorry, no more space... */
	Bool_V(&(Processo_In_Esecuzione->Events_Sem));
	return E_EVENT_NOMORE;
      } else {
	/* We now have a new event in the head of the list */
	evento=Processo_In_Esecuzione->Events_List;
	strcpy(evento->event,requested);
	evento->perms=(E_MASK_TAKEN | E_MASK_BOOL);
	evento->handler=NULL;
	evento->timeout=0;
	Init_Semaphore(&(evento->semaphore),0);
	Bool_V(&(Processo_In_Esecuzione->Events_Sem));
	//_color_printk(LightGreen,"- Creato l'evento %s\n",evento->event);
	return E_SUCCESS;
      }
    } else {
      /* This beautiful event already exist */
      Bool_V(&(Processo_In_Esecuzione->Events_Sem));
      //_color_printk(LightGreen,"L'evento ESISTE gia!!!!\n");
      return E_EVENT_EXIST;
    }
  }
}

/* Clear the requested event */
int clearevent(EVENT event){
  Event_struct * evento;

  //_color_printk(LightGreen,"CLEAREVENT: richiesta eliminazione evento %s\n",event);
  /* Looking for event ... */

  /* If the event is fixed it can't be cleared */
  if(searchevent_fix(event)!=NULL)return E_EVENT_NOPERMS;
  /* Is it in the public events? */
  Bool_P(&(Events_Sem));
  if((evento=searchevent_pub(event))!=NULL){
    /* yes, so if it is ours we can release it */
    if((evento->perms & E_MASK_TAKEN) && (evento->owner==Processo_In_Esecuzione->pid) ){
      evento->perms=0;
      evento->owner=0;
      Bool_V(&(Events_Sem));
      return E_SUCCESS;
    } else {
      /* The event is free or not ours, so how can we release it? */
      Bool_V(&(Events_Sem));
      return E_EVENT_NOPERMS;
    }
  }
  /* Looking in the process list ...*/
  return free_event_in_list(event);
}

/* Set the attributes of th e requested event */
int setevent(EVENT event,ATTRIBUTES attributi){
  Event_struct * evento;
  Thread_idx thread_glidx;

  //_color_printk(LightGreen,"DEBUG: Chiamata setevent per %s!\n",event);
  /* Is it in the public events? */
  Bool_P(&(Events_Sem));
  if((evento=searchevent_pub(event))!=NULL){
    /* yes, so try to modify it */
    if((evento->perms & E_MASK_TAKEN) && (evento->perms & E_MASK_FIX) 
       && (evento->owner==Processo_In_Esecuzione->pid)){
      /* Let's do some check */
      if(attributi->timeout!=0){
	/* Let's pick a timer */
	Bool_V(&(Events_Sem)); 
	return E_NOT_IMPLEMENTED_YET;
      }
      if(attributi->handler!=NULL){
	/* Create a thread */
	Bool_V(&(Events_Sem));
	return E_NOT_IMPLEMENTED_YET;
	thread_glidx=Create_Thread(Processo_In_Esecuzione,(dword)attributi->handler,0);
	/* zzo ci fo coll'indice? */
      }
      /* Set the perms, but ignore the FIX attribute */
      evento->perms=(attributi->permessi & (!E_MASK_FIX));
      /* Check if the initialization of the semaphore is compatible
	 with the attribute bool */
      if((evento->perms | E_MASK_BOOL) && attributi->valore>1 ){
 	Bool_V(&(Events_Sem));
	return E_EVENT_WRONGVALS; 
      }
      Init_Semaphore(&(evento->semaphore),attributi->valore);
      Bool_V(&(Events_Sem));
      return E_SUCCESS;
    } else {
      /* The event is free, fix or not ours, so negate permission */
      Bool_V(&(Events_Sem));
      return E_EVENT_NOPERMS;
    }
  } 
  Bool_V(&(Events_Sem));

  /* Looking in the process list ...*/
  Bool_P(&(Processo_In_Esecuzione->Events_Sem));
  if((evento=searchevent_priv(event))!=NULL){
    /* Let's do some check */
    if(attributi->timeout!=0){
      Bool_V(&(Processo_In_Esecuzione->Events_Sem));
      return E_NOT_IMPLEMENTED_YET;
    }
    if(attributi->handler!=NULL){
      Bool_V(&(Processo_In_Esecuzione->Events_Sem));
      return E_NOT_IMPLEMENTED_YET;
    }
    /* Set the perms, but ignore the FIX attribute */
    evento->perms=(attributi->permessi & (!E_MASK_FIX));
    /* Check if the initialization of the semaphore is compatible
       with the attribute bool */
    if((evento->perms | E_MASK_BOOL) && attributi->valore>1 ){
      Bool_V(&(Processo_In_Esecuzione->Events_Sem));
      return E_EVENT_WRONGVALS; 
    }
    Init_Semaphore(&(evento->semaphore),attributi->valore);
    Bool_V(&(Events_Sem));
    return E_SUCCESS;
  }
  Bool_V(&(Processo_In_Esecuzione->Events_Sem));
  /* Manca la gestione degli eventi fissi, che per ora non ci sono */
  return E_EVENT_NOTEXIST;
}


/* SIGNAL the happening of the event */
int signal(PID pid, EVENT evento){
 Event_struct * cursore;
  pid_t pidt;

  //_color_printk(LightGreen,"SIGNAL: segnalazione di %s a %d!\n",evento,pid);
  if(pid !=0){
    /* Let's search this process */
    if((pidt=Get_Pid_t_From_Pid(pid))==NULL){
      return E_PID_NOT_FOUND;
    } else {
      /* We have found the process... */
      //_color_printk(LightGreen,"lock su %x: \n",&(pidt->Events_Sem));
      Bool_P(&(pidt->Events_Sem));
      cursore=pidt->Events_List;
      /**************************************************************/      
      //_color_printk(LightGreen,"SIGNAL: Cerco evento %s\n",evento);
      //_color_printk(LightGreen,"- confronto %s con %s:",evento,cursore->event,
      //	    strncmp((char *)evento,cursore->event,MAX_EVENT_NAME_LENGTH),cursore);
      while(cursore!=NULL && strncmp((char *)evento,cursore->event,MAX_EVENT_NAME_LENGTH)){
	//_color_printk(LightGreen," Fallito! \n");
	cursore=cursore->next;
	//_color_printk(LightGreen,"- confronto %s con %s:",evento,cursore->event);
      }
      
      if(cursore==NULL){
	Bool_V(&(pidt->Events_Sem));
	//_color_printk(LightGreen," Fallito!\n- Evento NON trovato!\n",cursore);
	return E_EVENT_NOTEXIST;
      }else {
	//_color_printk(LightGreen," OK!\n Trovato!\n");
	//_color_printk(LightGreen,"Semaforo %x: \n",&(cursore->semaphore));
	V(&(cursore->semaphore));
	Bool_V(&(pidt->Events_Sem));
	return E_SUCCESS;
      }
    }
  } else {
    /* No process specified, look in the public events */
    Bool_P(&(Events_Sem));
    if((cursore=searchevent_pub(evento))!=NULL){
      /* Does we have permissions? */
      if((cursore->perms & E_MASK_EXT) || 
	 (cursore->owner==Processo_In_Esecuzione->pid) ){
	V(&(cursore->semaphore));
	Bool_V(&(Events_Sem));
	return E_SUCCESS;
      } else {
	Bool_V(&(Events_Sem));
	return E_EVENT_NOPERMS;
      }
    } else {
      Bool_V(&(Events_Sem));
      return E_EVENT_NOTEXIST;
    }
  }
  panic("Raggiunto punto IRRAGGIUNGIBILE nella wait!\n");
}


/* Wait for an event happening */ 
int wait(EVENT event, PID * retpid){
  Event_struct * evento;

  //_color_printk(LightGreen,"WAIT: Attendo l'evento %s:\n",event);
  Bool_P(&(Events_Sem));
  if((evento=searchevent_pub(event))!=NULL){
    /* Is this owned by someone? */
    if(!(evento->perms & E_MASK_TAKEN)){
      Bool_V(&(Events_Sem));
      return E_EVENT_PUB_NOTTAKEN;
    }
    /* Check if we can do the wait */
    if(evento->owner==Processo_In_Esecuzione->pid){
      Bool_V(&(Events_Sem));
      P(&(evento->semaphore));
      return E_SUCCESS;
    } else {
      /* We don't own semaphore, so check if we can do a wait */
      if(!(evento->perms & E_MASK_EXT)){ 
	Bool_V(&(Events_Sem));
	return E_EVENT_NOPERMS;
      }
      Bool_V(&(Events_Sem));
      P(&(evento->semaphore));
      return E_SUCCESS;
    }
  } else Bool_V(&(Events_Sem));
  /* Look in the fixed event */
  if((evento=searchevent_fix(event))!=NULL){
    if(!(evento->perms & E_MASK_FIX)){
      /* Da finire!!!! */
    }
  }
  /* Look in the private event */
  //_color_printk(LightGreen,"lock su %x: \n",&(Processo_In_Esecuzione->Events_Sem));
  Bool_P(&(Processo_In_Esecuzione->Events_Sem));
  if((evento=searchevent_priv(event))!=NULL){
    Bool_V(&(Processo_In_Esecuzione->Events_Sem));
    //_color_printk(LightGreen,"Semaforo %x: \n",&(evento->semaphore));
    P(&(evento->semaphore));
    //_color_printk(LightGreen,"Esco dalla wait... \n");
    return E_SUCCESS;    
  } else return E_EVENT_NOTEXIST;
  panic("Raggiunto punto IRRAGGIUNGIBILE nella wait!\n");
}













