#!/bin/sh

#include global config
. /etc/scripts/global.sh

LOG="logger -t prnctrl"
port=${MDEV##*lp}

if [ "$ACTION" = "add" ]; then
    if [ -z "$(iptables -L | grep "jetdirect")" ]; then
	iptables -A servicelimit -i $lan_if -p tcp --dport 9100 -j ACCEPT > /dev/null 2>&1
	$LOG "Add p910nd firewall rules"
    fi
    # For GDI printers put printers firmware file in /etc, rename to prnfw.dl
    if [ -f /etc/prnfw.dl ]; then
	cat prnfw.dl > /dev/usb/$MDEV
    fi
    if [ -z "`pidof p910nd`" ]; then
	$LOG "Start p910nd daemon on port 910${port}"
	/bin/p910nd -b -f /dev/usb/$MDEV $port
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
