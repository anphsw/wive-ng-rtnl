#!/bin/sh

LOG="logger -t prnctrl"
port=${MDEV##*lp}

if [ "$ACTION" = "add" ]; then
    if [ -z "$(iptables -L | grep "jetdirect")" ]; then
	iptables -A servicelimit -i br0 -p tcp --dport 9100 -j ACCEPT
	$LOG "Add p910nd firewall rules"
    fi
    # For GDI printers put printers firmware file in /etc, rename to prnfw.dl
    if [ -f /etc/prnfw.dl ]; then
	cat prnfw.dl > /dev/usb/$MDEV
    fi
    if [ -z "`pidof p910nd`" ]; then
	/bin/p910nd -f /dev/usb/$MDEV $port
	$LOG "Start p910nd daemon on port 910${port}"
    fi
else
    if [ ! -z "`pidof p910nd`" ]; then
	killall p910nd
	$LOG "Stop p910nd daemon on port 910${port}"
    fi
    iptables -D servicelimit -i br0 -p tcp --dport 9100 -j ACCEPT
    rmmod usblp
    $LOG "Delete p910nd firewall rules and remove USBLP kernel module"
fi

exit 0
