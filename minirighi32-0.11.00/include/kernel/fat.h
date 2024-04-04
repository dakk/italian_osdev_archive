/*!	\file include/kernel/fat.h
 *	\brief FAT-12 filesystem header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-07\n
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#ifndef FAT12_H
#define FAT12_H

/** \ingroup FileSystem
 *  \defgroup FSFAT12 FAT-12
 *  The FAT-12 file system.
 *  @{
 */

//! Maximum length for the path string.
#define PATH_LEN		1024

//! The root directory cluster.
#define FAT12_ROOT_DIR		0

//! Physical size of the fat in clusters.
#define FAT_PHYS_SIZE		9

//! The boot sector.
#define FAT_BOOT_SECTOR		0

//! The cluster size.
#define FAT_SECTOR_SIZE		512

//! End of file (fat12).
#define EOF_FAT12 		0xFF8

//! End of file (fat16) not yet implemented!
#define EOF_FAT16 		0xFFF8

//! End of file (fat32) not yet implemented!
#define EOF_FAT32 		0xFFFFFF8

//! The fat12 super block (boot sector).
typedef struct fat12_super_block
{
	byte Jump[ 3 ];
	unsigned char Name[ 8 ];
	word BytesPerSector;
	byte SectorsPerCluster;
	word ReservedSectors;
	byte Fats;
	word RootDirectoryEntries;
	word LogicalSectors;
	byte MediumDescriptorByte;
	word SectorsPerFat;
	word SectorsPerTrack;
	word Heads;
	word HiddenSectors;
	byte code[ 482 ];
} __attribute__ ((packed)) fat12_super_block_t;

//! Physical FAT structure (into the disk).
typedef struct FAT12
{
	byte data[ FAT_SECTOR_SIZE * FAT_PHYS_SIZE ];
}  __attribute__ ((packed)) FAT12_t;

//! Logical transposition of the FAT structure.
typedef struct logical_FAT12
{
	word data[ 3072 ];
} __attribute__ ((packed)) logical_FAT12_t;

//! Attribute type.
typedef struct fat12_file_attr
{
	unsigned long read_only	: 1;
	unsigned long hidden	: 1;
	unsigned long system	: 1;
	unsigned long label	: 1;
	unsigned long directory	: 1;
	unsigned long archive	: 1;
	unsigned long __res	: 2;
} __attribute__ ((packed)) fat12_file_attr_t;

//! File entry type.
typedef struct FileEntry
{
	unsigned char Name[8];
	unsigned char Extension[3];
	fat12_file_attr_t Attribute;
	byte Reserved[10];
	word Time;
	word Date;
	word StartCluster;
	dword FileLength;
} __attribute__ ((packed)) FileEntry_t;

//! Date type.
typedef struct fat_date
{
	int year;
	int month;
	int day;
} __attribute__ ((packed)) fat_date_t;

//! Time type.
typedef struct fat_time
{
	int hour;
	int minute;
	int second;
} __attribute__ ((packed)) fat_time_t;

//! Sector Directory type.
typedef struct SectorDir
{
	FileEntry_t Entry[ FAT_SECTOR_SIZE / sizeof(FileEntry_t) ];
} __attribute__ ((packed)) SectorDir_t;

//! Iterate over a directory.
//! \param i The index to iterate.
//! \param c The clusters to read.
#define foreach_fat12_dir_entry( i, c ) \
	for( i = 0; i < (c) * fat12_super.BytesPerSector / sizeof(FileEntry_t); i++ )

// --- Prototypes ----------------------------------------------------- //

int fat12_locate_file( FileEntry_t *f, int dir, char *path );
int fat12_file_size( int dir, char *path );
int fat12_ls( int dir, char *path );
int fat12_cd( int dir, const char *path );
int fat12_load_file( uint8_t *buf, size_t size, int dir, char *filename );
int fat12_rm( int dir, char *filename );
int fat12_sync( void );
int fat12_mount( void );
int fat12_umount( void );

/** @} */ // end of FSFAT12

#endif
