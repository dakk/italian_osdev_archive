
#include	"cheapOSlib.h"

void lavoro(int cicli);

int main(int argc,char** argv)
	{
	int pid,i;

	print("Ciao da Task %3d\n",getpid());

	pid=1;
	for(i=0;i<20 && pid>0;i++)
		{
		pid=fork();
		if(pid==0) lavoro(30);
		wait(200);
		}

	print("Task %3d Esce!\n",getpid());

	return 0;
	}

void lavoro(int cicli)
	{
	int i,j,k,count;

	for(i=0;i<cicli;i++)
		{
		count=0;
		for(j=0;j<10000;j++)
			for(k=0;k<10000;k++)
				{
				count++;
				}
		count/=2;
		}

	return;
	}

