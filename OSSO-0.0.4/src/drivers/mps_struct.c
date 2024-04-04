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
 * file : $Source: /cvsroot/osso/OSSO/src/drivers/mps_struct.c,v $
 * Description: Modules Pid Server: drivers data management.
 ***************************************************************************

 ***************************************************************************
 * $Id: mps_struct.c,v 1.7 2001/08/04 18:22:47 steros Exp $
 ***************************************************************************
 *
 * $Log: mps_struct.c,v $
 * Revision 1.7  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.6  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.5  2001/07/08 11:12:50  steros
 * Converted MPS to dynamic allocation and other little fix.
 *
 * Revision 1.4  2001/06/09 18:30:23  steros
 * Fixes.
 *
 * Revision 1.3  2001/05/03 17:24:06  steros
 * Improved MPS boot phase.
 *
 * Revision 1.2  2001/05/02 18:50:27  steros
 * Driver dependancies automation implemented.
 *
 * Revision 1.1  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 *
 ***************************************************************************/


#include <printf.h>
#include <malloc.h>
#include <memory.h>
#include <processes.h>
#include <drivers/protocols/mpsint.h>

#include "mps_boot.h"
#include "mps_struct.h"


//#define DEBUG         /* Enable debug checks and messages.  */
//#define DEBUG_LIGHT   /* Prints structure state at changes. */

#define DBGLprintf(a...); { printf("[MPS:%s:%u] ",__FILE__,__LINE__); printf(a); }
#ifdef DEBUG
  #define DBGprintf(a...); { printf("[MPS:%s:%u] ",__FILE__,__LINE__); printf(a); }
#else
  #define DBGprintf(a...);
#endif


/* These defines must be > 0. */
#define CATEG_BLOCK_NUM     8  /* Num of new cat to add on resizing. */
#define DRIVER_BLOCK_NUM   16  /* Num of new drv to add on resizing. */
#define PID_BLOCK_NUM      32  /* Num of new pid to add on resizing. */
//#define WAITING_QUEUE_SIZE 32
#define MAX_WAITQUEUE      32



/* Prototypes for mps.c */
void Send_AskPid_Reply(PID program_pid, PID driver_pid);
void Driver_Init_Reply(void);


typedef struct _drv_instance_t {
  PID  pid;
  char in_use;   /* bool */
  int  driver_version;
  int  driver_release;
  char *description;
  PID  waiting;

  struct _drv_instance_t *next;
  struct _drv_instance_t *prev;
  void                   *driver;
} drv_instance_t;


typedef struct {
  char            *name;
  struct {
    int     is_multi:1;
    int     is_pool :1;
  } bits;
  int  num_waiting;
  PID  waiting[MAX_WAITQUEUE];

  int             numinstances;
  drv_instance_t  *instances;
} driver_t;


typedef struct {
  char *name;
  int total;
  int used;
  driver_t **drivers;
} category_t;


typedef struct {
  int total;
  int used;
  category_t **categories;
} categories_list_t;


typedef struct {
  PID            pid;
  drv_instance_t *instance;
} driver_pid_t;


categories_list_t *CategDrv=NULL;
driver_pid_t *PidList=NULL;
int TotPid=0, UsedPid=0;



/*
 TODO:
 - Binary dicotomic search to be used!!!
 - I should need pid indexing so.

 NOTE:
  - When I will implement the loading of a driver,
    it will have to be added suddenly in list with pid=0.
*/





/*------------------------------------------------------------------*/



void DBGViewData(void) {
  category_t **cat;
  driver_t **drv;
  drv_instance_t *inst;
  int ncat, ndrv;

  DBGLprintf("------------------------------------------------->\n");
  if (!CategDrv || !CategDrv->categories) {
    DBGLprintf("Nessuna categoria!\n");
  } else {
    for (cat=CategDrv->categories, ncat=0; ncat<CategDrv->used;
         ncat++, cat++)
    {
      DBGLprintf("Category <%s>\n",(*cat)->name);
      for (drv=(*cat)->drivers, ndrv=0; ndrv<(*cat)->used; ndrv++, drv++)
      {
        DBGLprintf("  Driver <%s>: multi=%d, pool=%d, num_waiting=%d\n",
                   (*drv)->name,(*drv)->bits.is_multi,(*drv)->bits.is_pool,
                   (*drv)->num_waiting);
        for (inst=(*drv)->instances; inst; inst=inst->next) {
          DBGLprintf("    Instance: pid=%d, in_use=%d, ver=%d, rel=%d\n",
                     inst->pid,inst->in_use,inst->driver_version,
                     inst->driver_release);
        }
      }
    }
  }
  DBGLprintf("<-------------------------------------------------\n");
}



