/*!	\file fs/vfs.c
 *	\brief Virtual File System for Devices.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-16
 *	\note TODO: under construction!!!
 */

#include <const.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <kernel/semaphore.h>
#include <kernel/vfs.h>

//! A semaphore to preserve the mutual exclusion for character devices.
DECLARE_MUTEX( chrdev_mutex );

//! A semaphore to preserve the mutual exclusion for block devices.
DECLARE_MUTEX( blkdev_mutex );

//! The character device list.
static struct chrdev_struct chr_devices[ MAX_CHRDEV ];

//! The block device list.
static struct blkdev_struct blk_devices[ MAX_BLKDEV ];

//! Get the character device list.
int get_chrdev_list( char *list, size_t size )
{
	int i, len;

	if( !size )
		return( 0 );

	len = snprintf( list, size, "Character devices:\n" );
	DOWN( &chrdev_mutex );
	for ( i=0; i<MAX_CHRDEV; i++ )
	{
		if( chr_devices[i].fops )
		{
			if( size-len<0 )
				break;
			len += snprintf( list+len, size-len, "%3d %s\n",
				i, chr_devices[i].name );
		}
	}
	UP( &chrdev_mutex );
	return( len );
}

//! Get the block device list.
int get_blkdev_list( char *list, size_t size )
{
	int i, len;

	if( !size )
		return( 0 );

	len = snprintf( list, size, "Block devices:\n" );
	DOWN( &blkdev_mutex );
	for ( i=0; i<MAX_BLKDEV; i++ )
	{
		if( blk_devices[i].fops )
		{
			if( size-len<0 )
				break;
			len += snprintf( list+len, size-len, "%3d %s\n",
				i, blk_devices[i].name );
		}
	}
	UP( &blkdev_mutex );
	return( len );
}

//! Get the complete device list (char+block).
int get_device_list( char *list, size_t size )
{
	int len;

	len = get_chrdev_list( list, size );
	if( size-len>=0 )
		len += get_blkdev_list( list, size-len );

	return( len );
}

//! Register a character device.
int register_chrdev( unsigned int major, const char *name, struct file_operations *fops )
{
	// The specified major number must be valid.
	if( major>=MAX_CHRDEV )
		return( -EINVAL );

	// If major is zero the system search a unused major slot.
	if( major==0 )
	{
		DOWN( &chrdev_mutex );
		for( major=MAX_CHRDEV-1; major>0; major-- )
		{
			if( chr_devices[major].fops==NULL )
			{
				// OK! Register the device.
				chr_devices[major].name = name;
				chr_devices[major].fops = fops;
				UP( &chrdev_mutex );
				return( major );
			}
		}
		// No slot free!
		UP( &chrdev_mutex );
		return( -EBUSY );
	}
	// Register the device into the specified major slot.
	DOWN( &chrdev_mutex );
	if( chr_devices[major].fops && chr_devices[major].fops != fops )
	{

		UP( &chrdev_mutex );
		return( -EBUSY );
	}
	chr_devices[major].name = name;
	chr_devices[major].fops = fops;
	UP( &chrdev_mutex );
	return( 0 );
}

//! Unregister a character device.
int unregister_chrdev( unsigned int major, const char *name )
{
	// The specified major number must be valid.
	if( major>=MAX_CHRDEV )
		return( -EINVAL );

	// Free the block if it is used.
	DOWN( &chrdev_mutex);
	if( !chr_devices[major].fops || strcmp(chr_devices[major].name, name) )
	{
		UP( &chrdev_mutex );
		return( -EINVAL );
	}
	chr_devices[major].name = NULL;
	chr_devices[major].fops = NULL;
	UP( &chrdev_mutex );
	return( 0 );
}

//! Register a block device.
int register_blkdev( unsigned int major, const char *name, struct file_operations *fops )
{
	// The specified major number must be valid.
	if( major>=MAX_BLKDEV )
		return( -EINVAL );

	// If major is zero the system search a unused major slot.
	if( major==0 )
	{
		DOWN( &blkdev_mutex );
		for( major=MAX_BLKDEV-1; major>0; major-- )
		{
			if( blk_devices[major].fops==NULL )
			{
				// OK! Register the device.
				blk_devices[major].name = name;
				blk_devices[major].fops = fops;
				UP( &blkdev_mutex );
				return( major );
			}
		}
		// No slot free!
		UP( &blkdev_mutex );
		return( -EBUSY );
	}
	// Register the device into the specified major slot.
	DOWN( &blkdev_mutex );
	if( blk_devices[major].fops && blk_devices[major].fops != fops )
	{

		UP( &blkdev_mutex );
		return( -EBUSY );
	}
	blk_devices[major].name = name;
	blk_devices[major].fops = fops;
	UP( &blkdev_mutex );
	return( 0 );
}

//! Unregister a character device.
int unregister_blkdev( unsigned int major, const char *name )
{
	if( major>=MAX_BLKDEV )
		return( -EINVAL );

	// Free the block if it is used.
	DOWN( &blkdev_mutex);
	if( !blk_devices[major].fops || strcmp(blk_devices[major].name, name) )
	{
		UP( &blkdev_mutex );
		return( -EINVAL );
	}
	blk_devices[major].name = NULL;
	blk_devices[major].fops = NULL;
	UP( &blkdev_mutex );
	return( 0 );
}
