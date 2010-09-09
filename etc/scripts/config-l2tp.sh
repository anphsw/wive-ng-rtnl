#!/bin/sh

#include global
. /etc/scripts/global.sh

SERVERNM=`nvram_get 2860 vpnServer`
USER=`nvram_get 2860 vpnUser`
PASSWORD=`nvram_get 2860 vpnPassword`
MTU=`nvram_get 2860 vpnMTU`
MPPE=`nvram_get 2860 vpnMPPE`
PEERDNS=`nvram_get 2860 vpnPeerDNS`
DEBUG=`nvram_get 2860 vpnDebug`
AUTHMODE=`nvram_get 2860 vpnAuthProtocol`

killall -q pppd > /dev/null 2>&1
killall -q xl2tpd > /dev/null 2>&1

modprobe -q ppp_generic > /dev/null 2>&1
modprobe -q pppox > /dev/null 2>&1
modprobe -q pppol2tp > /dev/null 2>&1

LOG="logger -t vpnhelper"

check_param() {
    if [ "$SERVERNM" = "" ] || [ "$USER" = "" ] || [ "$PASSWORD" = "" ]; then
	$LOG "Server adress, username or password not set. Exit..."
	exit 1
    fi
}

get_vpn_ip() {
    $LOG "Get vpn server $SERVERNM ip adress"
    NS=`ipget $SERVERNM | tail -n1`
    if [ "$NS" != "" ]; then
        SERVER=$NS
        $LOG "Server adress is $SERVER"
	nvram_set 2860 vpnServerIP "$SERVER"
    else
        SERVER=$SERVERNM
        $LOG "Not resolve adress for $SERVER"
    fi
}

echo "==================START-L2TP-CLIENT======================="
    check_param
    get_vpn_ip
    reachable=0;
    while [ $reachable -eq 0 ]; do
	$LOG "Check for L2TP server $SERVER reachable"
        ping -q -c 1 $SERVER
        if [ "$?" -eq 0 ]; then
            reachable=1
        else
            $LOG "Server unreachable wait 30 sec."
            sleep 30
	    get_vpn_ip
            reachable=0;
        fi
    done
    

    $LOG "Get route to vpn server."
    if [ -f /etc/default.gw ]; then
	newdgw=`cat /etc/default.gw`
    else
	newdgw=""
    fi

    if [ "$newdgw" != "" ] && [ "$newdgw" != "$SERVER" ]; then
	$LOG "Add route to $SERVER via $newdgw"
	ip route replace $SERVER via $newdgw metric 0
    fi

    if [ "$PEERDNS" = "on" ]; then
	PEERDNS=usepeerdns
    else
	PEERDNS=
    fi

    if [ "$MPPE" = "on" ] ; then
	modprobe ppp_mppe > /dev/null 2>&1
        MPPE=allow-mppe-128
        else
        MPPE=
    fi

    if [ "$DEBUG" = "on" ] ; then
        DEBUG="-D"
    else
        DEBUG=""
    fi

    if [ "$MTU" = "" ] || [ "$MTU" = "AUTO" ] ; then
        MTU=""
        MRU=""
        else
        MRU="mru $MTU"
        MTU="mtu $MTU"
    fi

    if [ "$AUTHMODE" = "1" ]; then
	L2TPPAP="require pap = yes"
	L2TPCHAP="require chap = no"
	PAP="require-pap"
	CHAP="refuse-chap"
    elif [ "$AUTHMODE" = "2" ]; then
	L2TPPAP="require pap = no"
	L2TPCHAP="require chap = yes"
	PAP="refuse-pap"
	CHAP="require-chap"
    elif [ "$AUTHMODE" = "3" ]; then
	L2TPPAP="require pap = no"
	L2TPCHAP="require chap = yes"
	PAP="refuse-pap"
	CHAP="refuse-chap"
    else
	L2TPPAP=""
	L2TPCHAP=""
	PAP=""
	CHAP=""
    fi

    #clear all configs
    ppp=/etc/ppp
    echo > $ppp/l2tpd.conf
    echo > $ppp/options.l2tp

    printf "[global]
    access control = yes
    [lac $SERVER]\n
    redial = yes
    redial timeout = 20
    $L2TPPAP
    $L2TPCHAP
    require authentication = no
    lns = $SERVER
    name = $USER
    autodial = yes
    pppoptfile = $ppp/options.l2tp
    " >> $ppp/l2tpd.conf

    printf "
    connect /bin/true
    idle 0
    maxfail 0
    refuse-eap
    $PAP
    $CHAP
    noipx
    noproxyarp
    $MTU
    $MRU
    $MPPE
    $PEERDNS
    lcp-echo-failure        10
    lcp-echo-interval       5 
    " >> $ppp/options.l2tp

    printf "$USER * $PASSWORD" >> $ppp/chap-secrets
    printf "$USER * $PASSWORD" >> $ppp/pap-secrets

    $LOG "Starting VPN network l2tp..."
    $LOG "Start xl2tpd"
    FULLOPTS="$DEBUG -c /etc/ppp/l2tpd.conf -s /etc/ppp/chap-secrets -p /var/lock/l2tpd.pid"
    xl2tpd $FULLOPTS &