/*------------------------------------------------------------------*/


/*
 This is like Linux realloc. The only difference is that oldsize is
 specified.
*/
void *MyRealloc(void *ptr, size_t oldsize, size_t newsize) {
  byte *area;

DBGprintf("MyRealloc> ptr=%u, old=%d, new=%d\n",ptr,oldsize,newsize);
  if (newsize)
    if ((area=(byte *)mallosso(newsize))==NULL) return NULL;
  if (ptr) {
    if (oldsize>=newsize)
      memcpy(area,ptr,newsize);
    else {
      memcpy(area,ptr,oldsize);
    }
    freeosso(ptr);
  }
DBGprintf("MyRealloc> area=%u\n",area);
  return area;
}


/*------------------------------------------------------------------*/


/*
 Find a category returning a pointer to it.
*/
category_t *FindCategory(char *category) {
  category_t **cat;
  int cont=CategDrv->used;

DBGprintf("FindCategory> cat=<%s>\n",category);
  if (!CategDrv) return NULL;
DBGprintf("FindCategory> 1\n");
  if ((cat=CategDrv->categories)==NULL) return NULL;
DBGprintf("FindCategory> 2 cat=%u\n",cat);
  for (; cont && strcmp(category,(*cat)->name); cont--, cat++) { //;
DBGprintf("FindCategory> 3a cont=%u, *cat=%u\n",cont,*cat);
DBGprintf("FindCategory> 3b cat_name=<%s>\n",(*cat)->name);
}
  if (!cont) return NULL;
DBGprintf("FindCategory> 4\n",category);
  return *cat;
}


/*
 Find a driver in a category structure returning a pointer to it.
*/
driver_t *FindDriverInCat(category_t *cat, char *driver) {
  driver_t **drv;
  int cont;

DBGprintf("FindDriverInCat> drv=<%s>\n",driver);
  if ((drv=cat->drivers)==NULL) return NULL;
  /* Look for driver. */
  for (cont=cat->used; cont && strcmp(driver,(*drv)->name); cont--, drv++);
DBGprintf("FindDriverInCat> 1\n");
  if (!cont) return NULL;
DBGprintf("FindDriverInCat> 2\n");
  return *drv;
}


/*
 Find a driver in a specified category returning a pointer to it.
*/
driver_t *FindDriver(char *category, char *driver) {
  category_t *cat;

DBGprintf("FindDriver> cat=<%s>, drv=<%s>\n",category,driver);
  cat=FindCategory(category);
  if (!cat) return NULL;
DBGprintf("FindDriver> 1\n");
  return FindDriverInCat(cat,driver);
}


/*
 Find an instance of a driver returning a pointer to it.
*/
drv_instance_t *GetFirstInstance(driver_t *driver) {
DBGprintf("GetFirstInstance>\n");
  return driver->instances;
}


/*
 Find an instance of a driver returning a pointer to it.
*/
drv_instance_t *GetNextInstance(drv_instance_t *instance) {
DBGprintf("GetNextInstance>\n");
  return instance->next;
}


/*
 Create a new category.
*/
category_t *CreateCategory(char *category) {
  category_t **cat, *c;
  int num, n2;

DBGprintf("CreateCategory> 1: cat=<%s>\n",category);
  /* If there's no space, resize array. */
  if (!CategDrv && !InitStruct()) return NULL;
  if (CategDrv->used>=CategDrv->total) {
    num=CategDrv->total+CATEG_BLOCK_NUM;
    cat=MyRealloc(CategDrv->categories,sizeof(category_t *)*CategDrv->total,
                                       sizeof(category_t *)*num);
DBGprintf("CreateCategory> 1a\n");
    if (!cat) return NULL;
DBGprintf("CreateCategory> 1b\n");
    CategDrv->categories=cat;
    CategDrv->total=num;
DBGprintf("CreateCategory> 1c\n");
  } else
    cat=CategDrv->categories;
  /* Create space for sorted inserting. */
DBGprintf("CreateCategory> 2: cat=%u\n",cat);
  for (c=*cat, num=0; num<CategDrv->used && strcmp(category,c->name)>=0;
       num++, c=cat[num]);
DBGprintf("CreateCategory> 3: c=%u, num=%d\n",c,num);
  for (n2=CategDrv->used; n2>num; n2--)
    CategDrv->categories[n2]=CategDrv->categories[n2-1];
  /* Insert category. */
  CategDrv->used++;
DBGprintf("CreateCategory> 4\n");
  if ((c=mallosso(sizeof(category_t)))==NULL) return NULL;
  CategDrv->categories[num]=c;
DBGprintf("CreateCategory> 5: c=%u, num=%d\n",c,num);
  c->total=0;
  c->used=0;
DBGprintf("CreateCategory> 6\n");
  if ((c->name=(char *)mallosso(strlen(category)+1))==NULL) return NULL;
  strcpy(c->name,category);
  c->drivers=0;
DBGprintf("CreateCategory> 7 c->name=%u\n",c->name);
DBGprintf("CreateCategory> 8a CategDrv=%u\n",CategDrv);
DBGprintf("CreateCategory> 8b CategDrv->categories=%u\n",CategDrv->categories);
DBGprintf("CreateCategory> 8c CategDrv->categories[0]=%u\n",CategDrv->categories[0]);
DBGprintf("CreateCategory> 8d CategDrv->categories[0]->name=%u\n",CategDrv->categories[0]->name);

#ifdef DEBUG_LIGHT
  DBGViewData();
#endif

  return c;
}


