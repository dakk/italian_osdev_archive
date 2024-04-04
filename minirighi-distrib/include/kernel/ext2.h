/*!	\file include/kernel/ext2.h
 *	\brief ext2 file system header.
 *	\author
 *		Filippo Brogi
 *	\note Copyright (&copy;) 2003
 *		Filippo Brogi
 *	\date Last update:
 *		2003-09-30 by Andrea Righi
 *			init_ext2() now returns a boolean value.\n
 */

#ifndef EXT2_H
#define EXT2_H

#include <const.h>

/** \ingroup FileSystem
 *  \defgroup FSext2 Ext2
 *  The ext2 file system.
 *  @{
 */

// dimensione blocco del disco
#define SIZE_SEC 512

// numero magico ext2
#define N_EXT2_NUMERO_MAGICO 0xEF53 //for normal versione of ext2
#define P_EXT2_NUMERO_MAGICO 0xEF51 //for versions of ext2fs prior to 0.2b.

// costanti errore ext2
#define EXT2_ERRORS_CONTINUE 1 // continua come se niente fosse
#define EXT2_ERRORS_RO 2 // rimonta a sola lettura
#define EXT3_ERRORS_PANIC 3 // causa un panic nel kernel

// valori EXT2_OS
#define EXT2_OS_LINUX 0
#define EXT2_OS_HURD 1
#define EXT2_MASIX 2
#define EXT2_FREEBSD 3
#define EXT2_OS_LITES4 4

// livelli di revisione
#define EXT2_GOOD_OLD_REV 0 //original format
#define EXT2DYNAMIC_REV 1 // formato v2 con dimensione inode dinamica

// valori EXT2_*_INO
#define EXT2_BAD_INO 0x01 //blocco inode danneggiato
#define EXT2_ROOT_INO 0x02 // inode directory radice
#define EXT2_ACL_IDX_INO 0x03 //ACL index inode
#define EXT2_ACL_DARA_INO 0x04 //ACL data inode
#define EXT2_BOOTLOADER INO 0x05 //boot loader inode
#define EXT2_UNDEL_DIR_INO 0x06 // inode directory ripristinata

// valori EXT2_S_I

// ------------ file format ------------
#define MODE_MASK 0xF000 // format mask
#define MODE_SOCK 0xC000 // socket
#define MODE_LINK 0xA000 // symbolic link
#define MODE_FILE 0x8000 // regular file
#define MODE_BDEV 0x6000 // block device
#define MODE_DIR 0x4000 // directory
#define MODE_CDEV 0x2000 // character device
#define MODE_FIFO 0x1000 // fifo

// ------------ access rights ------------
#define EXT2_S_ISUID 0x0800 // SUID
#define EXT2_S_ISGID 0x0400 // SGID
#define EXT2_S_ISVTX 0x0200 // sticky bit
#define EXT2_S_IRWXU 0x01C0 // user access rights mask
#define EXT2_S_IRUSR 0x0100 // read
#define EXT2_S_IWUSR 0x0080 // write
#define EXT2_S_IXUSR 0x0040 // execute
#define EXT2_S_IRWXG 0x0038 // group access right mask
#define EXT2_S_IRGRP 0x0020 // read
#define EXT2_S_IWGRP 0x0010 // write
#define EXT2_S_IXGRP 0x0008 // execute
#define EXT2_S_IRWXO 0x0007 // others access rights mask
#define EXT2_S_IROTH 0x0004 // read
#define EXT2_S_IWOTH 0x0003 // write
#define EXT2_S_IXOTH 0x0001 // execute


// valori EXT2_FT
//------------ tipo file ---------------
#define EXT2_FT_UNKNOWN 0
#define EXT2_FT_REG_FILE 1
#define EXT2_FT_DIR 2
#define EXT2_FT_CHRDEV 3
#define EXT2_FT_BLKDEV 4
#define EXT2_FT_FIFO 5
#define EXT2_FT_SOCK 6
#define EXT2_FT_SYMLINK 7
#define EXT2_FT_MAX 8


