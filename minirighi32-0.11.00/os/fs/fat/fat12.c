/*!	\file fs/fat/fat12.c
 *	\brief FAT-12 filesystem.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-08-19 Andrea Righi:
 *			Fixed mutual exclusion problem.\n
 *		2004-04-24 Andrea Righi:
 *			Added sync, mount, umount routines.\n
 *			Removed the fat12_cat() routine; we can
 *			use fat12_load_file() to implement the cat
 *			command.\n
 *		2004-04-17 Andrea Righi:
 *			Added fat12_invalidate_mount() when an I/O
 *			error occurs during read/write operations
 *			from the mounted device.\n
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#include <const.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/floppy.h>
#include <kernel/keyboard.h>
#include <kernel/kmalloc.h>
#include <kernel/semaphore.h>
#include <kernel/video.h>

#include <kernel/fat.h>

//! The boot sector.
fat12_super_block_t fat12_super;

//! The physical structure of the fat.
FAT12_t fat;

//! The logical structure of the fat.
logical_FAT12_t lfat;

//! Dirty flag to report if the logical structure has been modified.
int fat12_dirty_flag = 0;

//! The starting cluster of the root directory.
size_t dir_start;

//! The starting cluster of the data area.
size_t data_start;

//! A semaphore for fat12 operations mutual exclusion.
DECLARE_MUTEX( fat12_mutex );

//! Check for a valid fat12 floppy filesystem.
static __INLINE__ int fat12_check( void )
{
	int ret = 0;

	// Check the bootsector.
	if ( (fat12_super.BytesPerSector != FAT_SECTOR_SIZE) ||
		(fat12_super.SectorsPerCluster != 1) ||
		(fat12_super.Fats !=2 ) ||
		(fat12_super.RootDirectoryEntries != 224) ||
		(fat12_super.LogicalSectors != 2880) ||
		(fat12_super.MediumDescriptorByte != 0xF0) )
	{
		// Not a valid fat12 filesystem.
		 ret =  -EINVAL;
	}

	return( ret );
}

//! Invalidate the fat12 super block and umount the filesystem.
static __INLINE__ void fat12_invalidate_mount()
{
	// Clear the bootsector info.
	memset( &fat12_super, 0, sizeof(fat12_super_block_t) );

	// Clear the dirty flag.
	fat12_dirty_flag = 0;
}

//! Get the next sector.
static __INLINE__ bool next_sector(word *next, word actual)
{
	*next = lfat.data[ actual ];
	if( (*next == 0) || (*next >= 0x0FF0) )
		return( FALSE );
	else
		return( TRUE );
}

//! How many clusters to the end of file.
static __INLINE__ int how_many_cluster( word start )
{
	word c = 1, cl;

	// The root directory (start=0) has a fixed size.
	if( !start )
	{
		return( (fat12_super.RootDirectoryEntries * sizeof(FileEntry_t))
			/ fat12_super.BytesPerSector );
	}

	// Calculate the size of the directory... it's not the root!
	cl = start;
	while( next_sector(&cl, cl) )
		c++;

	return( c );
}

//! Find the sector n from actual.
static __INLINE__ word find_sector( int n, word actual )
{
	word temp;

	temp = actual;
	for( ; n--; )
		if( next_sector(&temp, temp) == FALSE )
			break;
	return( temp );
}

//! Integer to date conversion.
static __INLINE__ void int_to_date(fat_date_t *d, word date)
{
	d->year  = date/512;
	d->month = (date - (d->year * 512)) / 32;
	d->day   = date - (d->year * 512) - (d->month * 32);
}

//! Integer to time conversion.
static __INLINE__ void int_to_time(fat_time_t *time, word t)
{
	time->hour   = t / 2048;
	time->minute = (t - (time->hour * 2048)) / 32;
	time->second = (t - (time->hour * 2048) - (time->minute * 32)) * 2;
}

//! Generate a file name from the file entry structure.
static char *fat12_construct_file_name( char *name, int size, FileEntry_t *f )
{
	int i, k;

	// Null the name.
	memset( name, '\0', size );

	// Construct the name.
	for( i = 0; i < 8; i++ )
	{
		if( f->Name[ i ] == ' ' )
		{
			name[ i ] = '\0';
			break;
		}
		else
		{
			name[ i ] = tolower( f->Name[ i ] );
		}
	}
	// Construct the extension.
	if( f->Extension[ 0 ] != ' ' )
	{
		strncat( name, ".", size );
		k = strnlen( name, size );
		for( i = 0; i < 3; i++ )
		{
			if( f->Extension[ i ] == ' ' )
			{
				name[ i + k ] = '\0';
				break;
			}
			else
			{
				name[ i + k ] = tolower( f->Extension[ i ] );
			}
		}
	}
	return( name );
}

//! Read some cluster.
//! \return The number of bytes read.
static int fat12_read_clusters( char *buf, size_t size, uint16_t cstart, uint16_t cnum )
{
	int n = MIN( how_many_cluster( cstart ), cnum );
	int ret;
	uint8_t *tmp_buf;

	if( (!n) || (!size) )
		return( 0 );

	// Allocate the buffer.
	tmp_buf = kcalloc( n, fat12_super.BytesPerSector, GFP_KERNEL );
	if( tmp_buf == NULL )
	{
		printk( KERN_ERR "%s(): out of memory!\n",
			__FUNCTION__ );
		return( -ENOMEM );
	}
	size = MIN( size, n * fat12_super.BytesPerSector );

	// The fat12 root directory is a special cluster.
	if( cstart == FAT12_ROOT_DIR )
	{
		// Read the root cluster.
		ret = fdc_read( dir_start, tmp_buf, n );
		if( ret < 0 )
		{
			// Invalidate the filesystem.
			fat12_invalidate_mount();
			goto out;
		}
		else
		{
			// Copy the temporary buffer into the buffer.
			memcpy( buf, tmp_buf, size );
		}
	}
	else
	{
		int i = 0;
		word clust = (word)cstart;

		// Read n clusters.
		while( i < n )
		{
			ret = fdc_read( data_start + clust,
				tmp_buf + i * fat12_super.BytesPerSector, 1 );
			if( ret < 0 )
			{
				// Invalidate the filesystem.
				fat12_invalidate_mount();
				goto out;
			}
			i++;
			// Get the next sector.
			if( next_sector(&clust, clust) == FALSE )
			{
				break;
			}
		}
		// Copy the temporary buffer into the buffer.
		memcpy( buf, tmp_buf, size );
	}

	// Read operation ok.
	ret = size;
out:
	// Free the temporary buffer.
	kfree( tmp_buf );
	return( ret );
}

//! Write some cluster.
//! \return The number of bytes written or the error code.
static int fat12_write_clusters( char *buf, size_t size, uint16_t cstart, uint16_t cnum )
{
	int n = MIN( how_many_cluster( cstart ), cnum );
	int ret;
	uint8_t *tmp_buf;

	if( (!n) || (!size) )
		return( 0 );

	// Allocate the buffer.
	tmp_buf = kcalloc( n, fat12_super.BytesPerSector, GFP_KERNEL );
	if( tmp_buf == NULL )
	{
		printk( KERN_ERR "%s(): out of memory!\n",
			__FUNCTION__ );
		return( -ENOMEM );
	}
	size = MIN( size, n * fat12_super.BytesPerSector );

	// Copy the bytes to write into the temporary buffer.
	memcpy( tmp_buf, buf, size );

	// The fat12 root directory is a special cluster.
	if( cstart == FAT12_ROOT_DIR )
	{
		// Write the root cluster.
		ret = fdc_write( dir_start, tmp_buf, n );
		if( ret < 0 )
		{
			// Invalidate the filesystem.
			fat12_invalidate_mount();
			goto out;
		}
	}
	else
	{
		int i = 0;
		word clust = (word)cstart;

		// Write n clusters.
		while( i < n )
		{
			ret = fdc_write( data_start + clust,
				tmp_buf + i * fat12_super.BytesPerSector, 1 );
			if( ret < 0 )
			{
				// Invalidate the filesystem.
				fat12_invalidate_mount();
				goto out;
			}

			// Next cluster.
			i++;

			// Get the next sector.
			if( next_sector(&clust, clust) == FALSE )
			{
				break;
			}
		}
	}

	// Write operation ok.
	ret = size;
out:
	// Free the temporary buffer.
	kfree( tmp_buf );
	return( ret );
}

//! Get a file entry in the root directory.
static int fat12_get_root_dir_entry( FileEntry_t *f, char *filename )
{
	char name[ 13 ];
	int n = how_many_cluster( FAT12_ROOT_DIR );
	union
	{
		char *byte;
		SectorDir_t *dir;
	} buf;
	int i, ret = -ENOENT;

	if( *filename == '\0' )
		return( ret );

	// Special entries.
	if( (strcmp(filename, ".") == 0) || (strcmp(filename, "..") == 0) )
	{
		memset( f, 0, sizeof(FileEntry_t) );
		memset( f->Name, ' ', sizeof(f->Name) );
		f->Name[ 0 ] = '/';
		memset( f->Extension, ' ', sizeof(f->Extension) );
		f->Attribute.directory = 1;
		f->Attribute.system = 1;
		f->StartCluster = FAT12_ROOT_DIR;
		f->Time = f->Date = 0;
		return( FAT12_ROOT_DIR );
	}

	// Allocate the buffer.
	buf.byte = kcalloc( n, fat12_super.BytesPerSector, GFP_KERNEL );
	if( buf.byte == NULL )
	{
		printk( KERN_ERR "%s(): out of memory!\n",
			__FUNCTION__ );
		return( -ENOMEM );
	}

	// Read the root directory clusters.
	ret = fat12_read_clusters(
		buf.byte,
		n * fat12_super.BytesPerSector,
		FAT12_ROOT_DIR,
		n );
	if( ret < 0 )
		goto out;

	// Iterate over the root directory.
	foreach_fat12_dir_entry( i, n )
	{
		if ( buf.dir->Entry[ i ].Name[ 0 ] == 0 )
		{
			// End of entries.
			ret = -ENOENT;
			goto out;
		}
		if ( buf.dir->Entry[ i ].Name[ 0 ] == 0xe5 )
		{
			// Entry deleted.
			continue;
		}

		// Construct the name of the entry.
		fat12_construct_file_name( name, sizeof(name), &(buf.dir->Entry[ i ]) );

		// Compare the two names.
		if( strncasecmp(filename, name, sizeof(name)) == 0 )
		{
			// File located.
			memcpy( f, &(buf.dir->Entry[ i ]), sizeof(FileEntry_t) );
			ret = buf.dir->Entry[ i ].StartCluster;
			goto out;
		}
	}
out:
	kfree( buf.byte );
	return( ret );
}

//! Get a specific file entry into a directory.
static int fat12_get_dir_entry( FileEntry_t *f, int dir, char *filename )
{
	char name[ 13 ];
	int i;
	int ret = -ENOENT;
	word clust = dir;
	union
	{
		char byte[ FAT_SECTOR_SIZE ];
		SectorDir_t dir;
	} buf;

	if( *filename == '\0' )
		return( ret );

	// Allocate a buffer for the current directory entries.
	clust = dir;
	do
	{
		// Read the next cluster of the directory.
		ret = fat12_read_clusters( buf.byte, sizeof(buf.byte), clust, 1 );
		if( ret < 0 )
			goto out;

		// Iterate over the directory.
		foreach_fat12_dir_entry( i, 1 )
		{
			if ( buf.dir.Entry[ i ].Name[ 0 ] == 0 )
			{
				// End of entries.
				ret = -ENOENT;
				goto out;
			}

			if ( buf.dir.Entry[ i ].Name[ 0 ] == 0xe5 )
			{
				// Entry deleted.
				continue;
			}

			// Construct the name of the entry.
			fat12_construct_file_name( name, sizeof(name), &(buf.dir.Entry[ i ]) );

			// Compare the two names.
			if( strncasecmp(filename, name, sizeof(name)) == 0 )
			{
				// File located.
				memcpy( f, &(buf.dir.Entry[ i ]), sizeof(FileEntry_t) );
				ret = buf.dir.Entry[ i ].StartCluster;
				goto out;
			}
		}
	} while( next_sector(&clust, clust) == TRUE );
out:
	return( ret );
}

//! Get a specific file entry into a generic directory.
static int fat12_get_file_entry( FileEntry_t *f, int dir, char *filename )
{
	int ret;
	if( dir == FAT12_ROOT_DIR )
		ret = fat12_get_root_dir_entry( f, filename );
	else
		ret = fat12_get_dir_entry( f, dir, filename );
	return( ret );
}

//! Set a new path from the current directory.
static int fat12_set_relative_path( int dir, char *new_path )
{
	FileEntry_t f;
	int clust;

	if( *new_path == '\0' )
		return( dir );

	clust = fat12_get_file_entry( &f, dir, new_path );
	if( f.Attribute.directory )
	{
		return( clust );
	}
	return( -ENOENT );
}

//! Set a path from the directory dir.
//! \return The new directory cluster if exists or -ENOENT otherwise.
static int fat12_set_path( int dir, const char *buf )
{
	char *str = kmalloc( PATH_LEN, GFP_KERNEL );
	char *tmp = str;
	char *path;

	// Save the path into a buffer.
	strncpy( tmp, buf, PATH_LEN );

	if( tmp[ 0 ] == '/' )
	{
		// Absolute path.
		dir = 0;
		while( *tmp == '/' )
			tmp++;
		if( *tmp == '\0' )
			goto out;
	}

	// Find the directory.
	do
	{
		path = strsep( &tmp, "/" );
		if( path == NULL )
			break;
		dir = fat12_set_relative_path( dir, path );
	} while( dir >= 0 );
out:
	kfree( str );
	return( dir );
}

//! Locate a file getting the starting cluster.
static int __fat12_locate_file( FileEntry_t *fe, int dir, char *path )
{
	char str_path[ 1024 ];
	char *tmp = str_path, *filename;
	int ret;

	ret = fat12_check();
	if( ret < 0 )
	{
		return( ret );
	}
	ret = -ENOENT;

	// Copy the path string into a temporary area.
	strncpy( str_path, path, sizeof(str_path) );

	if( *tmp == '/' )
	{
		ret = dir = FAT12_ROOT_DIR;
	}

explore:
	do
	{
		filename = strsep( &tmp, "/" );
		if( filename == NULL )
			goto out;
	} while( *filename == '\0' );

	ret = fat12_get_file_entry( fe, dir, filename );
	if( ret < 0 )
		goto out;

	// Is this a directory?
	if( fe->Attribute.directory )
	{
		dir = fat12_set_relative_path( dir, filename );
		goto explore;
	}
out:
	if( fe->Attribute.directory )
	{
		// We have to fix the file length.
		fe->FileLength = how_many_cluster( ret ) * fat12_super.BytesPerSector;
	}

	return( ret );
}

// --- Print informations --------------------------------------------- //

//! Print the attribute of a file.
static __INLINE__ void fat12_show_file_attrib( fat12_file_attr_t attr )
{
	if( attr.label )
	{
		kprintf( "<label> " );
		return;
	}
	kprintf( "%c", attr.directory	? 'd' : '-' );
	kprintf( "%c", attr.read_only	? 'r' : '-' );
	kprintf( "%c", attr.hidden	? 'h' : '-' );
	kprintf( "%c", attr.system	? 's' : '-' );
	kprintf( "%c", attr.archive	? 'a' : '-' );
}

//! Print a fat12 file entry.
static bool fat12_show_file_entry( FileEntry_t *f )
{
	fat_date_t FileDate;
	fat_time_t FileTime;
	char name[ 13 ];

	// Do not print the disk label.
	if( !(f->Attribute.label) )
	{
		int_to_date( &FileDate, f->Date );
		int_to_time( &FileTime, f->Time );

		// Print attribute informations.
		fat12_show_file_attrib( f->Attribute );

		// Print the file date and time.
		kprintf( "  %04d/%02d/%02u", (FileDate.year+1980), FileDate.month, FileDate.day );
		kprintf( "  %02d:%02d:%02d", FileTime.hour, FileTime.minute, FileTime.second );

		// Print the file size.
		if( f->Attribute.directory )
		{
			kprintf( "  %10d",
				how_many_cluster( f->StartCluster ) *
				fat12_super.BytesPerSector );
		}
		else
		{
			kprintf( "  %10d", (f->FileLength));
		}

		// Print the filename.
		if( f->Attribute.directory )
			set_color( LIGHT_BLUE );
		if( f->Attribute.hidden )
			set_color( GREY );
		if( f->Attribute.system )
			set_color( YELLOW );
		kprintf( "  %s\n",
			fat12_construct_file_name( name, sizeof(name), f ) );
		set_color( DEFAULT_COLOR );

		return( TRUE );
	}
	return( FALSE );
}

//! Print the files into a directory.
//! \return The number of entry shown or the error code.
static int __fat12_ls( int dir, char *path )
{
	int i, n, scroll;
	union
	{
		char *byte;
		SectorDir_t *dir;
	} buf;
	FileEntry_t f;
	int ret = fat12_check();

	if( ret < 0 )
		return( ret );

	ret = -ENOENT;

	if( *path != '\0' )
	{
		dir = __fat12_locate_file( &f, dir, path );
		if( dir < 0 )
			return( ret );
		if( !f.Attribute.directory )
		{
			fat12_show_file_entry( &f );
			return( 1 );
		}
	}

	n = how_many_cluster( dir );
	buf.byte = kcalloc( n, fat12_super.BytesPerSector, GFP_KERNEL );
	if( buf.byte == NULL )
	{
		printk( KERN_ERR "%s(): out of memory!\n",
			__FUNCTION__ );
		return( -ENOMEM );
	}

	// Read all the clusters of the directory.
	ret = fat12_read_clusters( buf.byte,
		n * fat12_super.BytesPerSector,
		dir, n );
	if( ret < 0 )
	{
		scroll = ret;
		goto out;
	}

	// Print every entry.
	scroll = 0;
	foreach_fat12_dir_entry( i, n )
	{
		if( buf.dir->Entry[ i ].Name[ 0 ] == 0xe5 )
		{
			// File deleted.
			continue;
		}

		if( buf.dir->Entry[ i ].Name[ 0 ] == 0 )
		{
			// End of file.
			break;
		}

		if( (scroll) && !(scroll % (k_get_crt_height() - 2)) )
		{
			kprintf("Press a key to continue...\n");
			scroll++;
			if( kgetchar() == CTRL_C )
				goto out;
		}
		// Show the file entry.
		if( fat12_show_file_entry(&(buf.dir->Entry[i])) == TRUE )
			scroll++;
	}
out:
	kfree( buf.byte );
	return( scroll );
}

// --- Write procedures ------------------------------------------------//

//! Write the logical FAT to the disk.
static int fat12_write()
{
	int i, j;
	int ret;

	// Converts the FAT from the logical structure into the
	// physical structure.
	for( i = 0, j = 0; j < 3072; j += 2 )
	{
		fat.data[ i++ ] = (byte)lfat.data[j];
		fat.data[ i++ ] = (byte)((lfat.data[j]>>8)&(0x0F))|((lfat.data[j+1]<<4)&(0xF0));
		fat.data[ i++ ] = (byte)(lfat.data[j+1]>>4);
	}

	// Copy the FAT to the disk.
	for( j = 0; j < fat12_super.Fats; j++ )
	{
		ret = fdc_write( 1, (uint8_t *)&fat, fat12_super.SectorsPerFat );
		if( ret < 0 )
		{
			// Invalidate the filesystem.
			fat12_invalidate_mount();
			return( ret );
		}
	}

	// Clear the dirty flag.
	fat12_dirty_flag = 0;

	// Write successful!
	return( 0 );
}

//! Delete the file from the FAT structure.
static void fat12_do_delete_file( word cluster )
{
	word next;

	while( next_sector(&next, cluster) == TRUE )
	{
		lfat.data[ cluster ] = 0;
		cluster = next;
	}
	lfat.data[ cluster ] = 0;

	// Logical fat structure has been changed: update the
	// dirty flag.
	fat12_dirty_flag = 1;
}

//! Remove a file in the current directory.
static int fat12_local_rm( int dir, char *filename )
{
	int i, n;
	char name[ 13 ];
	union
	{
		char *byte;
		SectorDir_t *dir;
	} buf;
	int ret = fat12_check();

	if( strcmp(filename, "/" ) == 0 )
		return( -EPERM );
	if( strcmp(filename, ".." ) == 0 )
		return( -EPERM );
	if( strcmp(filename, "." ) == 0 )
		return( -EPERM );

	if( ret < 0 )
		return( ret );

	n = how_many_cluster( dir );
	buf.byte = kcalloc( n, fat12_super.BytesPerSector, GFP_KERNEL );
	if( buf.byte == NULL )
	{
		printk( KERN_ERR "%s(): out of memory!\n",
			__FUNCTION__ );
		return( -ENOMEM );
	}

	// Read all the clusters of the directory.
	ret = fat12_read_clusters( buf.byte,
		n * fat12_super.BytesPerSector,
		dir, n );
	if( ret < 0 )
	{
		goto out;
	}

	// Search the filename.
	ret = -ENOENT;
	foreach_fat12_dir_entry( i, n )
	{
		if( buf.dir->Entry[ i ].Name[ 0 ] == 0xe5 )
		{
			// File deleted.
			continue;
		}

		if( buf.dir->Entry[ i ].Name[ 0 ] == 0 )
		{
			// End of file.
			break;
		}

		// Construct the name of the entry.
		fat12_construct_file_name( name, sizeof(name), &(buf.dir->Entry[ i ]) );

		// Compare the two names.
		if( strncasecmp(filename, name, sizeof(name)) == 0 )
		{
			// Delete the file.
			buf.dir->Entry[ i ].Name[ 0 ] = 0xe5;
			fat12_write_clusters( buf.byte,
				n * fat12_super.BytesPerSector,
				dir, n );
			fat12_do_delete_file(buf.dir->Entry[ i ].StartCluster);
			ret = 0;
			goto out;
		}
	}
out:
	kfree( buf.byte );
	return( ret );
}

//! Remove a file from the disk.
static int __fat12_rm( int dir, char *path )
{
	char *dir_name, *file_name;
	int ret = fat12_check();

	if( ret < 0 )
		return( ret );

	if( strcmp(path, "/" ) == 0 )
		return( -EPERM );
	if( strcmp(path, ".." ) == 0 )
		return( -EPERM );
	if( strcmp(path, "." ) == 0 )
		return( -EPERM );

	// Separate the directory and file name.
	dir_name = path;
	file_name = path + strlen( path );
	if( *file_name == '/' )
		file_name--;
	while( (*file_name != '/') && (file_name != dir_name) )
	{
		file_name--;
	}
	if( file_name == dir_name )
	{
		if( *file_name == '/' )
		{
			// Delete a root directory entry.
			ret = fat12_local_rm( FAT12_ROOT_DIR, file_name + 1 );
		}
		else
		{
			// Delete a local directory entry.
			ret = fat12_local_rm( dir, path );
		}
	}
	else
	{
		int del_dir;

		*file_name++ = '\0';

		// Locate the directory of the file to remove.
		del_dir = fat12_set_path( dir, dir_name );
		if( del_dir < 0 )
		{
			*(file_name - 1) = '/';
			return( del_dir );
		}
		// Remove the file.
		ret = fat12_local_rm( del_dir, file_name );
		*(file_name - 1) = '/';

	}
	return( ret );
}

//! Syncronize fat12 filesystem buffers.
static int __fat12_sync( void )
{
	int ret;

	ret = fat12_check();
	if( ret < 0 )
	{
		return( ret );
	}

	// Write the fat12 super block.
	if( fat12_dirty_flag == 1 )
	{
		ret = fat12_write();
		if( ret < 0 )
		{
			return( ret );
		}
	}

	return( ret );
}

// --- Fat12 Interfaces ----------------------------------------------- //

//! Get file informations (mutual exclusion guaranteed).
int fat12_locate_file( FileEntry_t *fe, int dir, char *path )
{
	int ret;

	DOWN( &fat12_mutex );

	ret = __fat12_locate_file( fe, dir, path );

	UP( &fat12_mutex );

	return( ret );
}

//! Get the size of a file in bytes.
int fat12_file_size( int dir, char *filename )
{
	FileEntry_t fe;
	int ret;

	DOWN( &fat12_mutex );

	ret = __fat12_locate_file( &fe, dir, filename );

	UP( &fat12_mutex );

	return( (ret < 0) ? ret : fe.FileLength );
}

//! Change current directory.
int fat12_cd( int dir, const char *path )
{
	int ret;

	DOWN( &fat12_mutex );

	ret = fat12_check();
	if( ret < 0 )
	{
		goto out;
	}
	if( *path == '\0' )
	{
		ret = dir;
		goto out;
	}
	// Set the path.
	ret = fat12_set_path( dir, path );
out:
	UP( &fat12_mutex );
	return( ret );
}

//! Load a file into a buffer.
int fat12_load_file( uint8_t *buf, size_t size, int dir, char *filename )
{
	int ret;
	FileEntry_t f;

	DOWN( &fat12_mutex );

	ret = __fat12_locate_file( &f, dir, filename );
	if( ret < 0 )
	{
		ret = -ENOENT;
		goto out;
	}
	else if( ret == 0 )
	{
		goto out;
	}

	// Read every cluster of the file into the buffer.
	ret = fat12_read_clusters( buf, size, ret, -1 );
out:
	UP( &fat12_mutex );
	return( ret );
}

//! Interface for file list command (mutual exclusion guaranteed).
int fat12_ls( int dir, char *path )
{
	int ret;

	DOWN( &fat12_mutex );

	ret = __fat12_ls( dir, path );

	UP( &fat12_mutex );
	return( ret );
}

//! Interface for remove command (mutual exclusion guaranteed).
int fat12_rm( int dir, char *path )
{
	int ret;

	DOWN( &fat12_mutex );

	ret = __fat12_rm( dir, path );

	UP( &fat12_mutex );
	return( ret );
}

//! Interface for the sync operation (mutual exclusion guaranteed).
//! \note sync always returs 0.
int fat12_sync( void )
{
	DOWN( &fat12_mutex );

	__fat12_sync();

	UP( &fat12_mutex );
	return( 0 );
}

//! Initialize the FAT12 filesystem from the floppy (mount).
//! \return The root directory inode or the error code.
int fat12_mount( void )
{
	int i, j;
	int ret;

	DOWN( &fat12_mutex );

	if( fat12_check() == 0 )
	{
		// Filesystem already mounted!
		ret = -EPERM;
		goto out;
	}

	// Clear the bootsector info.
	memset( &fat12_super, 0, sizeof(fat12_super_block_t) );

	// Clear the dirty flag.
	fat12_dirty_flag = 0;

	// Flush the floppy buffers to the device.
	fdc_sync();

	// Read the boot sector.
	ret = fdc_read( FAT_BOOT_SECTOR, (byte *)&fat12_super, 1 );
	if( ret < 0 )
	{
		printk( KERN_ERR
			"%s(): floppy error %i. Unable to read the block.\n",
			__FUNCTION__, ret );
		goto out;
	}

	// Check for a valid fat12 filesystem.
	ret = fat12_check();
	if( ret < 0 )
	{
		printk( KERN_ERR
			"%s(): not a valid fat12 fs.\n",
			__FUNCTION__ );
		goto out;
	}

	// Read the FAT.
	ret = fdc_read( 1, (byte *)&fat, fat12_super.SectorsPerFat );
	if( ret < 0 )
	{
		// Invalidate the filesystem.
		fat12_invalidate_mount();
		printk( KERN_ERR
			"%s(): floppy error %i. Unable to read the fat blocks.\n",
			__FUNCTION__, ret );
		goto out;
	}

	// Initialize the begin of the root directory and data area.
	dir_start = fat12_super.Fats * fat12_super.SectorsPerFat + 1;
	data_start = dir_start + (fat12_super.RootDirectoryEntries * 32 / fat12_super.BytesPerSector) - 2;

	// Converts the FAT into the logical structures (array of word).
	for( i = 0, j = 0; i < 4608; i += 3 )
	{
		lfat.data[ j++ ] = (fat.data[i] + (fat.data[i+1] << 8)) & 0x0FFF;
		lfat.data[ j++ ] = (fat.data[i+1] + (fat.data[i+2] << 8)) >> 4;
	}

	ret = FAT12_ROOT_DIR;
out:
	UP( &fat12_mutex );
	return( ret );
}

//! Umount the fat12 filesystem.
int fat12_umount( void )
{
	int ret;

	DOWN( &fat12_mutex );
	ret = fat12_check();
	if( ret < 0 )
	{
		UP( &fat12_mutex );
		return( ret );
	}

	// Syncronize the filesystem buffers.
	__fat12_sync();

	// Flush the floppy buffers.
	fdc_sync();

	// Invalidate the filesystem.
	fat12_invalidate_mount();

	UP( &fat12_mutex );
	return( 0 );
}
