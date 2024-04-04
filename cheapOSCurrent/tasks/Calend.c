
#include"cheapOSlib.h"

char mesenome[12][4]=
	{
	"Gen",
	"Feb",
	"Mar",
	"Apr",
	"Mag",
	"Giu",
	"Lug",
	"Ago",
	"Set",
	"Ott",
	"Nov",
	"Dic",
	};

void calend_view(int anno,int mese);

int main(int argc,char* argv[])
	{
	char buffer[6];
	unsigned char c;
	int anno,mese;
	int console;

	console=1;

	if(argc==2)
		console=atoi(argv[1]);

	if(!openconsole(console))
		exit(1);

	print("%c",CCLEAR);
	print("%c",CHOME);

	do
		{
		print("Anno:");
		getstring(buffer,6);
		anno=atoi(buffer);
		print("\nMese:");
		getstring(buffer,6);
		mese=atoi(buffer);
		}
	while(mese<1 || mese>12);

	c=' ';
	while(c!='q')
		{
		print("%c",CCLEAR);
		print("%c",CHOME);
		print("\n\n\t%s\t%d\n\n",mesenome[mese-1],anno);
		calend_view(anno,mese);

		print("\n\n\tComandi:\n\t\tsu Anno Successivo\t\t\tgiu Anno Precedente\n");
		print("\t\tdx Mese Successivo\t\t\t sx Mese Precedente\n");
		print("\t\t q Esce");

		c=getchar();
		switch(c)
			{
			case CUP:
				anno++;
			break;

			case CDOWN:
				if(anno>0)
					anno--;
			break;

			case CDX:
				mese++;
				if(mese>12)
					{
					mese=1;
					anno++;
					}
			break;

			case CSX:
				mese--;
				if(mese<1 && anno>0)
					{
					mese=12;
					anno--;
					}
			break;

			default:
				c=c;/*Nulla!*/
			}
		}

	print("\n");
	closeconsole();

	return 0;
	}

void calend_view(int anno,int mese)
	{
	int i;
	int d,m;

	d=(int)(365.25*(anno-(mese<3)))+1;
	d+=(int)(30.6*(mese+1+(mese<3)*12))-(int)( (	(int)((anno-(mese<3))/100)-7)*0.75		);
	d%=7;

	m=31-(mese==4 || mese==6 || mese==9 || mese==11)-(mese==2)*3;

	print("%c\tDom\tLun\tMar\tMer\tGio\tVen\tSab\n",CDKBLUE);
	print("%c",CWHITE);

	for(i=0;i<d;i++)
		print("   \t");

	for(i=d;i<(m+d);i++)
		{
		if(i%7==0 && i!=0)
			print("\n");

		print("\t %2d",(i-d+1));
		}

	return;
	}


