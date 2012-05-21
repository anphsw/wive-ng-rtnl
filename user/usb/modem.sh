#!/bin/sh

##########################################################################################################
#
# Autodetect 3G Modem port (Wive-NG-Project)
#
##########################################################################################################

LOG="logger -t 3G"

symlink_modem() {
    ln -s /dev/$MDEV /dev/modem
}

symlink_diag() {
    ln -s /dev/$MDEV /dev/diag
}

symlink_pcui() {
    ln -s /dev/$MDEV /dev/pcui
}

symlink_pcsc() {
    ln -s /dev/$MDEV /dev/pcsc
}

symlink_pcsc() {
    ln -s /dev/$MDEV /dev/ndis
}

if [ "$ACTION" = "add" ]; then
    for path in $(find /sys/devices -name "$MDEV" 2>/dev/null); do
        DEVPATH=${path#/sys}
    done
    [ -z "$DEVPATH" ] && $LOG "environment variable DEVPATH is unset" && exit 1
    if [ -d /sys${DEVPATH} ]; then
	cd /sys${DEVPATH}/..
	for var in id[PV]*; do
	    [ -r $var ] && eval "$var='$(cat $var)'"
	done
	bInterfaceNumber=$(cat /sys${DEVPATH}/bInterfaceNumber 2>/dev/null)
	bInterfaceProtocol=$(cat /sys${DEVPATH}/bInterfaceProtocol 2>/dev/null)
	if [ "${idVendor}" = "12d1" ]; then
	    case "${idProduct}" in
		"1001")
		    case "$bInterfaceNumber" in
			"00")
			    symlink_modem
			    ;;
			"01")
			    symlink_diag
			    ;;
			"02")
			    symlink_pcui
			    ;;
			*)
			    $LOG "Oooops ${idVendor}:${idProduct} $bInterfaceNumber"
			    ;;
		    esac
		    ;;
		"1003")
		    case "$bInterfaceNumber" in
			"00")
			    symlink_modem
			    ;;
			"01")
			    symlink_pcui
			    ;;
			*)
			    $LOG "Oooops ${idVendor}:${idProduct} $bInterfaceNumber"
			    ;;
		    esac
		    ;;
		"1004")
		    case "$bInterfaceNumber" in
			"00")
			    symlink_modem
			    ;;
			"01")
			    symlink_diag
			    ;;
			"02")
			    symlink_pcui
			    ;;
			"03")
			    symlink_pcsc
			    ;;
			*)
			    $LOG "Oooops ${idVendor}:${idProduct} $bInterfaceNumber"
			    ;;
		    esac
		    ;;
		"1401")
		    case "$bInterfaceNumber" in
			"00")
			    symlink_diag
			    ;;
			"02")
			    symlink_pcui
			    ;;
			"03")
			    symlink_modem
			    ;;
			*)
			    $LOG "Oooops ${idVendor}:${idProduct} $bInterfaceNumber"
			    ;;
		    esac
		    ;;
		"1402")
		    case "$bInterfaceNumber" in
			"00")
			    symlink_modem
			    ;;
			"01")
			    symlink_ndis
			    ;;
			"02")
			    symlink_pcui
			    ;;
			*)
			    $LOG "Oooops ${idVendor}:${idProduct} $bInterfaceNumber"
			    ;;
		    esac
		    ;;
		"1403")
		    case "$bInterfaceNumber" in
			"00")
			    symlink_pcui
			    ;;
			"01")
			    symlink_ndis
			    ;;
			"02")
			    symlink_diag
			    ;;
			"03")
			    symlink_pcsc
			    ;;
			*)
			    $LOG "Oooops ${idVendor}:${idProduct} $bInterfaceNumber"
			    ;;
		    esac
		    ;;
		*)
		    $LOG "Unknow Huawei modem ${idVendor}:${idProduct}"
		    ;;
	    esac
	fi
    fi
else

fi

exit 0
