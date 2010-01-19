#!/bin/sh
#
# $Id: config-pppoe.sh,v 1.4 2008-07-04 12:39:13 chhung Exp $
#
# usage: config-pppoe.sh <user> <password> <wan_if_name>
#

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

echo "Save default route to /var/tmp/dgw.ppp"
old_dgw=`route -n -e | awk '{def=$1} def=="0.0.0.0" {print $2}'`
if [ "$old_dgw" != "0.0.0.0" ] ; then
    echo $old_dgw > /var/tmp/dgw.ppp
fi
echo "Remove default route"
ip route del default 2> /dev/null

OPTFILE="file /etc/ppp/options.pppoe"
# Standard PPP options we always use
PPP_STD_OPTIONS="noipdefault noauth -detach defaultroute persist usepeerdns"
# PPPoE invocation
PPPOE_CMD="$3 user $1 password $2"

    killall -9 pppd
    killall -9 xl2tpd

pppd $OPTFILE mtu 1400 mru 1400 $PPP_STD_OPTIONS plugin /lib/rp-pppoe.so $PPPOE_CMD &

