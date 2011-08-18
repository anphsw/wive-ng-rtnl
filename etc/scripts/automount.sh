#!/bin/sh

if [ "$ACTION" = "add" ] ; then
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
	if ! mount -o utf8 "$MOUNT_SRC" "$MOUNT_DST"; then
	    if ! mount "$MOUNT_SRC" "$MOUNT_DST"; then
		if ! ntfs-3g "$MOUNT_SRC" "$MOUNT_DST" -o force; then
		    if ! rm -r "$MOUNT_DST"; then
			exit 1
		    fi
		fi
	    fi
	fi
    else
	if ! mkdir -p "/media/$MDEV"; then
	    exit 1
	fi
	if ! mount "/dev/$MDEV" "/media/$MDEV"; then
	    if ! ntfs-3g "/dev/$MDEV" "/media/$MDEV" -o force; then
		if ! rm -r "/media/$MDEV"; then
		    exit 1
		fi
	    fi
	fi
    fi
else
    mount_point=`mount | grep $MDEV | awk '{print $3}'`
    if [ `mount | grep $MDEV | wc -l` -ge 1 ]; then
	if ! umount "/dev/$MDEV"; then
	    exit 1
	fi
	if ! rm -r "$mount_point"; then
	    exit 1
	fi
    fi
fi

killall -q -SIGTTIN goahead
exit 0
