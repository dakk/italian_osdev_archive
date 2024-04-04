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
 * file : kernel/chardevs.c
 * Character device functions
 ***************************************************************************

 ***************************************************************************
 * $Id: chardevs.c,v 1.2 2000/08/25 13:39:38 iurlano Exp $
 ***************************************************************************
 *
 * $Log: chardevs.c,v $
 * Revision 1.2  2000/08/25 13:39:38  iurlano
 * Added CHANGELOG
 * more translations
 *
 * Revision 1.1.1.1  2000/05/30 08:11:08  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#include <chardevs.h>
#include <memory.h>
#include <panic.h>
#include <errors.h>
#include <types.h>
#include <semaphore.h>
#include <process.h>
#include <printk.h>

ssize_t read(int fd, void *buf, ssize_t count){
  int i;
  char * buffer=(char *)buf;
  fd--;

  //printk("READ %x\n",buf);

  Bool_P(&(chardevs[fd].semaforo));
  for(i=0;i<count;i++){
    if (chardevs[fd].head==chardevs[fd].tail) break;
    buffer[i]=chardevs[fd].buffer[chardevs[fd].head];

    //printk("%c",chardevs[fd].buffer[chardevs[fd].head]);
    //printk("%c",buffer[i]);

    ++(chardevs[fd].head);
    chardevs[fd].head=chardevs[fd].head%chardevs[fd].size;
  }
  Bool_V(&(chardevs[fd].semaforo));
  return i;
}



ssize_t write(int fd, const void *buf, ssize_t count){
  int i;
  char * buffer=(char *)buf;  
  fd--;

  Bool_P(&(chardevs[fd].semaforo));

  //printk("WRITE\n");

  for(i=0;i<count;i++){
    if (chardevs[fd].head==(chardevs[fd].tail+1)%chardevs[fd].size) break;
    chardevs[fd].buffer[chardevs[fd].tail]=buffer[i];
    ++(chardevs[fd].tail);
    chardevs[fd].tail=chardevs[fd].tail%chardevs[fd].size;

    //printk("%c",buffer[i]);

  }
  Bool_V(&(chardevs[fd].semaforo));
  return i;
}


/****************************************/
/* Funzioni di gestione dei dispositivi */
/****************************************/


/* inizializzazione dei dispositivi a caratteri */
void initchardevs(void){
  int i;

  Init_Bool_Semaphore(&(CH_Mutex),1);
  chardevs=(Chardev *)getpage();
  for(i=0;i<MAXCHARDEVS;i++) chardevs[i].present=0;
}




/* ritorna un buffer di dimensione dim*/
int getcharbuffer(unsigned int dim){
  int i;
  
  if(dim>CHARBUFMAXSIZE)return E_BUFTOOLONG;
  Bool_P(&(CH_Mutex));
  for(i=0;chardevs[i].present!=0;i++);
  if(i==MAXCHARDEVS){
    Bool_V(&(CH_Mutex));
    return E_NOMORECHDEV;
  }
  chardevs[i].ownerpid=Processo_In_Esecuzione->pid;
  chardevs[i].present=1;
  Bool_V(&(CH_Mutex));
  /*mallosso(dim)*/ 
  chardevs[i].buffer=(char *)getpage();
  chardevs[i].head=0;
  chardevs[i].tail=0;
  chardevs[i].size=dim;
  Init_Bool_Semaphore(&(chardevs[i].semaforo),1);
  return i+1;
}



/* rilascia un buffer precedentemente acquisito */
int releasecharbuffer(unsigned int fd){

  fd--;
  if(chardevs[fd].present==0)return E_WRONGCHDEV;
  if(chardevs[fd].ownerpid!=Processo_In_Esecuzione->pid)return E_NOT_OWNER;
  /* MANCA IL CONTROLLO DEI PERMESSI!!!! */
  /* Da stabilire se bisogna aspettare che il buffer sia vuoto */
  Bool_P(&(chardevs[fd].semaforo));
  freepage((page *)chardevs[fd].buffer);
  chardevs[fd].present=0;
  return E_SUCCESS;
}

















