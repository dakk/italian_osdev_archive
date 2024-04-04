
#include "floppy.h"

ISR_DECLARE(floppy_handler);

//CACHE
struct cache_t floppy_cache;

//Flag Motore
bool floppy_motore=FALSE;

//Timeout //
int floppy_timeout=0;

//Countdown Motore
int floppy_countdown_motore=0;

// Operazione Finita
bool floppy_operazionefinita=FALSE;

//Flag disco Cambiato
bool floppy_change = FALSE;

//Stato Floppy
unsigned char floppy_stato[7]={0};

//Testina Corrente
unsigned char floppy_track=0xFF;

//Stato Registro ST0
unsigned char ST0=0;

//SEMAPHORO DI MUTUA ESCLUSIONE
semaphoro_t floppy_mutex=VERDE;

// Floppy types
struct floppy_type_t floppy_type[]=
	{
		{2880,18,2,80,0x54,0x1B,0x00},	// 1.44MB
		{3360,21,2,80,0x0C,0x1C,0x00}	// 1.68MB
	};

bool floppy_init()
	{
	kprint("Floppy : OK\n");

	cache_init(&floppy_cache,FLOPPY_CACHESECTOR_NUM);
	kprint("\tCache %d Sector\n",FLOPPY_CACHESECTOR_NUM);

	isr_setISR(0x26,ISR_ADDRESS(floppy_handler));
	irq_enable(6);

	time_addTJ(1,0,floppy_thread);

	return TRUE;
	}


ISR_CODE_H(floppy_handler)
	{
	floppy_operazionefinita=TRUE;
	irq_end();
	return;
	}


void	floppy_thread()
	{
	if (floppy_timeout>0)
		floppy_timeout--;

	if (floppy_countdown_motore>0)
		floppy_countdown_motore--;

	else if(floppy_motore && !floppy_countdown_motore)
		{
		outportb(FLOPPY_DOR,0x0C);
		floppy_motore=FALSE;
		}

	return;
	}


bool	floppy_changed()
	{
	return floppy_change;
	}


void floppy_sendb(unsigned char b)
	{
	int msr;
	int tmo;

	for (tmo=0;tmo<128;tmo++)
		{
		msr=inportb(FLOPPY_MSR);
		if ((msr&0xC0)==0x80)
			{
			outportb(FLOPPY_DATA, b);
			return;
			}
		inportb(0x80);
		}

	return;
	}


unsigned char floppy_getb()
	{
	int msr;
	int tmo;

	for (tmo=0;tmo<128;tmo++)
		{
		msr = inportb(FLOPPY_MSR);
		if ((msr & 0xD0) == 0xD0)
			return(inportb(FLOPPY_DATA));
		inportb(0x80);
		}

	return -1;
	}


void floppy_motoron()
	{

	//kprint("floppy motoron\n");

	if (!floppy_motore)
		{
		outportb(FLOPPY_DOR,0x1C);
		syscall_wait(FLOPPY_TIME_MOTOR_SPINUP);
		floppy_motore=TRUE;
		}

	floppy_countdown_motore=-1;

	return;
	}


void floppy_motoroff()
	{
	//kprint("floppy motoroff in 300 ticks\n");

	if (floppy_motore && (floppy_countdown_motore==-1))
		floppy_countdown_motore=FLOPPY_TIME_MOTOR_OFF;
	}


bool floppy_wait(bool sensei)
	{
	bool opt;
	unsigned char i;

	floppy_timeout=2000;	//2 sec

	//kprint("floppy wait\n");

	opt=(!floppy_operazionefinita)&&(floppy_timeout);

	cpu_sti();
	while(opt)
		{
		cpu_cli();
		opt=(!floppy_operazionefinita)&&(floppy_timeout);
		cpu_sti();
		}
	cpu_cli();

	i = 0;
	while ((i < 7) && (inportb(FLOPPY_MSR) & MSR_BUSY))
		floppy_stato[i++] = floppy_getb();


	if (sensei)
		{
		floppy_sendb(FLOPPY_CMD_SENSEI);
		ST0 = floppy_getb();
		floppy_track = floppy_getb();
		}

	// Timed out? //
	if (!floppy_operazionefinita)
		{
		if (inportb(FLOPPY_DIR) & 0x80)
			floppy_change=TRUE;
		return FALSE;
		}

	floppy_operazionefinita=FALSE;
	return TRUE;
	}


bool floppy_seek(unsigned char track)
	{

	//kprint("floppy seek: %3d\n",track);

	if (floppy_track==track)
		return TRUE;

	floppy_motoron();

	floppy_sendb(FLOPPY_CMD_SEEK);
	floppy_sendb(0);
	floppy_sendb(track);

	if(!floppy_wait(TRUE))
		{
		floppy_motoroff();
		return FALSE;
		}

	syscall_wait(150);

	floppy_motoroff();

	if((ST0!=0x20)||(floppy_track!=track))
		return FALSE;

	return TRUE;
	}


