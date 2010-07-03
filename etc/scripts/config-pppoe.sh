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
opmode=`nvram_get 2860 OperationMode`

killall -q pppd > /dev/null 2>&1
killall -q xl2tpd > /dev/null 2>&1
LOG="logger -t vpnhelper"

if [ "$IFACE" = "WAN" ]; then
    if [ "$opmode" = "1" ]; then
	IFACE=eth2.2
    elif [ "$opmode" = "1" ]; then
	IFACE=ra0
    elif [ "$opmode" = "3" ]; then
	IFACE=apcli0
    fi
else
    IFACE=br0
fi

IN_BR=`brctl show | grep $IFACE -c`
if [ "$IFACE" != "br0" ] && [ "$IN_BR" = "1" ]; then
    PPPOE_IFACE="br0"
    $LOG "$PPPOE_IFACE in bridge. Set pppoe interface to br0"
fi

if [ "$SERVER" != "" ] ; then
    SERVER="rp_pppoe_ac $SERVER"
else
    SERVER=""
fi

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

$LOG "Start pppd at $IFACE to $SERVER mode PPPOE"
FULLOPT="$OPTFILE $MTU $MRU $MPPE $PPP_STD_OPTIONS plugin /lib/rp-pppoe.so $PPPOE_CMD"
pppd $FULLOPT &