/*
 Add a driver in the structure and returns a pointer to it.
*/
driver_t *AddDriver(char *category, char *driver, char is_multi,
                    char is_pool) {
  category_t *cat;
  driver_t **drv, *d;
  int num, n2;

DBGprintf("AddDriver> 1: cat=<%s>, drv=<%s>, multi=%d, pool=%d\n",
          category,driver,is_multi,is_pool);
  cat=FindCategory(category);
  if (!cat)
    cat=CreateCategory(category);
  if (!cat) return NULL;
  /* If there's no space, resize array. */
  if (cat->used>=cat->total) {
    num=cat->total+DRIVER_BLOCK_NUM;
    drv=MyRealloc(cat->drivers,sizeof(driver_t *)*cat->total,
                               sizeof(driver_t *)*num);
DBGprintf("AddDriver> 2\n");
    if (!drv) return NULL;
    cat->drivers=drv;
    cat->total=num;
  } else
    drv=cat->drivers;
  /* Create space for sorted inserting. */
  for (d=*drv, num=0; num<cat->used && strcmp(driver,d->name)>=0;
       num++, d=drv[num]);
  for (n2=cat->used; n2>num; n2--)
    cat->drivers[n2]=cat->drivers[n2-1];
  /* Insert category. */
  cat->used++;
DBGprintf("AddDriver> 3\n");
  if ((d=mallosso(sizeof(driver_t)))==NULL) return NULL;
  cat->drivers[num]=d;
DBGprintf("AddDriver> 4\n");
  if ((d->name=(char *)mallosso(strlen(driver)+1))==NULL) return NULL;
DBGprintf("AddDriver> 5\n");
  strcpy(d->name,driver);
  d->bits.is_multi=is_multi;
  d->bits.is_pool=is_pool;
DBGprintf("AddDriver> 6\n");

#ifdef DEBUG_LIGHT
  DBGViewData();
#endif

  return d;
}


/*
 Add an instance to a driver.
*/
drv_instance_t *AddInstance(driver_t *driver) {
  drv_instance_t *inst, *i;

DBGprintf("AddInstance> 1\n");
  inst=(drv_instance_t *)mallosso(sizeof(drv_instance_t));
  if (inst) {
DBGprintf("AddInstance> 2\n");
    inst->next=NULL;
    driver->numinstances++;
    if (driver->instances) {
DBGprintf("AddInstance> 3a\n");
      for (i=driver->instances; i->next; i=i->next);
      i->next=inst;
      inst->prev=i;
    } else {
DBGprintf("AddInstance> 3b\n");
      driver->instances=inst;
      inst->prev=NULL;
    }
    inst->driver=driver;
  }
DBGprintf("AddInstance> 4\n");

DBGprintf("AddInstance> 5: drv->name=<%s>, p1=%d, p2=%d\n",driver->name,
          inst, driver->instances);

  return inst;
}


/*
 Find the driver with the specified pid.
 In 'idx' (if not NULL) returns index of pid in pid list.
 Returns 0 if not found or instance pointer.
*/
drv_instance_t *FindDriverByPid(PID pid, int *idx) {
  driver_pid_t *p=PidList;
  int num=0;

DBGprintf("FindDriverByPid> 1: (pid=%d)\n",pid);
  for (; num<UsedPid; p++, num++)
    if (p->pid==pid) {
DBGprintf("FindDriverByPid> 2: used=%d, num=%d\n",UsedPid,num);
      if (idx) *idx=num;
      return p->instance;
    }
DBGprintf("FindDriverByPid> 3\n");
  return NULL;
}



