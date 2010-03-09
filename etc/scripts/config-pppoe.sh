#!/bin/sh
#
# $Id: config-pppoe.sh,v 1.4 2008-07-04 12:39:13 chhung Exp $
#
# usage: config-pppoe.sh <user> <password> <wan_if_name>
#
echo "==================START-PPPOE-CLIENT======================="

killall -q pppd > /dev/null 2>&1
killall -q xl2tpd > /dev/null 2>&1
LOG="logger -t vpnhelper"

if [ -f /etc/ppp/ip-down-route-reload ]; then
    $LOG "Load old dgw from file"
    /etc/ppp/ip-down-route-reload
    rm -f /etc/ppp/ip-down-route-reload
fi

usage()
{
	echo "Usage:"
	echo "  $0 <user> <password> <wan_if_name>"
	exit 1
}

if [ "$3" = "" ]; then
	echo "$0: insufficient arguments"
	usage $0
fi

#clear all configs
ppp=/etc/ppp
echo > $ppp/chap-secrets
echo > $ppp/pap-secrets

OPTFILE="file /etc/ppp/options.pppoe"
# Standard PPP options we always use
PPP_STD_OPTIONS="noipdefault noauth -detach persist usepeerdns"
# PPPoE invocation
PPPOE_CMD="$3 user $1 password $2"

$LOG "Start pppd"
pppd $OPTFILE mtu 1400 mru 1400 $PPP_STD_OPTIONS plugin /lib/rp-pppoe.so $PPPOE_CMD &
