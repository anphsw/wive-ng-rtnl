#!/bin/sh
#
# $Id: config-l2tp.sh,v 1.5 2008-08-21 09:42:47 chhung Exp $
#
# usage: config-l2tp.sh <mode> <mode_params> <server> <user> <password>
#

echo "==================START-L2TP-CLIENT======================="

SERVER=`nvram_get 2860 vpnServer`
USER=`nvram_get 2860 vpnUser`
PASSWORD=`nvram_get 2860 vpnPassword`
MTU=`nvram_get 2860 vpnMTU`
MPPE=`nvram_get 2860 vpnMPPE`
PEERDNS=`nvram_get 2860 vpnPeerDNS`
DEBUG=`nvram_get 2860 vpnDebug`

killall -q pppd > /dev/null 2>&1
killall -q xl2tpd > /dev/null 2>&1
LOG="logger -t vpnhelper"

    $LOG "Check for L2TP server reachable"
    reachable=0;
    while [ $reachable -eq 0 ]; do
        ping -q -c 1 $SERVER
        if [ "$?" -eq 0 ]; then
            reachable=1
        else
            $LOG "Server unreachable wait 30 sec."
            sleep 30
            reachable=0;
        fi
    done

    $LOG "Get vpn server ip adress"
    NS=`ipget $SERVER | tail -n1`
    if [ "$NS" != "" ]; then
        ADDRESS=$NS
        $LOG "Server adress is $ADDRESS"
        SERVER=$ADDRESS
    else
        $LOG "Not resolve adress for $SERVER"
        SERVER=$SERVER
    fi

    $LOG "Get route to vpn server."
    ROUTE=`ip r get $SERVER | grep dev | cut -f -3 -d " "`
    if [ "$ROUTE" != "" ] || [ "$ROUTE" != "0.0.0.0" ]; then
        $LOG "Add route to vpn server."
        ip r add $ROUTE
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

    #clear all configs
    ppp=/etc/ppp
    echo > $ppp/l2tpd.conf
    echo > $ppp/options.l2tp
    echo > $ppp/chap-secrets
    echo > $ppp/connect-errors

    printf "[global]
    access control = yes
    [lac $SERVER]\n
    redial = yes
    redial timeout = 20
    require chap = yes
    require authentication = no
    lns = $SERVER
    name = $USER
    require pap = no
    autodial = yes
    pppoptfile = $ppp/options.l2tp
    " >> $ppp/l2tpd.conf

    printf "
    connect /bin/true
    idle 0
    maxfail 0
    refuse-pap
    refuse-eap
    noipx
    noproxyarp
    mtu $MTU
    mru $MTU
    $MPPE
    $PEERDNS
    lcp-echo-failure        5                                                                                                    
    lcp-echo-interval       10 
    " >> $ppp/options.l2tp

    printf "$USER * $PASSWORD" >> $ppp/chap-secrets
    printf "$USER * $PASSWORD" >> $ppp/pap-secrets

    $LOG "Starting VPN network l2tp..."
    $LOG "Start xl2tpd"
    xl2tpd -c /etc/ppp/l2tpd.conf -s /etc/ppp/chap-secrets -p /var/lock/l2tpd.pid &