/*
 Insert a pid in list.
*/
void AddPid(PID pid, drv_instance_t *in) {
  driver_pid_t *p;
  int num, n2;

DBGprintf("AddPid> 1: pid=%d\n",pid);
  /* If there's no space, resize array. */
  if (UsedPid>=TotPid) {
    num=TotPid+PID_BLOCK_NUM;
    p=MyRealloc(PidList,sizeof(driver_pid_t *)*TotPid,
                        sizeof(driver_pid_t *)*UsedPid);
    if (!p) return;
    PidList=p;
    TotPid=num;
  } else p=PidList;
  /* Create space for sorted inserting. */
  for (num=0; num<UsedPid && p->pid<pid; num++, p++);
  for (n2=UsedPid; n2>num; n2--) {
    PidList[n2].pid=PidList[n2-1].pid;
    PidList[n2].instance=PidList[n2-1].instance;
  }
  /* Insert category. */
  UsedPid++;
  PidList[num].pid=pid;
  PidList[num].instance=in;
DBGprintf("AddPid> 2\n");
}





/*------------------------------------------------------------------*/



/*
 Structures initialization.
 Return 0 if error.
*/
int InitStruct(void) {
DBGprintf("InitStruct> 1\n");
  if ((CategDrv=mallosso(sizeof(categories_list_t)))==NULL) return 0;
  CategDrv->total=0;
  CategDrv->used=0;
  CategDrv->categories=NULL;
DBGprintf("InitStruct> 2\n");
  return 1;
}


/*
 Find a driver and return his pid or 0 if not found.
 If PidForWait is not 0 and the driver is loading, the process
 pid (PidForWait) is put in the wait queue for that driver: in
 this case the function returns -1.
*/
PID FindDriverPid(char *category, char *name, PID PidForWait) {
  driver_t *dr=FindDriver(category,name);
  drv_instance_t *in, *in_load=0;

DBGprintf("FindDriverPid> 1: cat=<%s>, drv=<%s>, PidForWait=%d\n",
          category,name,PidForWait);
  if (dr) {
DBGprintf("FindDriverPid> 2: Driver exists\n");
    /* Driver entry exists: find a valid instance */
    for (in=dr->instances; in; in=in->next) {
      if (!in_load && !in->pid && !(dr->bits.is_multi && in->waiting))
        in_load=in;
      if (in->pid && !(dr->bits.is_multi && in->in_use)) {
        in->in_use=1;
DBGprintf("FindDriverPid> 3: Found running: pid=%d\n",in->pid);
        return in->pid;   /* Running instance found! */
      }
    }
DBGprintf("FindDriverPid> 4: in_load=%d\n");
    if (in_load && PidForWait) {
DBGprintf("FindDriverPid> 5\n");
      /* A valid instance is loading. */
      if (dr->bits.is_multi) {
DBGprintf("FindDriverPid> 6a\n");
        /* Driver multi. */
        in->waiting=PidForWait;
        return -1;
      } else {
DBGprintf("FindDriverPid> 6b\n");
        /* Driver mono. */
        if (dr->num_waiting<MAX_WAITQUEUE) {
          dr->waiting[dr->num_waiting]=PidForWait;
          dr->num_waiting++;
          return -1;
        }
      }
    }
  }

//...^^^ Questo case andrebbe posticipato fino a fine boot phase.
DBGprintf("FindDriverPid> DBG1: BootRegisterFinished=%d\n",
          BootRegisterFinished);


DBGprintf("FindDriverPid> 7\n");
  return 0;
}


/*
 Run a driver specified by category/name.
 If pid is not 0 it also put it in the waiting queue of the driver.
 Returns 0 if error, or the pid created if ok.
*/
int RunDriver(char *category, char *name, PID pid) {
DBGprintf("RunDriver> 1: cat=<%s>, drv=<%s>, pid=%d\n",category,name,pid);

  printf("\n\n***** %s: RunDriver not yet implemented!!! *****\n\n\n",__FILE__);

/*-
  char FileName[MAX_ACCEPTABLE_PATH_LENGTH+1], *p=FileName,
       *md=ModulesDir;   // Environment variable...
  PID newpid;

  if (!BootRegisterFinished) return;   // ?!?

  for (*(p++)='/'; *md;) *(p++)=*(md++);
  for (*(p++)='/'; *category;) *(p++)=*(category++);
  for (*(p++)='/'; *name;) *(p++)=*(name++);

  newpid=Exec(FileName);
  if (EXEC_ERROR) {
    if (pid) reply(NOT_EXISTS);
    return 0;
  }
  AddNewDriver(category,name,0,0,version,release,NULL);
  return newpid;
-*/

DBGprintf("RunDriver> 2\n");
  return 0;
}


