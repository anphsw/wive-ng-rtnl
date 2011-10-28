#!/bin/sh

#include global config
. /etc/scripts/global.sh

LOG="logger -t prnctrl"
port=${MDEV##*lp}

if [ "$ACTION" = "add" ]; then
    eval `nvram_buf_get 2860 PrinterSrvEnabled PrinterSrvBidir`
    if [ "$PrinterSrvEnabled" = "1" ] && [ -z "`pidof p910nd`" ]; then
	# For GDI printers put printers firmware
	# file in /etc, rename to prnfw.dl
	if [ -f /etc/prnfw.dl ]; then
	    $LOG "Upload firmware /etc/prnfw.dl to printer $MDEV."
	    cat /etc/prnfw.dl > /dev/usb/$MDEV
	fi
	$LOG "Start p910nd daemon on port 910${port}"
	if "$PrinterSrvBidir" = "1" ]; then
	    /bin/p910nd -b -f /dev/usb/$MDEV $port
	else
	    /bin/p910nd -f /dev/usb/$MDEV $port
	fi
    fi
else
    if [ ! -z "`pidof p910nd`" ]; then
	$LOG "Stop p910nd daemon on port 910${port}"
	killall -q p910nd
	killall -q -9 p910nd
	# svae mem in 16Mb devices
	if [ -f /tmp/is_16ram_dev ]; then
	    rmmod usblp > /dev/null 2>&1
	fi
    fi
fi

exit 0
