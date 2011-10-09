#!/bin/sh


LOG="logger -t cdp"

cdpEnabled=`nvram_get 2860 cdpEnabled`
if [ "`pidof cdp-send`" ]; then
    killall -q cdp-send
    killall -q -SIGKILL cdp-send
fi
if [ -f /bin/cdp-send ] && [ "$cdpEnabled" = "1" ]; then
    . /etc/scripts/global.sh
    DEVNAME=`grep "DEVNAME" < /etc/version | awk {' print $3 '}`
    VERSIONPKG=`grep "VERSIONPKG" < /etc/version | awk {' print $3 '}`
    r_version="$DEVNAME-$VERSIONPKG"
    r_hostname=`hostname`
    r_cap="l3r"
    $LOG "Send CDP request mode $r_cap options $r_version $r_hostname $wan_if"
    cdp-send -t 120 -c "$r_cap" -s "$r_version" -n "$r_hostname" "$wan_if" & > /dev/null 2>&1
fi
