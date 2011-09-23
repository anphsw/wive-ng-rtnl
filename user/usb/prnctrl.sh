#!/bin/sh

#include global config
. /etc/scripts/global.sh

LOG="logger -t prnctrl"
port=${MDEV##*lp}

if [ "$ACTION" = "add" ]; then
    if [ -z "`pidof p910nd`" ]; then
	# For GDI printers put printers firmware
	# file in /etc, rename to prnfw.dl
	if [ -f /etc/prnfw.dl ]; then
	    $LOG "Upload firmware /etc/prnfw.dl to printer $MDEV."
	    cat /etc/prnfw.dl > /dev/usb/$MDEV
	fi
	$LOG "Start p910nd daemon on port 910${port}"
	/bin/p910nd -b -f /dev/usb/$MDEV $port
	if [ -z "$(iptables -L | grep "jetdirect")" ]; then
	    $LOG "Add p910nd firewall rules"
	    iptables -A servicelimit -i $lan_if -p tcp --dport 9100 -j ACCEPT > /dev/null 2>&1
	fi
    fi
else
    if [ ! -z "`pidof p910nd`" ]; then
	$LOG "Stop p910nd daemon on port 910${port}"
	killall -q p910nd
	killall -q -9 p910nd
    fi
    iptables -D servicelimit -i $lan_if -p tcp --dport 9100 -j ACCEPT > /dev/null 2>&1
    rmmod usblp > /dev/null 2>&1
    $LOG "Delete p910nd firewall rules and remove USBLP kernel module"
fi

exit 0
