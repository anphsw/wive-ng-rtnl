#!/bin/sh


LOG="logger -t cdp"

cdpEnabled=`nvram_get 2860 cdpEnabled`
if [ "`pidof cdp-send`" ]; then
    killall -q cdp-send
    killall -q -SIGKILL cdp-send
fi
if [ -f /bin/cdp-send ] && [ "$cdpEnabled" = "1" ]; then
    . /etc/scripts/global.sh
    DEVNAME=`grep "DEVNAME" < /etc/version | awk {' print $3 '} | sed s'@"@@'g`
    VERSIONPKG=`grep "VERSIONPKG" < /etc/version | awk {' print $3 '} | sed s'@"@@'g`
    r_hostname=`hostname`
    $LOG "Send CDP request options $r_version $r_hostname $wan_if"
    cdp-send -t 120 -m $DEVNAME -s $VERSIONPKG -n $r_hostname "$wan_if" & > /dev/null 2>&1
fi