/*
 Add new driver in the list.
*/
MpsRetVal AddNewDriver(char *category, char *name, PID pid,
                       char multi_instances, int version,
                       int release, char *description)
{
  driver_t *dr;
  drv_instance_t *in;
  int c, newdrv, newinst, numfree;

DBGprintf("AddNewDriver> 1a: cat=<%s>, drv=<%s>, pid=%d, multi=%d\n",
          category,name,pid,multi_instances);
DBGprintf("AddNewDriver> 1b: ver=%d, rel=%d, descr=<%s>\n",
          version,release,description);
  /* Check for errors. */
  if (strlen(category)>MAX_DRIVER_CATEGORY_LEN)
    return ERRDRV_DM_CATTOOLONG;
  if (strlen(name)>MAX_DRIVER_NAME_LEN)
    return ERRDRV_DM_DRVTOOLONG;
  /* Find if driver is in list. */
  dr=FindDriver(category,name);

  if (pid && dr && (dr->bits.is_multi ? 1 : 0)!=(multi_instances ? 1 : 0)) {
    printf("[MPS:%s:AddNewDriver] *** Incompatible definitions!!!\n",
           __FILE__);
    return 0; /*-^^^...-^^^*/
  }

DBGprintf("AddNewDriver> 2\n");
  if (!dr) {
    /* Driver not yet present. I add it. */
DBGprintf("AddNewDriver> 3a\n");
    dr=AddDriver(category,name,multi_instances,0);
    if (!dr) return ERRDRV_NOMEM;  /*... Always? ...*/
    newdrv=1;
    in=0;
  } else {
DBGprintf("AddNewDriver> 3b\n");
    /* Driver present. */
    if (dr->bits.is_multi) {
DBGprintf("AddNewDriver> 3b1\n");
      /* Multi instances driver: look for loading or last instance. */
      for (in=GetFirstInstance(dr); in && in->pid;
           in=GetNextInstance(in));
    } else {
DBGprintf("AddNewDriver> 3b2\n");
      /* Mono instance driver: Check first (and unique) instance. */
      in=GetFirstInstance(dr);
      if (in && in->pid) return ERRDRV_DM_DRVPRESENT;
    }
    newdrv=0;
    newinst=in && !(in->pid);
  }
DBGprintf("AddNewDriver> 4\n");
  /* If not present, add an instance. */
  if (!in) in=AddInstance(dr);
  if (!in) return ERRDRV_NOMEM;  /*... Always? ...*/

#ifdef DEBUG
  if (!newinst && !pid) printf("AddNewDriver> ERRORE #1\n");
#endif

DBGprintf("AddNewDriver> 5: in=%d\n",in);
  /* Insert pid in list. */
  AddPid(pid,in);

  /* Update structures. */
  in->pid=pid;
  in->in_use=0;
  in->driver_version=version;
  in->driver_release=release;

  if (newdrv) dr->num_waiting=0;
  if (newinst) in->waiting=0;

DBGprintf("AddNewDriver> 6\n");
  if (description && *description) {
DBGprintf("AddNewDriver> 6a\n");
    if (in->description) freeosso(in->description);
    if ((in->description=(char *)mallosso(
      strlen(description)+1))==NULL)
    {
      /*^^^ delete the work done?!? */
      printf("[MPS] Allocation error on description!\n");
      return ERRDRV_NOMEM;
    } else {
      strcpy(in->description,description);
    }
  }
DBGprintf("AddNewDriver> 7: in=%d\n",in);
  if (!BootRegisterFinished && pid) {
DBGprintf("AddNewDriver> 7a\n");
    numfree=MAX_WAITQUEUE-dr->num_waiting;
    Boot_RegisterDriver(category,name,pid,&(dr->waiting[dr->num_waiting]),
                        &numfree);
    dr->num_waiting+=(MAX_WAITQUEUE-dr->num_waiting-numfree);
  }

  /* Send driver init reply message. */
  Driver_Init_Reply();

DBGprintf("AddNewDriver> 8\n");
  // ... if pool?!?
//  if (!newdrv) {  ???????????????????????????????????????????????
DBGprintf("AddNewDriver> 9a\n");
    if (dr->bits.is_multi) {
DBGprintf("AddNewDriver> 9a1\n");
      /* Driver multi. */
      c=0;
      if (in->waiting) {
DBGprintf("AddNewDriver> 9a1a\n");
        Send_AskPid_Reply(in->waiting,pid);
        in->in_use=1;
      } else {
DBGprintf("AddNewDriver> 9a1b\n");
        if (dr->num_waiting) {
          Send_AskPid_Reply(dr->waiting[0],pid);
          dr->num_waiting--;
          if (dr->num_waiting)
            memcpy(&(dr->waiting[0]),&(dr->waiting[1]),
                   sizeof(PID)*dr->num_waiting);
          in->in_use=1;
        }
      }
    } else {
DBGprintf("AddNewDriver> 9a2\n");
      /* Driver mono. */
      if (in->waiting) {
DBGprintf("AddNewDriver> 9a2a: prg_pid=%d, drv_pid=%d\n",
          in->waiting,pid);
        Send_AskPid_Reply(in->waiting,pid);
        in->in_use=1;
      }
      for (c=0; c<dr->num_waiting; c++) {
DBGprintf("AddNewDriver> 9a2b: c=%d, prg_pid=%d, drv_pid=%d\n",c,
          dr->waiting[c],pid);
        Send_AskPid_Reply(dr->waiting[c],pid);
      }
      if (c) in->in_use=1;
      dr->num_waiting=0;
    }
//  } ????????????????????????????????????????????????????????
DBGprintf("AddNewDriver> 10: in=%d\n",in);

#ifdef DEBUG_LIGHT
  DBGViewData();
#endif

  return ERRDRV_NOERR;
}


