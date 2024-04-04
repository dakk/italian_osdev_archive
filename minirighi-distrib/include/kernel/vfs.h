/*!	\file include/kernel/vfs.h
 *	\brief Virtual File System header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-16
 *	\note TODO: under construction!!!
 */

#ifndef VFS_H
#define VFS_H

#include <kernel/posix_types.h>

//! Max number of character devices.
#define MAX_CHRDEV	255

//! Max number of block devices.
#define MAX_BLKDEV	255

//! Bits for minor device number.
#define MINORBITS	8

//! Bit mask for minor device number.
#define MINORMASK	( (1U << MINORBITS) - 1 )

//! Get the major device number from a device identifier.
#define MAJOR( dev )	( (unsigned int) ((dev) >> MINORBITS) )

//! Get the minor device number from a device identifier.
#define MINOR( dev )	( (unsigned int) ((dev) & MINORMASK) )

//! Make a device identifier from major, minor numbers.
#define MKDEV( ma, mi )	( ((ma) << MINORBITS) | (mi) )

//! No device.
#define NODEV		0

//! Max length of a generic file/directory name.
#define NAME_MAX	255

//! The directory structure.
struct dirent
{
	long		d_ino;		//!< directory inode
	off_t		d_off;		//!< size
	unsigned short	d_reclen;	//!< record length
	char		d_name[NAME_MAX+1]; //!< name of the directory
};

//! The file structure.
struct file
{
	mode_t		f_mode;		//!< access type
	loff_t		f_pos;		//!< file position
	mode_t		f_flags;	//!< open() flags
	atomic_t	f_count;	//!< reference count
	pid_t		f_ownder;	//!< PID of owner
	struct file_operations *f_op;	//!< file operations
};

//! The structure for file operations.
struct file_operations
{
	int (*lseek)(struct file *, off_t, int);
	int (*read)(struct file *, char *, int);
	int (*write)(struct file *, char *, int);
	int (*readdir)(struct file *, struct dirent *, int);
	int (*ioctl)(struct file *, unsigned int, unsigned long);
	int (*open)(struct file *);
	void (*release)(struct file *);
	int (*fsync)(struct file *);
	int (*check_media_change)(dev_t);
	int (*revalidate)(dev_t);
};

//! The structure for a character device.
struct chrdev_struct
{
	//! Name.
	const char *name;
	//! File operations.
	struct file_operations *fops;
};

//! The structure for a block device.
struct blkdev_struct
{
	//! Name.
	const char *name;
	//! File operations.
	struct file_operations *fops;
	//! Size of the block.
	int block_size;
};

// --- Prototypes ----------------------------------------------------- //

int get_chrdev_list( char *list, size_t size );
int get_blkdev_list( char *list, size_t size );
int get_device_list( char *list, size_t size );

int register_chrdev( unsigned int major, const char *name, struct file_operations *fops );
int unregister_chrdev( unsigned int major, const char *name );
int register_blkdev( unsigned int major, const char *name, struct file_operations *fops );
int unregister_blkdev( unsigned int major, const char *name );

#endif