#define DIM_SUPER_BLOCK 1024
#define START_SUPER_BLOCK 1024
struct super_block{
 dword s_inodes_count; //numero totale inode liberi e utilizzati
 dword s_blocks_count; //numero totale di blocchi liberi e utilizzati
 dword s_r_blocks_count; //numero totale di blocchi riservati al super user
 dword s_free_blocks_count; //numero di blocchi liberi compresi quelli riservati
 dword s_free_inodes_count; //numero totale di inode liberi
 dword s_first_data_block; /* id del blocco contenente la struttura del sb
 generalmente questo valore é 0 per i file system con con una dimensione del
 blocco maggiore di 1KB. Il super blocco inzia sempre al 1024 byte del disco
 che generalmente coincide con il primo byte del terzo settore */
 dword s_log_block_size; /* la dimensione del blocco si calcola come il numero
 di bit che si ottiene shiftando 1024. Questo valore può essere solo positivo;
    block size = 1024 << s_log_block_size; */
 dword s_log_frag_size; /* la dimensione del frammento si calcola come il numero
 di bit da shiftare dal valore 1024
    if (positive)
     fragment size = 1024 << s_log_frag_size;
    else
     fragment size = 1024 >> -s_log_frag_size; */
 dword s_blocks_per_group; //numero totale blocchi del gruppo
 dword s_frags_per_group; //numero totale frammenti per gruppo
 dword s_inodes_per_group; //numero totale di inodes per gruppo
 dword s_mtime; // data ultimo montaggio del file system
 dword s_wtime; // ultimo accesso in scrittura al file system
 word s_mnt_count; // numero di volte che é stato montato dall'ultima volta
 // in cui é stata completamente verificata
 word s_max_mnt_count; //numero massimo di volte che un file system può essere
 //montato prima che sia eseguito un check completo
 word s_magic; // numero magico
 word s_state; /* stato del file system montato. Quando il fs é stato montato
 lo stato é posto a EXT2_ERROR_FS. Quando il file system non é stato ancora
 montato il valore può essere EXT2_VALID_FS oppure EXT2_ERROR_FS se non é stato
 completamente smontato */
 word s_errors; //cosa fare in caso di errore
 word s_minor_rev_level; //
 dword s_lastcheck; // ultimo check del file system
 dword s_checkinterval; // massimo tempo di intervallamento tra due check
 dword s_creator_os; // identificativo so creatore del file system
 dword s_rev_level; // valore livello di revisione
 word s_def_resuid; // id user default per i blocchi riservati
 word s_def_resgid; // id group default per i blocchi riservati

 dword s_first_ino; /* indice del primo inode utilizzabile per file standard.
 nella revisione non dinamica del file system questo valore é fissato a 11. Con
 quella dinamica é possibile modificare questo valore*/
 word s_inode_size; /* dimensione  della struttura inode. Nel caso non
 dinamico questo valore é 128*/
 word s_block_group_nr; // numero dei gruppi nel superblocco
 dword s_feature_compact; //
 dword s_feature_incompact; //
 dword s_feature_ro_compact; //
 byte s_uuid[16]; // id del volume 128 bit
 word s_volume_name; //
 byte s_last_mounted[8]; // path directory dove é stato montato il fs
 dword s_algo_bitmap; /* usato da algoritmi di compressione per determinare i
 metodi utilizzati */
 byte s_reserved[886]; // riservati al sistema operativo
};

/* Il descrittore di gruppo é un array della struttura group_desc ognuna delle
quali definisce uno dei gruppi di blocchi dando la locazione della tabella 
degli inode, bitmap dei blocchi e inode, ed altre informazioni ancora. In 
generale é allocato consecutivamente al blocco del disco contenente il super
blocco*/


struct group_descriptor{
 dword bg_block_bitmap; // id primo blocco della bitmap dei blocchi del gruppo
 dword bg_inode_bitmap; // id primo blocco della bitmap degli inode
 dword bg_inode_table; //primo blocco della tabella degli inode
 word bg_free_blocks_count; //numero totale di blocchi liberi
 word bg_free_inodes_count; // numero totale inode liberi
 word bg_used_dirs_count; // numero inode allocati nelle directory
 word bg_pad; // valore usato per il padding della struttura 
 dword bg_reserved[3]; // valori riservati per future implementazioni
};


