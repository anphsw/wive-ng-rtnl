#!/bin/sh
#
# $Id: config-pppoe.sh,v 1.4 2008-07-04 12:39:13 chhung Exp $
#
# usage: config-pppoe.sh <user> <password> <wan_if_name>
#
echo "==================START-PPPOE-CLIENT======================="

SERVER=`nvram_get 2860 vpnServer`
USER=`nvram_get 2860 vpnUser`
PASSWORD=`nvram_get 2860 vpnPassword`
MTU=`nvram_get 2860 vpnMTU`
MPPE=`nvram_get 2860 vpnMPPE`
PEERDNS=`nvram_get 2860 vpnPeerDNS`
DEBUG=`nvram_get 2860 vpnDebug`
IFACE=`nvram_get 2860 vpnInterface`
ROUTING=`nvram_get 2860 vpnRoutingEnabled`

killall -q pppd > /dev/null 2>&1
killall -q xl2tpd > /dev/null 2>&1
LOG="logger -t vpnhelper"

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

if [ "$PEERDNS" = "on" ]; then
    PEERDNS=usepeerdns
else
    PEERDNS=
fi

#clear all configs
ppp=/etc/ppp
echo > $ppp/chap-secrets
echo > $ppp/pap-secrets

OPTFILE="file /etc/ppp/options.pppoe"
# Standard PPP options we always use
PPP_STD_OPTIONS="noipdefault noauth -detach persist $PEERDNS"
# PPPoE invocation
PPPOE_CMD="$3 user $1 password $2"

$LOG "Start pppd"
FULLOPT="$OPTFILE mtu $MTU mru $MTU $PPP_STD_OPTIONS plugin /lib/rp-pppoe.so $PPPOE_CMD"
pppd $FULLOPT &