/*
 Delete an instance from the structure.
 Returns error state.
*/
MpsRetVal DeleteInstance(PID pid) {

   /*------------------------*/
   /* THIS MUST BE TESTED!!! */
   /*------------------------*/

  drv_instance_t *inst;
  driver_t *drv;
  int idx;

DBGprintf("DeleteInstance> 1:pid=%d\n",pid);
  // Find pid.
  inst=FindDriverByPid(pid,&idx);
  if (!inst) return ERRDRV_DM_DRVNOTPRESENT;
DBGprintf("DeleteInstance> 2: drv=<%s>\n",((driver_t *)(inst->driver))->name);
  // Delete pid from list.
  for (UsedPid--; idx<UsedPid; idx++) {
    PidList[idx].pid=PidList[idx+1].pid;
    PidList[idx].instance=PidList[idx-1].instance;
  }
DBGprintf("DeleteInstance> 3\n");
  // Delete instance from structure.
  drv=inst->driver;
  if (inst->prev) {
    inst->prev->next=inst->next;
    inst->next->prev=inst->prev;
  } else {
    drv->instances=inst->next;
    inst->next->prev=NULL;
  }
DBGprintf("DeleteInstance> 4\n");
  drv->numinstances--;
  freeosso(inst);
DBGprintf("DeleteInstance> 5: numinstances=<%d>\n",drv->numinstances--);
  return ERRDRV_NOERR;
}


/*
 Close the boot phase loading pending drivers.
*/
void CloseBootPhase(void) {
  category_t **cat=CategDrv->categories;
  driver_t **drv;
  int numcat=CategDrv->used, numdrv, c;

DBGprintf("CloseBootPhase> 1\n");
  if (!BootRegisterFinished) return;
DBGprintf("CloseBootPhase> 2\n");
  /* For each driver... */
  for (; numcat; cat++, numcat--) {
DBGprintf("CloseBootPhase> 2a: numcat=%d\n",numcat);
    for (drv=(*cat)->drivers, numdrv=(*cat)->used; numdrv;
         drv++, numdrv--)
    {
DBGprintf("CloseBootPhase> 2b: numdrv=%d\n",numdrv);
      /* ...run instances needed by programs. */
      for (; c<(*drv)->num_waiting; c++) {
DBGprintf("CloseBootPhase> 2c: c=%d\n",c);
        RunDriver((*cat)->name,(*drv)->name,(*drv)->waiting[c]);
      }
      (*drv)->num_waiting=0;
    }
  }
DBGprintf("CloseBootPhase> 3\n");
}


/*
 Checks if a pid is already present in the structure.
*/
int DriverPidPresent(PID pid) {
  return (FindDriverByPid(pid,NULL) ? 1 : 0);
}
