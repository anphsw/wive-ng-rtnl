#! /bin/sh
#echo "run automount.sh" >> /tmp/auto01
if [ "$1" == "" ]; then
echo "parameter is none" 
exit 1
fi
mounted=`mount | grep $1 | wc -l`

#echo "run automount.sh test2" >> /tmp/auto01
# mounted, assume we umount
if [ $mounted -ge 1 ]; then
echo "R/media/$2" 
echo "R/media/$2" 
if ! umount "/media/$2"; then
exit 1
fi

#echo "run automount.sh test3" >> /tmp/auto01

if ! rm -r "/media/$2"; then
exit 1
fi
# not mounted, lets mount under /media
else
if ! mkdir -p "/media/$2"; then
exit 1
fi

#echo "run automount.sh test4" >> /tmp/auto01

if ! mount "/dev/$1" "/media/$2" -o iocharset=utf8; then
if ! mount "/dev/$1" "/media/$2"; then		# ext2, ext3
# failed to mount, clean up mountpoint
#if ! ntfs-3g "/dev/$1" "/media/$2" -o force; then
if ! ntfs-3g "/dev/$1" "/media/$2" -o silent,umask=0,locale=utf8; then
if ! mount "/dev/sda" "/media/$2" iocharset=utf8; then		# maybe fat < 256M
if ! mount "/dev/sda" "/media/$2"; then		# ext2, ext3
	if ! rm -r "/media/$2"; then
		#echo "run automount.sh rm " >> /tmp/auto01
		exit 1
	fi
fi
fi
fi
#exit 1
fi
fi
echo "A/media/$2" 
echo "A/media/$2" 
fi

#echo "run automount.sh test5" >> /tmp/auto01
# rc need to know the event.
killall -SIGTTIN init
exit 0

