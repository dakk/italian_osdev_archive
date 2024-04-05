
#include	"pidcoda.h"

void	pidcoda_init(struct pidcoda_t* pidcoda)
	{
	int i;

	for(i=0;i<(MAX_TASK_NUM+1);pidcoda->dato[i++]=0);
	pidcoda->fine=pidcoda->inizio=(MAX_TASK_NUM/2);

	return;
	}


int	pidcoda_elements(struct pidcoda_t* pidcoda)
	{
	return ((pidcoda->fine-pidcoda->inizio+MAX_TASK_NUM+1)%(MAX_TASK_NUM+1));
	}


int	pidcoda_pop(struct pidcoda_t* pidcoda)
	{
	int pid;

	if (pidcoda_elements(pidcoda)<=0) return -1;

	pid=pidcoda->dato[pidcoda->fine];
	pidcoda->fine=(pidcoda->fine-1+MAX_TASK_NUM+1)%(MAX_TASK_NUM+1);
	return pid;
	}


int	pidcoda_ins(struct pidcoda_t* pidcoda,int pid)
	{
	if (pidcoda_elements(pidcoda)>=MAX_TASK_NUM) return -1;

	pidcoda->dato[pidcoda->inizio]=pid;
	pidcoda->inizio=(pidcoda->inizio-1+MAX_TASK_NUM+1)%(MAX_TASK_NUM+1);

	return 1;
	}

/*
int	pidcoda_pick(struct pidcoda_t* pidcoda,int pid)
	{


	}
*/
void	pidcoda_dumpscreen(struct pidcoda_t* pidcoda)
	{
	int i;

	i=pidcoda_elements(pidcoda);
	kprint("(%3d)",i);

	kprint("<");
	for(i=(pidcoda->inizio+1)%(MAX_TASK_NUM+1);i!=(pidcoda->fine+1)%(MAX_TASK_NUM+1);i=(i+1)%(MAX_TASK_NUM+1))
		kprint(" %3d",pidcoda->dato[i]);
	kprint(" >");

	return;
	}

int	pidcoda_dump(char* buffer,struct pidcoda_t* pidcoda)
	{
	int i;
	int cur;

	i=pidcoda_elements(pidcoda);
	cur=ksprint(buffer,"(%3d)",i);

	cur+=ksprint(&(buffer[cur]),"<");
	for(i=(pidcoda->inizio+1)%(MAX_TASK_NUM+1);i!=(pidcoda->fine+1)%(MAX_TASK_NUM+1);i=(i+1)%(MAX_TASK_NUM+1))
		cur+=ksprint(&(buffer[cur])," %3d",pidcoda->dato[i]);
	cur+=ksprint(&(buffer[cur])," >");

	return cur;
	}


