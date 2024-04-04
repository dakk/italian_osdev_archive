
#include	"keycoda.h"

void	keycoda_init(struct keycoda_t* keycoda)
	{
	int i;

	for(i=0;i<KEY_BUFFER_LUNG;keycoda->dato[i++]=0);
	keycoda->fine=keycoda->inizio=(KEY_BUFFER_LUNG/2);

	return;
	}


int	keycoda_elements(struct keycoda_t* keycoda)
	{
	return ((keycoda->fine-keycoda->inizio+KEY_BUFFER_LUNG)%KEY_BUFFER_LUNG);
	}


unsigned char keycoda_pop(struct keycoda_t* keycoda)
	{
	unsigned char key;

	if (keycoda_elements(keycoda)<=0) return 0;

	key=keycoda->dato[keycoda->fine];
	keycoda->fine=(keycoda->fine-1+KEY_BUFFER_LUNG)%KEY_BUFFER_LUNG;
	return key;
	}


int	keycoda_ins(struct keycoda_t* keycoda,unsigned char key)
	{
	if (keycoda_elements(keycoda)>=(KEY_BUFFER_LUNG-1)) return 0;

	keycoda->dato[keycoda->inizio]=key;
	keycoda->inizio=(keycoda->inizio-1+KEY_BUFFER_LUNG)%KEY_BUFFER_LUNG;

	return 1;
	}

/*
int	keycoda_pick(struct keycoda_t* keycoda,unsigned char key)
	{


	}
*/

void	keycoda_dumpscreen(struct keycoda_t* keycoda)
	{
	int i;

	i=keycoda_elements(keycoda);
	kprint("(%3d)",i);

	kprint("<");
	for(i=(keycoda->inizio+1)%KEY_BUFFER_LUNG;i!=(keycoda->fine+1)%KEY_BUFFER_LUNG;i=(i+1)%KEY_BUFFER_LUNG)
		kprint(" %c",keycoda->dato[i]);
	kprint(" >");

	return;
	}

int	keycoda_dump(char* buffer,struct keycoda_t* keycoda)
	{
	int i;
	int cur;

	i=keycoda_elements(keycoda);
	cur=ksprint(buffer,"(%3d)",i);

	cur+=ksprint(&(buffer[cur]),"<");
	for(i=(keycoda->inizio+1)%KEY_BUFFER_LUNG;i!=(keycoda->fine+1)%KEY_BUFFER_LUNG;i=(i+1)%KEY_BUFFER_LUNG)
		cur+=ksprint(&(buffer[cur])," %c",keycoda->dato[i]);
	cur+=ksprint(&(buffer[cur])," >");

	return cur;
	}

