
#include	"cheapOSlib.h"

char schermo[70][20];

void gotoxy(int x,int y);
void printcharxy(int x, int y,char c);


int main(int argc,char*argv[])
	{
	int i,j;
	int x,y,x1,y1,xb,yb;
	bool fine;
	int console;
	unsigned char c;

	console=1;

	if(argc==2)
		console=atoi(argv[1]);

	if(!openconsole(console))
		exit(1);

	/*Inizzializza lo schermo!*/
	print("%c",CCLEAR);
	print("%c",CHOME);
	print("%c",CDKGREEN);
	print("\n");

	print("%c",CREVERSE);
	for(j=0;j<70;j++)
			printcharxy(j,0,' ');


	for(i=1;i<19;i++)
		{
		printcharxy(0,i,' ');

		print("%c",CREVERSE);
		for(j=1;j<69;j++)
			printcharxy(j,i,' ');
		print("%c",CREVERSE);

		printcharxy(69,i,' ');
		}

	for(j=0;j<70;j++)
			printcharxy(j,19,' ');
	print("%c",CREVERSE);
	/*Ciclo del Gioco!*/
	x=x1=1;
	y=y1=10;
	fine=FALSE;
	while(x<69 && !fine)
		{
		if(schermo[x][y]!=' ' && schermo[x][y]!='*')
			{
			x=x1;
			y=y1;
			}

		if(schermo[x][y]=='*') goto perso;

		print("%c",CLTGRAY);
		printcharxy(x1,y1,'.');

		print("%c",CDKCYAN);
		printcharxy(x,y,'@');

		xb=randomInt(1,68);
		yb=randomInt(1,18);

		print("%c",CLTRED);
		printcharxy(xb,yb,'*');

		if (xb==x && yb==y) goto perso;


		c=getchar();

		x1=x;
		y1=y;
		switch(c)
			{
			case CUP:
				y-=1;
			break;

			case CDOWN:
				y+=1;
			break;

			case CDX:
				x+=1;
			break;

			case CSX:
				x-=1;
			break;

			case 'q':
				fine=TRUE;
			break;

			default:
				c=c;//Nulla
			}



		}

	if(x>=69) goto vinto;

perso:
	print("%c",CWHITE);
	print("\nHai perso!\n");

	goto fine;

vinto:
	print("%c",CWHITE);
	print("\nBravo! Hai Vinto!");

fine:

	print("\n");
	closeconsole();

	return 0;
	}


void printcharxy(int x, int y,char c)
	{
	gotoxy(x+5,y+2);
	print("%c",c);
	schermo[x][y]=c;

	return;
	}

void gotoxy(int x,int y)
	{
	int i;

	print("%c",CHOME);
	print("\n");

	for(i=0;i<y;i++)
		print("%c",CDOWN);
	for(i=0;i<x;i++)
		print("%c",CDX);

	return;
	}
