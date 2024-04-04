/*!	\file include/kernel/kdev_t.h
 *	\brief Kernel devices types.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-05
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef KDEV_T_H
#define KDEV_T_H

typedef unsigned short	kdev_t;

#define MINORBITS	8
#define MINORMASK	( (1U << MINORBITS) - 1 )
#define MAJOR(dev)      ((unsigned int) ((dev) >> MINORBITS))
#define MINOR(dev)      ((unsigned int) ((dev) & MINORMASK))
#define HASHDEV(dev)    ((unsigned int) (dev))
#define NODEV           0
#define MKDEV(ma,mi)    (((ma) << MINORBITS) | (mi))

#endif