/* 
Il "block bitmap" é normalmente locato al primo blocco o al secondo blocco
nel caso in cui si abbia  il backup del super blocco. La sua locazione ufficiale
si può ottenere leggendo il bg_block_bitmap nel descrittore di gruppo. Ogni
bit rappresenta lo stato corrente del blocco, 1 indica usato, mentre 0 indica
libero/disponibile. 

L' "inode bitmap" lavora nello stesso modo della bitmap dei blocchi. Tale 
bitmap si determina a partire da bg_inode_bitmap. Quando si crea la tabella 
degli inode tutti gli inode riservati sono marcati come utilizzati. 

La tabella degli inode é usata per tenere traccia di ogni file : locazione,
dimensione, tipo, diritti di accesso sono tutti memorizzati negli inode. I nomi
dei file non sono memorizzati qui, nella tabella degli inode tutti i file sono 
referenziati con il loro numero di inode. La tabella degli inode é referenziata
da bg_inode_table e contiene s_inodes_per_group 
*/



struct i_node{
 word i_mode; // formato del file e diritti di accesso
 word i_uid; // user id associato col file
 dword i_size; // dimensione in byte del file
 dword i_atime; // ultimo accesso in secondi a partire dal 1/1/1970
 dword i_ctime; // data di creazione in secondi dal 1/1/1970
 dword i_mtime; // data ultima modifica in secondi dal 1/1/1970
 dword i_dtime; // data della cancellazione del file a partire dal 1/1/1970
 word i_gid; // gruppo che ha accesso al file
 word i_links_count; // numero dei riferimenti all'inode
 dword i_blocks; /* ammontare dei blocchi associati al file quelli correntemente 
 usati e quelli che verranno utilizzati  nel caso di una  crescita delle 
 dimensioni del file. In qusto caso la dimensione  dei blocchi é 512 kB e non
 quella specificata nel super blocco */
 dword i_flags; // comportamento del file system quando accede ai dati
 dword i_osd1; // valore dipendendente dal SO
 dword i_block[15]; /* array utilizzato per individuare i blocchi del disco in 
 cui si trova memorizzato il file. I primi 12 elementi servono per indirizzare
 direttamente i blocchi dati associati ai file, il 13-esimo é utilizzato per 
 l'indirizzamento indiriretto singolo il 14-esimo per l'indirizzamento indiretto
 doppio e il 15-esimo per quello triplo */
 dword i_generation; // indica versione del file (usato da NFS)
 dword i_file_acl; //numero el blocco contenenti gli attributi estesi
 dword i_dir_acl; // indica la high size del file
 dword i_faddr; // locazione dell'ultimo frammento del file
 /* inode osd2 structure Linux */
 byte l_i_frag; //numero frammento
 byte l_i_fsize; // dimensione frammento
 word reserved1; // reserved
 word l_i_uid_high; // bit dell'user id
 word l_i_gid_high; // bit del group id
 dword reserved2; // reserved
};

/*
Le directory sono memorizzate come file e si possono identificare guardando il
valore dell'attributo i_mode dell'inode e verificando che sia uguale a
EXT2_S_IFDIR. La directory radice é sempre memorizzata nella seconda posizione
della tabella degli inode. Ogni sottodirectory si può ottenere guardando il
contenuto della direcotry radice.
*/

struct dir_ff{
 dword inode; // numero entry del file. 0 indica che non é usata
 word rec_len; // spostamento all'elemento successivo della directory corrente
 byte name_len; // quanti caratteri contiene il nome
 byte file_type; // tipo del file
 char name[1]; // nome dell'entry
};

/* Utilizzare il formato standard delle liste linkate per le directory può
divenire molto lento quando il numero dei file comincia a crescere. Così
per incrementre le prestazioni si utilizza un indice hash che aiuta a
incrementare le prestazioni di ricerca */

/* STRUTTURA Dell'INDICE
La radice dell'albero indice si trova nel blocco 0 del file. Spazio per il
secondo livello dell'albero indiciato (per file system con blocchi da 4kB)
si trova nei blocchi da 1 a 511. I blocchi foglia delle direcotry sono
situati a partire dal blocco 512, così la coda del file delle directory
assomiglia ad una direcotry standard EXT2 e può essere processata con
ext2_readdir. Per direcotry con meno di 90K di file c'é un buco eseguendo dal
blocco 1 al blocco 511, così una directory vuota ha giusto due blocchi, sebbene
la sua dimensione sia all'incirca 2 mega nella lista della direcotry.
*/

// --- Prototypes ----------------------------------------------------- //

bool init_ext2();
char *pwd_ext2();
void ls_ext2();
void cd_ext2(char *param);
void cat_ext2(char *stringa);

/** @} */ // end of FSext2

#endif
