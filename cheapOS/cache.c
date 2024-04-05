#include	"cache.h"

bool cache_init(struct cache_t* cache,int maxblocknum)
	{
	cache->maxblocknum=0;

	cache->block=(struct cache_element_t*)mem_alloc(maxblocknum*sizeof(struct cache_element_t));
	if(cache->block==0) return FALSE;

	cache->maxblocknum=maxblocknum;
	cache_clear(cache);

	return TRUE;
	}

void	cache_clear(struct cache_t* cache)
	{
	int i;

	cache->misscount=cache->hitcount=0;
	cache->cachedblocknum=0;

	for(i=0;i<cache->maxblocknum;i++)
		{
		cache->block[i].lba=-1;
		cache->block[i].accessi=-1;
		}

	return;
	}

bool cache_findcopy(struct cache_t* cache,int lba,mem_t buffer)
	{
	int i;

	for(i=0;i<cache->maxblocknum;i++)
		if(cache->block[i].lba==lba)
			{
			mem_copy((mem_t)(cache->block[i].image),buffer,512);

			if(++cache->block[i].accessi>CACHE_MAXACCESSI)
				cache->block[i].accessi/=2;

			cache->hitcount++;
			return TRUE;
			}

	cache->misscount++;

	return FALSE;
	}

bool cache_insertrefresh(struct cache_t* cache,int lba,mem_t buffer)
	{
	int i;
	int posto;

	for(i=0;i<cache->maxblocknum;i++)
		if(cache->block[i].lba==lba)
			{
			mem_copy(buffer,(mem_t)(cache->block[i].image),512);
			return FALSE;
			}

	posto=0;

	for(i=0;i<cache->maxblocknum;i++)
		if((cache->block[i].accessi)<(cache->block[posto].accessi))
			{
			posto=i;
			if(cache->block[posto].accessi<0) break;
			}
			
	if(cache->block[posto].lba<0)
		(cache->cachedblocknum)++;


	mem_copy(buffer,(mem_t)(cache->block[posto].image),512);

	cache->block[posto].lba=lba;
	cache->block[posto].accessi=0;

	return TRUE;
	}

void cache_dumpscreen(struct cache_t* cache)
	{
	int i,j;

	i=j=0;
	if((cache->misscount+cache->hitcount)!=0)
		{
		i=100*cache->misscount/(cache->misscount+cache->hitcount);
		j=100*cache->hitcount/(cache->misscount+cache->hitcount);
		}

	kprint("Cached Block: %d\tAccessi: %d\tHit %d/100\tMiss %d/100\n",cache->cachedblocknum,(cache->misscount+cache->hitcount),j,i);

	for(j=i=0;i<cache->maxblocknum && j<cache->cachedblocknum;i++)
		if(cache->block[i].lba>=0)
			{
			kprint("[%3x (%4d)]\t",cache->block[i].lba,cache->block[i].accessi);
			if((j%5)==4) kprint("\n");
			j++;
			}

	return;
	}


int	cache_dump		(char*buffer,struct cache_t* cache)
	{
	int cars;
	int i,j;

	i=j=0;
	if((cache->misscount+cache->hitcount)!=0)
		{
		i=100*cache->misscount/(cache->misscount+cache->hitcount);
		j=100*cache->hitcount/(cache->misscount+cache->hitcount);
		}

	cars=ksprint(buffer,"Cached Block: %d\tAccessi: %d\tHit %d/100\tMiss %d/100\n",cache->cachedblocknum,(cache->misscount+cache->hitcount),j,i);

	for(j=i=0;i<cache->maxblocknum && j<cache->cachedblocknum;i++)
		if(cache->block[i].lba>=0)
			{
			cars+=ksprint(&(buffer[cars]),"[%3x (%4d)]\t",cache->block[i].lba,cache->block[i].accessi);
			if((j%5)==4) cars+=ksprint(&(buffer[cars]),"\n");
			j++;
			}

	return cars;
	}


