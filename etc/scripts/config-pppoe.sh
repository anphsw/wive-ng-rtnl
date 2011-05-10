#!/bin/sh

###################################################
# config-pppoe.sh - configure PPPOE client helper #
###################################################

#include global config
. /etc/scripts/global.sh

SERVER=`nvram_get 2860 vpnServer`
SERVICE=`nvram_get 2860 vpnService`
USER=`nvram_get 2860 vpnUser`
PASSWORD=`nvram_get 2860 vpnPassword`
MTU=`nvram_get 2860 vpnMTU`
MPPE=`nvram_get 2860 vpnMPPE`
PEERDNS=`nvram_get 2860 vpnPeerDNS`
DEBUG=`nvram_get 2860 vpnDebug`
IFACE=`nvram_get 2860 vpnInterface`
AUTHMODE=`nvram_get 2860 vpnAuthProtocol`
LCPECHO=`nvram_get 2860 vpnEnableLCP`
OPTFILE="/etc/ppp/options.pppoe"

killall -q pppd
killall -q xl2tpd

modprobe -q ppp_generic > /dev/null 2>&1
modprobe -q pppox > /dev/null 2>&1
modprobe -q pppoe > /dev/null 2>&1

LOG="logger -t vpnhelper-pppoe"

echo "==================START-PPPOE-CLIENT======================="

check_param() {
    if [ "$USER" = "" ] || [ "$PASSWORD" = "" ]; then
	$LOG "Username or password not set. Exit..."
	exit 1
    fi
}

check_param

if [ "$IFACE" = "WAN" ]; then
    IFACE=$wan_if
elif [ "$IFACE" = "LAN" ]; then
    IFACE=$lan_if
else
    IFACE=br0
fi

IN_BR=`brctl show | grep $IFACE -c`
if [ "$IFACE" != "br0" ] && [ "$IN_BR" = "1" ]; then
    PPPOE_IFACE="br0"
    $LOG "$PPPOE_IFACE in bridge. Set pppoe interface to br0"
fi

if [ "$SERVER" != "" ]; then
    SERVER="rp_pppoe_ac $SERVER"
else
    SERVER=""
fi

if [ "$SERVICE" != "" ]; then
    SERVICE="rp_pppoe_service $SERVER"
else
    SERVICE=""
fi

if [ "$PEERDNS" = "on" ]; then
    PEERDNS=usepeerdns
else
    PEERDNS=
fi

if [ "$MPPE" = "on" ]; then
    modprobe -q crypto_algapi
    modprobe -q cryptomgr
    modprobe -q blkcipher
    modprobe -q ppp_mppe
    MPPE=allow-mppe-128
else
    MPPE=
fi 

if [ "$MTU" = "" ] || [ "$MTU" = "AUTO" ]; then
    MTU=""
    MRU=""
else
    MRU="mru $MTU"
    MTU="mtu $MTU"
fi

if [ "$DEBUG" = "on" ]; then
    DEBUG="debug"
else
    DEBUG=""
fi

if [ "$AUTHMODE" = "1" ]; then
    PAP="require-pap"
    CHAP="refuse-chap"
elif [ "$AUTHMODE" = "2" ]; then
    PAP="refuse-pap"
    CHAP="require-chap"
elif [ "$AUTHMODE" = "3" ]; then
    PAP="refuse-pap"
    CHAP="refuse-chap"
else
    PAP=""
    CHAP=""
fi

if [ "$LCPECHO" = "on" ]; then
    LCPECHO="lcp-echo-adaptive"
else
    LCPECHO=""
fi

cp -f /etc/ppp/options.template $OPTFILE
printf "
lock
nomp
lcp-echo-failure 10
lcp-echo-interval 5
$LCPECHO
$PAP
$CHAP
" >> $OPTFILE

# Standard PPP options we always use
PPP_STD_OPTIONS="noipdefault noauth persist $PEERDNS -detach $DEBUG"
# PPPoE invocation
PPPOE_CMD="$IFACE $SERVER $SERVICE user $USER password $PASSWORD"

$LOG "Start pppd at $IFACE to $SERVER $SERVICE mode PPPOE"
FULLOPT="file $OPTFILE $MTU $MRU $MPPE $PPP_STD_OPTIONS plugin /lib/rp-pppoe.so $PPPOE_CMD"
pppd $FULLOPT &
