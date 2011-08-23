#!/bin/sh

try_mount() {
if ! mount -o utf8, noatime "$MOUNT_SRC" "$MOUNT_DST"; then
    if ! mount -o noatime "$MOUNT_SRC" "$MOUNT_DST"; then
	if ! ntfs-3g "$MOUNT_SRC" "$MOUNT_DST" -o force,noatime; then
	    if ! rm -r "$MOUNT_DST"; then
		exit 1
	    fi
	fi
    fi
fi
}

try_umount() {
mount_point=`mount | grep $MDEV | awk '{print $3}'`
if [ `mount | grep $MDEV | wc -l` -ge 1 ]; then
    sync
    if ! umount "/dev/$MDEV"; then
	if ! umount -l "/dev/$MDEV"; then
	    exit 1
	fi
    fi
    if ! rm -r "$mount_point"; then
        exit 1
    fi
fi
}

swap_on() {
auto_swap=`nvram_get 2860 auto_swap`
if [ "$auto_swap" = "1" ] && [ -f /bin/swapon ] && [ -f /bin/fdisk ]; then
    SWAPPART=`fdisk -l | grep swap | awk {' print $1 '}`
    if [ "$SWAPPART" != "" ]; then
	swapon $SWAPPART > /dev/null 2>&1
    fi
fi
}

swap_off() {
if [ -f /bin/swapoff ]; then
    swapoff -a > /dev/null 2>&1
fi
}

if [ "$ACTION" = "add" ] ; then
    swap_on
    if [ -f /bin/blkid ]; then
	eval "$(blkid /dev/$MDEV | sed 's/^[^ ]* //;s/\([^ ]*=\)/MDEV_\1/g')"
	readonly MDEV_TYPE MDEV_UUID MDEV_LABEL MDEV_PATH=/dev/$MDEV
	line=`cat /etc/fstab | egrep -m 1 "^(UUID=${MDEV_UUID}|LABEL=${MDEV_LABEL}|${MDEV_PATH})[ \t]"`
	if [ -z "$line" ]; then
	    MOUNT_SRC=$MDEV_PATH
	    if [ -z "$MDEV_LABEL" ]; then
		MOUNT_DST="/media/$MDEV_UUID"
	    else
		MOUNT_DST="/media/$MDEV_LABEL"
	    fi
	else
	    MOUNT_SRC=`echo $line | sed 's/ .*//'`
	    if echo "$line" | egrep -q '[, \t]noauto[, \t]'; then
		#no auto mounting
		exit 0
	    fi
	    MOUNT_DST=`echo $line | sed 's/^[^ \t]*[ \t]*//;s/[ \t].*//'`
	fi
	if [ -d "$MOUNT_DST" ]; then
	    [ -z "$line" ] && exit 1
	else
	    if ! mkdir -p "$MOUNT_DST"; then
		exit 1
	    fi
	fi
	try_mount
    else
	MOUNT_SRC="/dev/$MDEV"
	MOUNT_DST="/media/$MDEV"
	if ! mkdir -p "$MOUNT_DST"; then
	    exit 1
	fi
	try_mount
    fi
else
    swap_off
    try_umount
fi

killall -q -SIGTTIN goahead

exit 0
