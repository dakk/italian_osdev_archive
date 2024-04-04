/*!	\file fs/ext2/ext2.c
 *	\brief ext2 file system.
 *	\author Filippo Brogi
 *	\note Copyright (&copy;) 2003 Filippo Brogi
 *	\date Last update:\n
 *		2003-09-30 Andrea Righi:
 *			init_ext2() now returns a boolean value.\n
 */

#include <const.h>
#include <stdio.h>
#include <string.h>

#include <arch/i386.h>
#include <arch/mem.h>

#include <kernel/debug.h>
#include <kernel/Ide.h>
#include <kernel/IdeDebug.h>
#include <kernel/IdeTimer.h> // must remove
#include <kernel/IdeLow.h>   // must remove
#include <kernel/keyboard.h>
#include <kernel/kmalloc.h>
#include <kernel/video.h>

#include <kernel/ext2.h>

#define min(a,b) (((a)<(b)) ? (a) : (b))

struct super_block *super; // puntatore al superblocco
// contenuto di un blocco su disco si usa come variabile ausiliaria nelle func
byte *data_block;
// codice errore lettura da disco
int err;


bool mount_ext2 = FALSE; // TRUE se il montaggio del file system é andato a buon fine
char path_ext2[1024];
int level;



word spb,bpg,gdpb,ipb,sbpos;
dword number_of_groups; // numero gruppi di blocchi nel file
dword dim_block,dim_ptr,dim_frag; // dimensione del blocco dati e puntatore
dword inodes_per_block; // numero inode per blocco
dword dir_entries_per_block; // numero directory entries per block



//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//                  FUNZIONI AUSILIARIE

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// TODO: il 63 va letto dalle partition table del MBR
dword blocco_to_LBA(dword num_block){
	return(63 +(num_block*spb));

}


void stampa_bg(struct group_descriptor *bg){

	kprintf ("\n\n\rid primo blocco bitmap blocchi: %u",bg->bg_block_bitmap);
	kprintf ("\n\rid primo blocco bitmap inode: %u",bg->bg_inode_bitmap);
	kprintf ("\n\rid primo blocco tabella inode: %u",bg->bg_inode_table);
	kgetchar();
}


/*
    lettura del descrittore di gruppo. In generale é allocato
    consecutivamente al blocco del disco contenente il super
    blocco, cioè blocco 1 della partizione
*/


