#!/bin/sh

##################################################################
#
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

killall -q pppd > /dev/null 2>&1
killall -q xl2tpd > /dev/null 2>&1
LOG="logger -t vpnhelper"

if [ "$PEERDNS" = "on" ]; then
    PEERDNS=usepeerdns
else
    PEERDNS=
fi

if [ "$MPPE" = "on" ] ; then
    MPPE=allow-mppe-128
else
    MPPE=
fi 

if [ "$MTU" = "" ] || [ "$MTU" = "AUTO" ] ; then
    MTU=""
    MRU=""
else
    MRU="mru $MTU"
    MTU="mtu $MTU"
fi

if [ "$DEBUG" = "on" ] ; then
    DEBUG="debug"
else
    DEBUG=""
fi

OPTFILE="file /etc/ppp/options.pppoe"
# Standard PPP options we always use
PPP_STD_OPTIONS="noipdefault noauth persist $PEERDNS -detach $DEBUG"
# PPPoE invocation
PPPOE_CMD="$IFACE $SERVER user $USER password $PASSWORD"

$LOG "Start pppd"
FULLOPT="$OPTFILE $MTU $MRU $MPPE $PPP_STD_OPTIONS plugin /lib/rp-pppoe.so $PPPOE_CMD"
pppd $FULLOPT &
