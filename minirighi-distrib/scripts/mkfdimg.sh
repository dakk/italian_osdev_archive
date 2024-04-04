#!/bin/sh
#
# This script is used by the top makefile to install minirighi
# on a floppy disk image.
#
#      Author: Andrea Righi <righiandr@users.sf.net>
#

# This script can be used only by the top makefile #
if [ "$MAKE" == "" ]; then
	echo "Only the top makefile can run this script!!!"
	exit 1
fi

# Create the mtools configuration file #
echo -n "Creating mtools configuration file... "
rm -f $MTOOLSRC
touch $MTOOLSRC
echo "#" > $MTOOLSRC
echo "# Automatically generated by make config: don't edit" >> $MTOOLSRC
echo "#" >> $MTOOLSRC
echo "drive a: file=\"$FDIMAGE\" 1.44m mformat_only" >> $MTOOLSRC
echo "done."

#echo -e "\n\t!!! WARNING !!!\n"
#echo -e "Every data in \"$FDIMAGE\" will be lost!!!"
#echo -e "Press ENTER to continue or CTRL+C to break..."
#read

# Create the floppy disk image #
if test ! -e $FDIMAGE
then
	echo "Creating file $FDIMAGE... "
	dd if=/dev/zero of=$FDIMAGE bs=512 count=2880
	echo "done."
fi

# Formatting disk #
echo -n "Formatting disk image... "
mformat a:
echo "done."

# Creating directories #
echo -n "Creating directories... "
mmd a:/bin
mmd a:/boot
mmd a:/boot/grub
mmd a:/etc
mmd a:/kernel
mmd a:/home
mmd a:/home/doc
echo "done."

# Copying generic files #
echo -n "Copying generic files.. "
mcopy COPYING a:/home/doc/
mcopy INSTALL a:/home/doc/
mcopy README a:/home/doc/
echo "done."

# Generating GRUB configuration file #
echo -n "Generating GRUB file menu.lst... "
cat << EOF > $TOPDIR/boot/grub/menu.lst
#
# Automatically generated by make: do not edit!
#

default 0

title $KERNELNAME $VERSION
root (fd0)
kernel /kernel/mr32.gz

title reboot
reboot

title halt
halt

EOF
echo "done."

# Installing GRUB #
echo -n "Copying GRUB files... "
mcopy $TOPDIR/boot/grub/stage1 a:/boot/grub
mcopy $TOPDIR/boot/grub/stage2 a:/boot/grub
mcopy $TOPDIR/boot/grub/menu.lst a:/boot/grub
echo "done."

echo -n "Installing GRUB... "
if test ! -x $GRUB;
then
	echo -e "\nYou cannot execute GRUB ($GRUB)!!!";
	exit 1;
fi
$GRUB --batch << EOF 1>/dev/null 2>/dev/null || exit 1
device (fd0) $FDIMAGE
install (fd0)/boot/grub/stage1 (fd0) (fd0)/boot/grub/stage2 p (fd0)/boot/grub/menu.lst
quit
EOF

echo "done."

# Well done. #
echo
echo "Floppy image created on $FDIMAGE."
echo