// il primo descrittore di gruppo parte da 0
bool ReadGroupDescriptor(dword grp,struct group_descriptor* data){

    if (grp>number_of_groups)
    {
        return FALSE;
    }

    return TRUE;
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//                  GESTIONE DESCRITTORI DI GRUPPO

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// struttura tabella dei descrittori di gruppo
struct group_descriptor  *group_desc_table;



//------------ Inizializzazione Tabella Descrittori di gruppo ------------//
bool init_group_desc_table(){
	if(!(group_desc_table = (struct group_descriptor*)
		kmalloc(sizeof(struct group_descriptor)*number_of_groups, GFP_KERNEL)))
	{
		kprintf ("\n\rImpossible create group descriptor table: out of memory");
		return FALSE;
	}
	memset(group_desc_table,0,sizeof(struct group_descriptor)*number_of_groups);

	data_block = kmalloc(dim_block, GFP_KERNEL);
	memset(data_block,0,dim_block);


	if ( (err = ReadSectorsLba(0,(int64_t)blocco_to_LBA(sbpos),spb,(word *)data_block,TRUE)) )
	{
		// si stampa l'errore relativo al fallimento lettura disco
		kprintf ("\n\rerror reading group descriptor block");
		ShowIdeErrorMessage(err,TRUE);
		return FALSE;
	}
	memcpy(group_desc_table,data_block, sizeof(struct group_descriptor)*number_of_groups);

	kfree(data_block);
	
	return TRUE;

}


struct group_descriptor * get_group_desc(dword grp){

	if(grp>number_of_groups)
	{
		kprintf ("\n\rInvalid group descriptor number");
		return NULL;
	}
	return(&group_desc_table[grp]);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//                      GESTIONE DEGLI INODE

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// struttura tabella inode
struct i_node_tab
{
	dword i_node_n; // numero dell'inode
	struct i_node inode; // informazioni generali inode
	// parametri per la gestione tabella inode
	word ref;
} *inode_table;

word dim_inode_table;
word free_inode; //numero elementi della tabella inode liberi

// stampa su video il contenuto di un inode

void stampa_i(struct i_node *ino){
int i;

	kprintf ("\n\n\rtipo inode: %X",ino->i_mode & MODE_MASK);
	kprintf ("\n\ruser id associato al file: %u",ino->i_uid);
	kprintf ("\n\rfile size (byte): %u",ino->i_size);
	kprintf ("\n\rnumero blocchi riservati al file: %u",ino->i_blocks);
	for (i=0;i<15;i++)
	{
		kprintf ("\n\rblock[%u]: %u",i,ino->i_block[i]);
	}
	kgetchar();
}

// dato un inode mi dice dove é il blocco in cui si trova (0 errore lettura)
dword Inode2Block(dword ino){
	struct group_descriptor* group_desc;

	if (!ino || ino>super->s_inodes_count)
	{
		kprintf ("\n\rinvalid inode number");
		return 0;
	}

	// il primo inode parte da 1
	ino--;


	// si cerca il descrittore del gruppo di blocchi in cui si trova l'inode
	if (!(group_desc = get_group_desc(ino/super->s_inodes_per_group)))
	{
		kprintf ("\n\rNot group desc per inode");
		return 0;
	}

	ino %= super->s_inodes_per_group;

	// bg_inode_table punta al primo blocco tabella inode
	//kprintf("\n\r%u",group_desc->bg_inode_table+ino/ipb);
	return (group_desc->bg_inode_table+ino/ipb);
}


// lettura di un inode dal disco
bool ReadInode(dword ino, struct i_node* data){
	dword ino_block = Inode2Block(ino);
	if (!ino_block)
	{
		kprintf ("\n\rerror finding inode block");
		return FALSE;
	}
    
	data_block = kmalloc(dim_block, GFP_KERNEL);
	memset(data_block,0,dim_block);
	//kprintf("\n\rino_block: %u",ino_block);
	// TODO : si deve leggere in data_block il blocco del disco che contiene l'inode
	if ( (err = ReadSectorsLba(0,(int64_t)blocco_to_LBA(ino_block),spb,(word *)data_block,TRUE)) )
	{
		// errore nella lettura del blocco relativo all'inode
		kprintf ("\n\rerror reading inode block");
		ShowIdeErrorMessage(err,TRUE);
		return FALSE;
	}
	// si copia l'inode relativo
	memcpy(data,&data_block[((ino-1)%ipb)*sizeof(struct i_node)],sizeof(struct i_node));
	//stampa_i(data);
	kfree(data_block);
	return TRUE;
}

//--------------- Inizializzazione Tabella Inode ---------------//
bool init_inode_table(){
	int i;
	// la cache degli inode può contenere il 1% degli inode su disco
	dim_inode_table = super->s_inodes_count / 100;
	if(!(inode_table = (struct i_node_tab*)
		kmalloc(sizeof(struct i_node_tab)*dim_inode_table, GFP_KERNEL)))
	{
		kprintf("\n\rImpossible create inode table: out of memory");
		return FALSE;
	}
	memset(inode_table,0,sizeof(struct i_node_tab)*dim_inode_table);
	for (i = 0;i < dim_inode_table; i++)
	{
		inode_table[i].i_node_n = 0;
		inode_table[i].ref = 0;
	}
	// tutti gli elementi della tabella sono liberi
	free_inode = dim_inode_table;
	//kprintf("\n\rtabella inode creata correttamente");
	return TRUE;
}

//---------------- Cerca elemento LRU -----------------------//
int inode_LRU(){
	word lru = 0;
	int i;

	// l'elemento cercato é quello meno referenziato negli ultimi
	// 16 riferimenti in memoria, la variabile ref é a 16 bit

	for (i = 1;  i<dim_inode_table; i++)
		if (inode_table[i].ref < inode_table[lru].ref)
		{
			lru = i;
		}
	//kprintf("\n\rci sono passato lru: %u %u",lru,inode_table[lru].ref);
	return (lru);

}


//--------------- Cerca inode e restituisce la posizione -------------------//

struct i_node *get_inode(dword i_node_number){
	int i;
	int pos_inode = -1;
	struct i_node new_inode;

	// controlliamo la validità dell'inode
	if (!i_node_number || i_node_number>super->s_inodes_count)
	{
		kprintf("\n\rInvalid inode number");
		return(NULL);
	}

	//kprintf("\n\r inode_number: %u",i_node_number);

	// si cerca se l'inode é presente nella tabella
	for (i=0; i < (dim_inode_table-free_inode); i++)
		if (inode_table[i].i_node_n == i_node_number)
		{
			// memorizza posizione inode
			pos_inode = i;
			break;
		}

	if (pos_inode == -1)
	{
		// l'elemento non é presente nella cache dobbiamo reperirlo su disco
		ReadInode(i_node_number,&new_inode);
		//stampa_i(&new_inode);

		if (free_inode>0) // ci sono posizioni non utilizzate
		{
			pos_inode = dim_inode_table - free_inode;
			free_inode--; // decremento numero posizioni libere
		}
		else
		{
			pos_inode = inode_LRU();
		}
		inode_table[pos_inode].inode = new_inode; // inserisco nella tabella
		inode_table[pos_inode].i_node_n = i_node_number; // inserisco numero inode
		inode_table[pos_inode].ref = 0; // azzero il riferimento
	}

	// aggiornamento dei riferimenti
	for (i=0; i < (dim_inode_table-free_inode); i++)
	{
		inode_table[i].ref = inode_table[i].ref >> 1;
		if (i == pos_inode)
		{
			inode_table[i].ref = inode_table[i].ref | 0x8000;
		}
	}

	//kprintf ("\n\rinode inserito nella tabella e restituito");
	//kprintf ("\n\rpos_inode %u",pos_inode);
	//kprintf ("\n\rinode number %u",inode_table[pos_inode].i_node_n);
	//stampa_i(&inode_table[pos_inode].inode);
	// restituisco puntatore all'inode cercato
	return (&inode_table[pos_inode].inode);

}

//############################################################

//              GESTIONE DEI FILE

//############################################################

dword * ptr_dir;

// funzione che permette di verificare se é un file regolare
bool isFile(struct i_node* ino){

	if ((ino->i_mode & MODE_MASK)==MODE_FILE)
	{
		return TRUE;
	}
	return FALSE;
}

// funzione che permette di determinare se é un fast symbolic link
bool isFastSymbolicLink(struct i_node* ino){

	if ((ino->i_mode & MODE_MASK)==MODE_LINK)
	{
		return TRUE;
	}
	return FALSE;
}


bool ReadIndirect1(dword* dst, dword* cnt, dword blk) {
	dword* r1;
	int m;
	// controlliamo se ci sono blocchi puntati indirettamente
	if(*cnt<=0)
	{
		return TRUE;
	}

	//kprintf("\n\r read indirect 1");
	/* dword* */ r1 = (dword *)kmalloc(dim_ptr * sizeof(dword), GFP_KERNEL);
	memset(r1,0,dim_ptr * sizeof(dword));
	// si deve leggere il blocco dati puntato indirettamente

	if ( (err = ReadSectorsLba(0,(int64_t)blocco_to_LBA(blk),spb,(word *)r1,TRUE)) )
	{
		kprintf ("\n\rerror reading indirect addressing");
		ShowIdeErrorMessage(err,TRUE);
		kfree(r1);
		return FALSE;
	}

	/*int*/ m = min(*cnt,dim_ptr);
	// se cnt < dim_ptr vuol dire che non tutti gli indici
	// dei blocchi del blocco indice sono occupati dal file
	memcpy(dst,r1,m<<2);
	kfree(r1);
	*cnt -= m;
	dst += m;
	return TRUE;
}

bool ReadIndirect2(dword* dst, dword* cnt, dword blk) {
	int i;
	dword* r2;
	if(*cnt<=0)
	{
		return TRUE;
	}

	//kprintf("\n\r read indirect 2");
	/*dword* */ r2 = (dword *)kmalloc(dim_ptr * sizeof(dword), GFP_KERNEL);
	if ( (err = ReadSectorsLba(0,(int64_t)blocco_to_LBA(blk),spb,(word *)r2,TRUE)) )
	{
		kprintf ("\n\rerror reading second level indirect addressing");
		ShowIdeErrorMessage(err,TRUE);
		kfree(r2);
		return FALSE;
	}

	for (i = 0; *cnt && i<dim_ptr; i++)
		if(!ReadIndirect1(dst,cnt,r2[i]))
			{
				kfree(r2);
				return FALSE;
			}

	kfree(r2);
	return TRUE;
}

// apertura di un file sia esso directory, link o file

bool Open_File (struct i_node* ino,word tipo_file){

	int i;
	char * fsl;
	dword n,m;
	dword *ptr;
	if (ino == NULL)
	{
		// si é verificato un errore nessun inode specificato
		kprintf ("\n\rInvalid inode\n\r");
		return FALSE;
	}

	// check di validità dell'inode
	if (!(tipo_file == MODE_DIR || tipo_file == MODE_FILE || tipo_file == MODE_LINK))
	{
		// tipo non valido
		kprintf ("\n\rInvalid file type\n\r");
		return FALSE;
	}

	// TODO : fast symbolic link
	if ((tipo_file==MODE_LINK) && (ino->i_size<64))
	{
		kprintf ("\n\rFast symbolic link path : ");

		fsl = (byte *)kmalloc(sizeof(struct i_node), GFP_KERNEL);
		memset(fsl,0,sizeof(struct i_node));
		memcpy(fsl,ino,sizeof(struct i_node));
		for (i=39;i<(ino->i_size+39);i++)
			kprintf("%c",fsl[i]);
		kprintf("\n\n\r");
		kfree(fsl);
		ptr_dir = NULL;
		return FALSE;
	}

	// lettura di tutti i puntatori ai blocchi dati del file

	// numero di blocchi che compongono il file
	/*dword*/ n = (ino->i_size+dim_block-1)/dim_block;

	// si linearizzano tutti i puntatori ai blocchi dati in modo
	// tale da semplificare la struttura gerarchica ed ottimizzare
	// l'accesso ai blocchi dati veri e propri
	ptr_dir = (dword *)kmalloc(n * sizeof(dword), GFP_KERNEL);
	memset(ptr_dir,0,n * sizeof(dword));

	// scorre l'area di memoria 32 bit alla volta
	/*dword * */ptr = (dword *)ptr_dir;

	// i primi 12 blocchi sono indirizzati direttamente
	/*dword*/ m = (n<12) ? n : 12;

	// memcpy copia byte per byte perciò m<<2
	if (m) memcpy(ptr,ino->i_block,m<<2);

	//kprintf("\n\rstampo ptr:%u n:%u m:%u",ptr_dir[0],n,m);
	//kgetchar();
	// blocchi puntati indirettamente
	n-=m;
	// ptr avanza per indirizzare la memoria contenente i blocchi indiretti
	ptr += m;
	// se ci sono blocchi indiretti si aprono
	if (!ReadIndirect1(ptr,&n,ino->i_block[12]))
	{
		//kprintf ("\n\rind1");
		kfree(ptr_dir);
		return FALSE;
	}
	// indirizzamento indiretto doppio
	if (!ReadIndirect2(ptr,&n,ino->i_block[13]))
	{
		//kprintf ("\n\rind2");
		kfree(ptr_dir);
		return FALSE;
	}
	// indirizzamento indiretto triplo
	if(n)
	{
		//kprintf ("\n\rind3");
		dword* r3 = (dword *)kmalloc(dim_ptr * sizeof(dword), GFP_KERNEL);
		// TODO : lettura indirizzamento triplo
		if( (err = ReadSectorsLba(0,blocco_to_LBA(ino->i_block[14]),spb,(word *)r3,TRUE)) )
		{
			kprintf ("\n\rerror reading third level indirect addressing");
			ShowIdeErrorMessage(err,TRUE);
			kfree(r3);
			return FALSE;
		}

		// TODO : ricontrollare la definizione di i indice
		for(i = 0; n && i<dim_ptr; i++)
			if(!ReadIndirect2(ptr,&n,r3[i]))
			{
				kfree(ptr_dir);
				kfree(r3);
				return FALSE;
			}
		kfree(r3);
	}

    return TRUE;
    // a questo punto ptr_dir contiene il valore dei puntatori ai blocchi
    // del file aperto
}





//çççççççççççççççççççççççççççççççççççççççççççççççççççççççççççç

//              GESTIONE DELLE DIRECTORY

//çççççççççççççççççççççççççççççççççççççççççççççççççççççççççççç

// variabili per gestire gli elementi delle directory
word count_dir; // numero elementi della dir
char** ext2_file_name = NULL; // filename
dword* inode_dir = NULL; // inodes
byte* file_type_dir = NULL; //file type
char* text = NULL; // ext2_file_name[] punta dentro la stringa di caratteri
char* name = NULL;


// elementi che si usano per il backup della memoria
char path_ext2_backup[1024];
int level_backup;
dword ino_dir_backup,ino_current_dir;


// funzione che permette di verificare se é un file
bool isDir(struct i_node* ino){

	if ((ino->i_mode & MODE_MASK)==MODE_DIR)
	{
		return TRUE;
	}
	return FALSE;
}

// dato il nome trova l'inode (0 errore)
dword FindFile(char* cmp){
	int i;

	for (i=0;i<count_dir;i++)
		if (!strcmp(cmp,ext2_file_name[i]))
		{
			return inode_dir[i];
		}
	return 0;
}


// Apertura di una directory dato l'inode relativo, FALSE = errore
bool Open_Dir(struct i_node* inode){

	byte *ptr_block;
	dword block_dir; // blocchi che compongono la directory
	//dword *ptr_dir; // puntatori blocchi disco contenenti la directory
	byte *tmp; // memorizza il contenuto della direcotry
	int i,len,idx;
	struct dir_ff *dir;


	//controlliamo se si tratta effettivamente di una directory

	if (!(isDir(inode) || isFastSymbolicLink(inode)))
	{
		return FALSE;
	}

	// si restituisce la memoria allocata per i campi se già utilizzati
	// if (name != NULL)
	if (ext2_file_name != NULL)
	{
		kfree(inode_dir);
		kfree(file_type_dir);
		kfree(text);
		kfree(ext2_file_name);
	}

	// se tutto va bene si procede all'apertura memorizzando i puntatori
	// ai blocchi dati nell'area di memoria puntata da ptr_dir
	if (!(Open_File(inode,inode->i_mode & MODE_MASK)))
	{
		return FALSE;
	}


	// dimensione in blocchi della directory
	block_dir = (inode->i_size + dim_block - 1)/dim_block;

	// inserimento nella sezione di memoria tmp dei dati relativi alla directory
	tmp = (byte *)kmalloc(inode->i_size, GFP_KERNEL);
	memset(tmp,0,inode->i_size);


	//punta all'inizio dell'area di memoria tmp
	ptr_block = (byte *)tmp;

	//memset(ptr_block,0,dim_block);
	for (i = 0; i<block_dir;i++)
	{
		//kprintf("\n\rptr_dir[%u]: %u",i,ptr_dir[i]);
		if ( (err = ReadSectorsLba(0,(int64_t)blocco_to_LBA(ptr_dir[i]),spb,(word *)ptr_block,TRUE)) )
		{
			kprintf ("\n\rError reading data block");
			ShowIdeErrorMessage(err,TRUE);
        		kfree(ptr_dir);
			kfree(tmp);
			return FALSE;
        	}
        	ptr_block += dim_block;
	}

	kfree(ptr_dir);


	// passo 1 : conteggio elementi della directory

	count_dir = 0; // memorizza il numero di elementi della dir
	len = 0;
	// i si incrementa della dimensione degli elementi della directory
	// len si incrementa della lunghezza degli elementi +1 per il fine stringa
	// dir punta ad ogni passo alla sequenza di byte di ogni elemento della dir
	for (i = 0; i<inode->i_size; )
	{
		dir = (struct dir_ff*)&tmp[i];
		if (dir->inode) // se uguale a zero vuol dire non utilizzato
		{
			//TODO: nel caso in cui non si stampino i fsl
			count_dir++;
			len += dir->name_len + 1;
		}
		i += dir->rec_len;
	}

	// vettore che contiene gli inode dei file nella directory
	inode_dir = (dword *)kmalloc(count_dir * sizeof(dword), GFP_KERNEL);
	memset(inode_dir,0,count_dir * sizeof(dword));
	// vettore che contiene il tipo dei file nella directory
	file_type_dir = (byte *)kmalloc(count_dir, GFP_KERNEL);
	memset(file_type_dir,0,count_dir);
	// nome dei file degli elementi della directory
	// *** Andrea Righi 2003-10-03 *********************************//
	// name = (char *)kmalloc(count_dir, GFP_KERNEL);
	// memset(name,0,count_dir);
	// ext2_file_name = &name;
	//**************************************************************//
	ext2_file_name = kmalloc(count_dir*sizeof(char *), GFP_KERNEL);

	text = (char *)kmalloc(len, GFP_KERNEL);
	memset(text,0,len);

	// secondo passo si registrano gli elementi
	idx = 0;
	for (len = i = 0;i<inode->i_size;)
	{
		dir = (struct dir_ff*)&tmp[i];
		// si memorizzano gli indirizzi degli inode degli elementi
		if (dir->inode)
		{
			inode_dir[idx] = dir->inode;
			file_type_dir[idx] = dir->file_type;
			// copia dell'elemento di memoria
			memcpy(&text[len],dir->name,dir->name_len);
			// si azzera l'elemento dopo il nome
			text[len+dir->name_len] = '\0';
			//punta al corrispondente nome di file
			ext2_file_name[idx++] = &text[len];
			len += dir->name_len + 1;
		}
		i += dir->rec_len;
	}

	// si restituisce la memoria occupata da tmp
	kfree(tmp);

	return TRUE;
}


/**************************************************************************
*
*           Procedure che definiscono comandi della shell per Ext2
*
***************************************************************************/


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//			Gestione date relative ai file

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct date_ext2{
    dword giorno;
    dword mese;
    dword anno;
}data;

struct time_ext2{
    dword secondi;
    dword minuti;
    dword ora;
}tempo;


struct time_ext2 det_time(dword ino_time){
	struct time_ext2 t;
	dword s;

	s = ino_time % 86400;
	// ora
	t.ora = s / 3600;
	s -= t.ora*3600;
	// minuti
	t.minuti = (s / 60);
	s -= t.minuti*60;
	// secondi
	t.secondi = s;
	return(t);

}

struct date_ext2 det_date(dword ino_time){
	struct date_ext2 d;
	dword mg;

	ino_time = ino_time / 86400; //(60*60*24)

	// tempo in giorni trascorso dal 1/1/1970

	mg = ino_time + 1;

	// si determina l'anno
	for (d.anno=1970;;d.anno++)
	{
		if ((d.anno % 4)==0) // bisestile
		{
			if (mg<366) break;
			mg -= 366;
		}
		else
		{
			if (mg<365) break;
			mg -= 365;
		}

	}

	// mg contiene mese e giorno
	for (d.mese=1;;d.mese++)
	{

		if ((d.mese==4)||(d.mese==6)||(d.mese==9)||(d.mese==11))
		{
			if (mg<=30) break;
			mg -= 30;
		}
		else if ((d.mese==2)&&(d.anno%4==0))
			{
				if (mg<=29) break;
				mg -= 29;
			}
			else if ((d.mese==2)&&(d.anno%4!=0))
				{
					if (mg<=28) break;
					mg -= 28;
				}
				else
				{
					if (mg<=31) break;
					mg -= 31;
				}
	}

	d.giorno = mg;
	return(d);

}




// restituisce la path corrente a partire dalla radice
char *pwd_ext2(){
    if (mount_ext2)
        return(path_ext2);
     else return('\0');
}


// restituisce l'elenco dei file della directory corrente
void ls_ext2(){
    int i;
    struct i_node* info_file;
    if (mount_ext2){

	clrscr();
	//kprintf("\n\r Total: %u",count_dir);
	for (i=0; i<count_dir; i++)
	{

		info_file = get_inode(inode_dir[i]);
		// data e ora di creazione del file
		tempo = det_time(info_file->i_ctime);
		data = det_date(info_file->i_ctime);

		// TODO : fast symbolic link
		if ((info_file->i_mode&MODE_MASK)==MODE_LINK)
			continue;
		switch (info_file->i_mode & MODE_MASK)
		{

		case MODE_FILE:
		set_color(WHITE);
		break;

		case MODE_DIR:
		set_color(LIGHT_BLUE);
		break;

		case MODE_CDEV:
		set_color(LIGHT_GREEN);
		break;

		case MODE_BDEV:
		set_color(LIGHT_RED);
		break;

		case MODE_FIFO:
		set_color(LIGHT_MAGENTA);
		break;

		case MODE_SOCK:
		set_color(LIGHT_CYAN);
		break;

		case MODE_LINK:
		set_color(RED);
		break;

		}

		kprintf("\n\r %s",ext2_file_name[i]);
		set_color(DEFAULT_COLOR);
		kprintf ("\r			(%u)",inode_dir[i]);
		set_color(WHITE);
		kprintf("\r					%u",info_file->i_size);
		kprintf("\r						%u:%u",tempo.ora,tempo.minuti);
		kprintf("\r							 %u-%u-%u",data.anno,data.mese,data.giorno);
		kprintf("\r									%u",info_file->i_blocks);
		set_color(DEFAULT_COLOR);
                if (((i+1)%22)==0)
		{
			kprintf ("\n\r ------ Continue ------");
			if( kgetchar() == CTRL_C )
			{
				kprintf("\n\n\r");
				return;
			}
			clrscr();
                }
        }
	kprintf("\n\n");
    }
    else kprintf("\n\rUnmounted File System Ext2\n\n\r");
    return;
}



// permette di cambiare la directory corrente
void cd_ext2(char *param){
	struct i_node* ino;
	int i,j_param,len_param,i_param;
	dword ino_dir;
	char elem_path[256]; // si usa per il parsing della path
	bool errore;

	//kprintf("\n\rparam %s",param);
	if(!mount_ext2)
	{
		kprintf("\n\rUnmounted File System Ext2\n\n\r");
		return;
	}

	if (!strcmp(param,"."))
	{
		// si rimane nella directory corrente
		return;
	}

	if (!strcmp(param,"/"))
	{
		// si ritorna alla directory radice
		if (!Open_Dir(get_inode(EXT2_ROOT_INO)))
		{
			// se non si può più aprire la root si smonta il file system
			mount_ext2 = FALSE;
			return;
		}
		level = 0;
		path_ext2[0]='\0';
		return;
	}


	if ((level == 0)&&!strcmp(param,".."))
	{
		kprintf("\n\rCannot go up from root\n\n\r");
	}
	else if ((level ==1)&&!strcmp(param,".."))
		{
			// si ritorna alla directory radice
			if (!Open_Dir(get_inode(EXT2_ROOT_INO)))
			{
				// se non si può più aprire la root si smonta il file system
				mount_ext2 = FALSE;
				return;
			}
			level = 0;
			path_ext2[0]='\0';
			ino_current_dir = EXT2_ROOT_INO;
		}
		else
		{

			errore = FALSE;
			// dobbiamo effettuare il salvataggio dei componenti
			memcpy(&path_ext2_backup,&path_ext2,1024);
			//kprintf ("\n\rpath_ext2_backup : %s\n\r",path_ext2_backup);
			level_backup = level;
			ino_dir_backup = ino_current_dir;

			// dobbiamo fare il parsing della path
			i_param = 0;
			if (param[0]=='.'&&param[1]=='/')
			{
				// ./ sta ad indicare restiamo nella dir corrente
				// non ha senso riaprirla
				i_param = 2;
			}
			if (param[strlen(param)-1]=='/')
			{
				len_param = strlen(param)-1;
			}
			else len_param = strlen(param);
			//kprintf ("\n\rlen_param : %u\n\r",len_param);

			for (j_param=0;i_param<=len_param;i_param++)
				if ((param[i_param]=='/')||i_param==len_param)
				{
					elem_path[j_param]='\0';
					j_param = 0;
					//kprintf ("\n\relem_path : %s\n\r",elem_path);
					if (!(ino_dir=FindFile(elem_path)))
					{
						// non esiste nella directory corrente la sottodirectory param
						kprintf("\n\rDirectory no match\n\n\r");
						errore = TRUE;
						break;
					}
					else if (!(ino=get_inode(ino_dir)))
						{
						// invalid inode number
						errore = TRUE;
						break;
						}
						else if (!(isDir(ino)||isFastSymbolicLink(ino)))
							{
								kprintf("\n\rNot a directory\n\n\r");
								errore = TRUE;
								break;
							}
							else if (!Open_Dir(ino))
								{
									if (!isFastSymbolicLink(ino))
										kprintf("\n\rOpen failed\n\n\r");
									errore = TRUE;
									break;
								}
								else
								{
									// si aggiorna la path salendo nell'albero
									if (!strcmp(elem_path,".."))
									{

										if (level)
										{
											level--;
											for (i=strlen(path_ext2)-2;path_ext2[i]!='/';i--);
											path_ext2[i+1]='\0';
										}
										if (!level)
											path_ext2[0]='\0';
									}
									else
									{
										strcat(path_ext2,elem_path);
										strcat(path_ext2,"/");
										level++;
									}
									ino_current_dir = ino_dir;

								}
				}
				else elem_path[j_param++] = param[i_param];

				if (errore)
				{
					// si ripristina la vecchia directory
					kprintf("\n\rerrore\n\r");
					level = level_backup;
					memcpy(&path_ext2,&path_ext2_backup,1024);
					ino_current_dir = ino_dir_backup;
					Open_Dir(get_inode(ino_current_dir));


				}
				kprintf("\n\r level :%u\n\r",level);

		}
}

void cat_ext2(char *param){
	struct i_node * ino;
	dword i;
	word block_file;
	char *c=NULL;
	int stop=0,ll=0;
	dword ino_file;

	if (!mount_ext2)
	{
		kprintf("\n\rUnmounted File System Ext2\n\r");
		return;
	}

	if (!(ino_file = FindFile(param)))
	{
		kprintf("\n\rNo such file\n\r");
		return;
	}
	else if (!(ino=get_inode(ino_file)))
		{
			// invalid inode number
		}
		else if (!(isFile(ino)||isFastSymbolicLink(ino)))
		{
			kprintf("\n\rNot a regular file\n\r");
			return;
		}
		else if (Open_File(ino,ino->i_mode & MODE_MASK))
			{
				// dimensione in blocchi del file
				block_file = 0;
				stop = 3;
				data_block = (byte *)kmalloc(dim_block, GFP_KERNEL);
				memset(data_block,0,dim_block);
				//clrscr();
				set_color(LIGHT_BLUE);
				kprintf("filename: %s\n\n\r",param);
				set_color(DEFAULT_COLOR);
				for (i=0;i<ino->i_size;i++)
				{
					if (i % dim_block ==0)
					{
						if ( (err = ReadSectorsLba(0,(int64_t)blocco_to_LBA(ptr_dir[block_file]),spb,(word *)data_block,TRUE)) )
						{
							kprintf ("\n\rError reading file data block");
							ShowIdeErrorMessage(err,TRUE);
        						kfree(ptr_dir);
							kfree(data_block);
							return;
						}
						block_file++;
						c = (char *)data_block;
					}


					if (*c==10) // carattere di fine linea
					{
						stop++;
						kprintf("\n\r");
						ll=0;
					}
					else
					{
						kputchar(*c);
						if(ll++>70)
						{
							// se la linea eccede in lunghezza la dimensione
							//dello schermo si divide in due linee
							stop++;
							ll = 0;
						}
					}

					c++;

					if (((stop)%22)==0)
					{
						kprintf ("\n\r ------ Continue ------");
						if( kgetchar() == CTRL_C )
						{
							// *** Andrea Righi 2003-10-04 *** //
							kfree(data_block);
							//********************************//
							kprintf("\n\r");
							return;
						}
						clrscr();
						stop = 3;
						set_color(LIGHT_BLUE);
						kprintf("filename: %s\n\n\r",param);
						set_color(DEFAULT_COLOR);
					}
				}
				kfree(data_block);
			}
}



/**************************************************************************
*
*           Procedure di inizializzazione del file system Ext2
*
***************************************************************************/


//--------------- Lettura da disco Ext2 ---------------//
bool read_ext2(){

	int dev,sec;

	/*
 	 lettura del superblocco a partire START_SUPER_BLOCK. Il super blocco
 	 si trova sempre nel primo blocco della partizione e occupa esattamente 1024 byte
 	*/

 	data_block = kmalloc(DIM_SUPER_BLOCK, GFP_KERNEL);
 	memset(data_block,0,DIM_SUPER_BLOCK);

 	dev = 0;
 	sec = 2;

	// seleziono il canale
	SelectAtaChannel(CC_PRIMARY);

	if ( (err = ReadSectorsLba(dev,(int64_t) blocco_to_LBA(0)+sec,2,(word *) data_block,TRUE)) )
	{
	 // si stampa l'errore relativo al fallimento lettura disco
	 ShowIdeErrorMessage(err,TRUE);
	 return FALSE;
 	}
 	else 	{
 		 // si copia il settore nella variabile super
		 	// *** Andrea Righi ****************************//
			//! super must be allocated first...
			super = kmalloc(sizeof(struct super_block), GFP_KERNEL);
			// *********************************************//
 		 	memcpy(super,data_block,sizeof(struct super_block));
			return TRUE;
		}
	kfree(data_block);
}


//------------- Controllo validità Ext2 ---------------//
bool check_ext2(){
	if ((super->s_magic != N_EXT2_NUMERO_MAGICO)&&(super->s_magic != P_EXT2_NUMERO_MAGICO))
	{
		return(FALSE);
	}
	else return TRUE;
}


bool init_ext2(){
	path_ext2[0] = '\0'; //inizializzazione della path

	if (!read_ext2())
	{
		printk( KERN_ERR
			"%s(): disk I/O error. Unable to read the superblock!\n",
			__FUNCTION__ );
        	return(FALSE);
	}
	// inizializzazione ext2 a buon fine

	if (!check_ext2())
	{
		printk( KERN_ERR
			"%s(): not a valid ext2 filesystem!\n",
			__FUNCTION__ );
		return(FALSE);
	}
	// ext2 é valido


	// calcolo parametri della versione corrente file system

	dim_block = 1024 << super->s_log_block_size; //dimensione dei blocchi

	dim_frag = 1024 << super->s_log_frag_size; // dimensione dei frammenti

	spb = dim_block / SIZE_SEC; //settori per blocco

	sbpos = super->s_first_data_block + 1; // posizione del superblocco

	bpg = super->s_blocks_per_group; //blocchi per gruppo

	gdpb = dim_block / sizeof(struct group_descriptor); // desc di gruppo per blocco

	ipb = dim_block / super->s_inode_size; // inodes per blocco dim inode 128 bit

	number_of_groups = super->s_inodes_count / super->s_inodes_per_group; // numero gruppi

	dir_entries_per_block = dim_block / sizeof(struct dir_ff); //directory per blocco

	dim_ptr = dim_block >> 2; // dim del blocco in parole da 32 bit

	#ifdef EXT2_DEBUG
	// informazioni di carattere generale sulla ext2 corrente
	printk( KERN_INFO "%s(): Ext2 parameters\n", __FUNCTION__ );
	printk( KERN_INFO "\tTotal number of inodes:                       %u\n", super->s_inodes_count);
	printk( KERN_INFO "\tTotal number of block:                        %u\n", super->s_blocks_count);
	printk( KERN_INFO "\tNumber of blocks reserved for the super user: %u\n", super->s_r_blocks_count);
	printk( KERN_INFO "\tNumber of groups:                             %u\n", number_of_groups);
	printk( KERN_INFO "\tBlock per group:                              %u\n", bpg);
	printk( KERN_INFO "\tBlock dimension:                              %u\n", dim_block);
	printk( KERN_INFO "\tFragment dimension:                           %u\n", dim_frag);
	printk( KERN_INFO "\tSector per block:                             %u\n", spb);
	printk( KERN_INFO "\tDirectories per block:                        %u\n", dir_entries_per_block);
	printk( KERN_INFO "\tGroup descriptors per block:                  %u\n", gdpb);
	printk( KERN_INFO "\tInodes per group:                             %u\n", super->s_inodes_per_group);
	printk( KERN_INFO "\tFrags per group:                              %u\n", super->s_frags_per_group);
	printk( KERN_INFO "\tInodes per block:                             %u\n", ipb);
	#endif

	// inizializzazione tabella dei descrittori di gruppo
	if (!init_group_desc_table())
	{
		printk( KERN_ERR
			"%s(): cannot initialize group descriptor table!\n",
			__FUNCTION__ );
		return(FALSE);
	}

	// inizializzazione tabella degli inode
	if (!init_inode_table())
	{
		printk( KERN_ERR
			"%s(): cannot initialize inode table!\n",
			__FUNCTION__ );
		return(FALSE);
	}

	// dobbiamo leggere l'inode relativo alla directory radice "inode 2 sempre"
	if (!Open_Dir(get_inode(EXT2_ROOT_INO)))
	{
		printk( KERN_ERR
			"%s(): not able to open the root directory!\n",
			__FUNCTION__ );
		return(FALSE);
	}

	ino_current_dir = EXT2_ROOT_INO;

	mount_ext2 = TRUE; //file system montato

	level = 0; // livello nell'albero delle directory, root = 0

	printk( KERN_INFO "%s(): ext2 correctly mounted.\n", __FUNCTION__ );

	return(TRUE);
}
