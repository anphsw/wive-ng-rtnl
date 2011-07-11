#!/bin/sh

if [ -f /var/run/syslogd.pid ]; then
    LOG="logger -t usbctrl"
else
    LOG="echo usbctrl"
fi

if [ -z "$ACTION" ]; then
    for path in $(find /sys/devices -name "$MDEV" 2>/dev/null); do
        DEVPATH=${path#/sys}
    done
    MODALIAS=$(cat /sys${DEVPATH}/modalias 2>/dev/null)
    TYPE=$(echo $MODALIAS | printf '%d/%d/%d' $(sed 's/.*d[0-9]\{4\}//;s/ic.*//;s/[dscp]\+/ 0x/g'))
    PRODUCT=$(echo $MODALIAS | sed 's!^usb:\(.*\)dc.*!\1!;s![vpd]!/!g;s!/0\{1,3\}!/!g;s!^/!!;y!ABCDEF!abcdef!')
#' hack for mcedit
    INTERFACE=$(echo $MODALIAS | printf '%d/%d/%d' $(sed 's/.*dp[0-F]\{2\}//;s/[iscp]\+/ 0x/g'))
    for var in DEVPATH MODALIAS TYPE PRODUCT INTERFACE; do
        [ -z "$(eval "echo \$${var}")" ] && $LOG "Could not set uevent environment variable $var" && exit 1
    done
fi

[ -z "$DEVPATH" ] && $LOG "environment variable DEVPATH is unset" && exit 1
if [ -d /sys${DEVPATH} ]; then
    cd /sys${DEVPATH}/..
    for var in id[PV]*; do
        [ -r $var ] && eval "$var='$(cat $var)'"
    done
fi

[ 0 -eq "${TYPE%%/*}" ] && TYPE=$INTERFACE

case $TYPE in
    8/6/*)
	if [ -f "/usr/share/usb_modeswitch/${idVendor}:${idProduct}" ]; then
	    $LOG "${ACTION} ${idVendor}:${idProduct} may be 3G modem in zero CD mode"
	    usb_modeswitch -c /usr/share/usb_modeswitch/${idVendor}:${idProduct} && exit 0
	else
	    $LOG "${ACTION} ${idVendor}:${idProduct} may be storage"
	    modprobe -q usb-storage
	fi
        ;;
    255/255/255)
        $LOG "${ACTION} ${idVendor}:${idProduct} may be 3G modem"
        modprobe -q usbserial vendor=0x${idVendor} product=0x${idProduct}
        ;;
    *)
        $LOG "${ACTION} ${idVendor}:${idProduct} ${TYPE} ${INTERFACE}"
        modprobe -q $MODALIAS
        ;;
esac
