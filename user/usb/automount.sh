#!/bin/sh

# include global config
. /etc/scripts/global.sh

LOG="logger -t automount"
MDEV_PATH=/dev/$MDEV

check_media() {
  if [ `mount | grep "media" | wc -l` -ge 1 ]; then
    $LOG "/media is binding to rw"
  else
    $LOG "prepare /media"
    mount -t tmpfs -o size=4K tmpfs /media
  fi
}

pre_mount() {
  if [ "$MDEV_LABEL" == "optware" ]; then
    $LOG "optware part"
    MOUNT_DST="/opt"
  else
    check_media
    MOUNT_DST="/media/$MDEV"
    if [ -d "$MOUNT_DST" ]; then
      $LOG "dir $MOUNT_DST is exist"
      exit 1
    else
      if ! mkdir -p "$MOUNT_DST"; then
	$LOG "can not create $MOUNT_DST"
	exit 1
      fi
    fi
  fi
}

try_mount() {
  modprobe -q "$MDEV_TYPE"
# mount -o utf8 not worked need external mount, busybox not support this
#  if ! mount -o utf8,noatime -t "$MDEV_TYPE" "$MDEV_PATH" "$MOUNT_DST"; then
    if ! mount -o noatime -t "$MDEV_TYPE" "$MDEV_PATH" "$MOUNT_DST"; then
      $LOG "can not mount"
      mount_err
    fi
#  fi
  if [ "$MDEV_LABEL" == "optware" ]; then
    #re read profile variables
    . /etc/profile
  fi
}

try_ntfs() {
  if ! ntfs-3g "$MDEV_PATH" "$MOUNT_DST" -o force,noatime; then
    $LOG "can not mount NTFS"
    mount_err
  fi
}

swap_on() {
  auto_swap=`nvram_get 2860 auto_swap`
  if [ "$auto_swap" = "1" ] && [ -f /bin/swapon ]; then
    $LOG "Swap on dev $MDEV_PATH"
    swapon "$MDEV_PATH" > /dev/null 2>&1
  fi
}

try_umount() {
  MOUNT_DST=`mount | grep "$MDEV" | awk '{print $3}'`
  if [ "$MOUNT_DST" ]; then
    $LOG "umount"
    sync
    if ! umount "$MOUNT_DST"; then
      if ! umount -fl "$MOUNT_DST"; then
	$LOG "can not unmount"
	exit 1
      fi
    fi
    mount_err
  fi
}

swap_off() {
  if [ `cat /proc/swaps | grep "$MDEV" | wc -l` -ge 1 ]; then
    $LOG "swap off dev $MDEV_PATH"
    swapoff "$MDEV_PATH"
  fi
}

mount_err() {
  if ! rm -r "$MOUNT_DST"; then
    $LOG "can not del $MOUNT_DST"
    exit 1
  fi
}

drop_disk_caches

if [ "$ACTION" = "add" ]; then
  # wait for disc appear, max 15 sec
  i=0
  while [ -z "$(ls /sys/class/scsi_disk/)" -a -z "$(ls /sys/bus/ide/devices/ 2>/dev/null)" ]; do
    sleep 1
    i=$((i + 1))
    if [ $i = "1" ]; then
	$LOG "Wait for disc appear, max 15 sec"
    fi
    if [ $i -gt 15 ]; then
	break
    fi
  done

  # start prepare and mounts procedure
  eval "$(blkid $MDEV_PATH | sed 's/^[^ ]* //;s/\([^ ]*=\)/MDEV_\1/g')"
  $LOG "add $MDEV_PATH with $MDEV_TYPE"
  case "$MDEV_TYPE" in
    ntfs)
      pre_mount
      try_ntfs
      ;;
    swap)
      swap_on
      ;;
    *)
      pre_mount
      try_mount
      ;;
  esac
else
  $LOG "remove $MDEV_PATH"
  try_umount
  swap_off
fi

drop_disk_caches

if [ "$MDEV_LABEL" != "optware" ] && [ "$MDEV_TYPE" != "swap" ]; then
    #regenerate config and restart samba
    service samba restart
    service transmission restart
fi

exit 0
