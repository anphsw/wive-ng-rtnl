#!/bin/sh
#
# $Id: config-l2tp.sh,v 1.5 2008-08-21 09:42:47 chhung Exp $
#
# usage: config-l2tp.sh <mode> <mode_params> <server> <user> <password>
#

echo "==================START-L2TP-CLIENT======================="

SERVER=`nvram_get 2860 wan_l2tp_server`
USER=`nvram_get 2860 wan_l2tp_user`
PASSWORD=`nvram_get 2860 wan_l2tp_pass`

killall -q -9 pppd > /dev/null 2>&1
killall -q -9 xl2tpd > /dev/null 2>&1
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
    NS=`nslookup "$SERVER" 2>&1`
    if [ "$?" -eq 0 ]; then
        RESOLVEOK=`echo "$NS" | grep -c "can't resolve"`
	if [ "$RESOLVEOK" = "0" ]; then
	    ADDRESS=`echo "$NS" | grep Address | tail -n1 | cut -c 12- | awk {' print $1 '}`
	    $LOG "Server adress is $ADDRESS"
    	    SERVER=$ADDRESS
        else
	    $LOG "Not resolve adress for $SERVER st1"
    	    SERVER=$SERVER
	fi
        $LOG "Not resolve adress for $SERVER st2"
    fi

    $LOG "Get route to vpn server."
    ROUTE=`ip r get "$SERVER" | grep dev | cut -f -3 -d " "`
    if [ "$ROUTE" != "" ] || [ "$ROUTE" != "0.0.0.0" ]; then
        echo "Add route to vpn server."
        ip r add $ROUTE
    fi

    $LOG "Remove default route"
    ip route del default 2> /dev/null

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
    mtu 1400
    mru 1400
    defaultroute
    usepeerdns
    " >> $ppp/options.l2tp

    printf "$USER * $PASSWORD" >> $ppp/chap-secrets
    printf "$USER * $PASSWORD" >> $ppp/pap-secrets

    $LOG "Starting VPN network l2tp..."
    $LOG "Start xl2tpd"
    xl2tpd -c /etc/ppp/l2tpd.conf -s /etc/ppp/chap-secrets -p /var/lock/l2tpd.pid &
