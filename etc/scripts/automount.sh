#!/bin/sh

MDEV_PATH=/dev/$MDEV

pre_mount() {
  if [ "$MDEV_LABEL" == "optware" ]; then
    MOUNT_DST="/opt"
  else
    MOUNT_DST="/media/$MDEV"
    if [ -d "$MOUNT_DST" ]; then
      exit 1
    else
      if ! mkdir -p "$MOUNT_DST"; then
	exit 1
      fi
    fi
  fi
}

try_mount() {
  modprobe -q "$MDEV_TYPE"
# mount -o utf8 not worked
#  if ! mount -o utf8,noatime -t "$MDEV_TYPE" "$MDEV_PATH" "$MOUNT_DST"; then
    if ! mount -o noatime -t "$MDEV_TYPE" "$MDEV_PATH" "$MOUNT_DST"; then
      if ! rm -r "$MOUNT_DST"; then
	exit 1
      fi
    fi
#  fi
}

if [ "$ACTION" = "add" ]; then
  eval "$(blkid $MDEV_PATH | sed 's/^[^ ]* //;s/\([^ ]*=\)/MDEV_\1/g')"
  case "$MDEV_TYPE" in
    ntfs)
      pre_mount
      if ! ntfs-3g "$MDEV_PATH" "$MOUNT_DST" -o force,noatime; then
	if ! rm -r "$MOUNT_DST"; then
	  exit 1
	fi
      fi
      ;;
    swap)
      if [ "$(nvram_get 2860 auto_swap)" = "1" ] && [ -f /bin/swapon ]; then
	swapon "$MDEV_PATH" > /dev/null 2>&1
      fi
      ;;
    *)
      pre_mount
      try_mount
      ;;
  esac
else
  if [ `mount | grep "$MDEV" | wc -l` -ge 1 ]; then
    sync
    if ! umount "$MDEV_PATH"; then
      if ! umount -l "$MDEV_PATH"; then
	exit 1
      fi
    fi
    if ! rm -r "/media/$MDEV"; then
      exit 1
    fi
  elif [ `cat /proc/swaps | grep "$MDEV" | wc -l` -ge 1 ]; then
    swapoff "$MDEV_PATH"
  fi
fi

killall -q -SIGTTIN goahead

exit 0