void floppy_reset()
	{

	//kprint("floppy reset\n");

	// Stop the motor and disable IRQ //
	outportb(FLOPPY_DOR,0x00);

	// Program data rate (500K/s) //
	outportb(FLOPPY_DSR,0x00);

	// re-enable floppy IRQ //
	outportb(FLOPPY_DOR,0x0C);

	// Wait for fdc //
	floppy_operazionefinita=TRUE;
	floppy_wait(TRUE);

	// Specify drive timings //
	floppy_sendb(FLOPPY_CMD_SPECIFY);
	floppy_sendb(0xDF); // SRT = 3ms, HUT = 240ms
	floppy_sendb(0x02); // HLT = 4ms, ND = 0 (DMA mode selected)

	// Clear disk change flag //
	floppy_seek(1);
	floppy_recalibrate();
	floppy_change=FALSE;

	return;
	}

void floppy_recalibrate()
	{
	//kprint("floppy recalibrate\n");

	floppy_motoron();

	floppy_sendb(FLOPPY_CMD_RECAL);
	floppy_sendb(0);

	floppy_wait(TRUE);

	floppy_motoroff();

	return;
	}

void floppy_lba2chs(int lba, unsigned char *track, int *head, int *sector)
	{
	*track = lba / (floppy_type[FLOPPY_GEOMETRY].spt * floppy_type[FLOPPY_GEOMETRY].heads);
	*head = (lba / floppy_type[FLOPPY_GEOMETRY].spt) % floppy_type[FLOPPY_GEOMETRY].heads;
	*sector = (lba % floppy_type[FLOPPY_GEOMETRY].spt) + 1;

	return;
	}

bool floppy_rw(int block,mem_t buffer,bool read)
	{
	int head,sector,tries;
	unsigned char track;

	semaphoro_down(&floppy_mutex);					//Mutua Esclusione!!!

	floppy_lba2chs(block, &track, &head, &sector);

	//CACHE
	if (read)
		if(cache_findcopy(&floppy_cache,block,buffer))
			{
			semaphoro_up(&floppy_mutex);					//Mutua Esclusione!!!
			return TRUE;
			}

	floppy_motoron();

	if (!read)
		mem_copy(buffer,DMABUFFER_BASE,512);

	for(tries=0; tries<3; tries++)
		{
		// Check for disk changed //
		if (inportb(FLOPPY_DIR) & 0x80)
			{
			floppy_change=TRUE;
			floppy_seek(1);
			floppy_recalibrate();
			floppy_motoroff();
			semaphoro_up(&floppy_mutex);
			return FALSE;
			}

		// Move the head to the right track //
		if (!floppy_seek(track))
			{
			floppy_motoroff();
			semaphoro_up(&floppy_mutex);
			return FALSE;
			}

		// Program data rate //
		outportb(FLOPPY_CCR, floppy_type[FLOPPY_GEOMETRY].rate);

		// Send command //
		if (read)
			{
			// Read operation //
			//kprint("floppy read\n");
			dma_xfer(2,DMABUFFER_BASE,FLOPPY_SECTOR_SIZE,FALSE);
			floppy_sendb(FLOPPY_CMD_READ);
			}
		else
			{
			// Write operation //
			//kprint("floppy  write\n");
			dma_xfer(2,DMABUFFER_BASE,FLOPPY_SECTOR_SIZE,TRUE);
			floppy_sendb(FLOPPY_CMD_WRITE);
			}

		floppy_sendb(head << 2);
		floppy_sendb(track);
		floppy_sendb(head);
		floppy_sendb(sector);
		floppy_sendb(2); // 512 bytes/sector //
		floppy_sendb(floppy_type[FLOPPY_GEOMETRY].spt);
		floppy_sendb(floppy_type[FLOPPY_GEOMETRY].rwgap);
		floppy_sendb(0xFF); // DTL = unused //

		if (!floppy_wait(FALSE))
			{
			semaphoro_up(&floppy_mutex);
			return FALSE;
			}

		if ((floppy_stato[0]&0xC0)==0)
			break; // Worked! :) //

		floppy_recalibrate();
		}


	floppy_motoroff();

	if (read)
		mem_copy(DMABUFFER_BASE,buffer,512);

	//CACHE
	cache_insertrefresh(&floppy_cache,block,buffer);

	semaphoro_up(&floppy_mutex);

	return (tries != 3);
	}

bool floppy_read(int block,mem_t buffer,unsigned long count)
	{
	int i;

	for(i=0; i<count; i++)
		if(!(floppy_rw(block+i,buffer+(FLOPPY_SECTOR_SIZE*i),TRUE)))
			return FALSE;


	return TRUE;
	}

bool floppy_write(int block,mem_t buffer,unsigned long count)
	{
 	int i;

	for(i=0;i<count;i++)
		if(!(floppy_rw(block+i,buffer+(FLOPPY_SECTOR_SIZE*i),FALSE)))
			return FALSE;

	return TRUE;
	}

