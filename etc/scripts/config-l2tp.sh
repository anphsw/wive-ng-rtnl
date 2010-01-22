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

killall -9 pppd > /dev/null 2>&1
killall -9 xl2tpd > /dev/null 2>&1

echo "Add static route to vpn server"
ADDRESS=`nslookup "$SERVER" | grep Address | tail -n1 | cut -c 12- | awk {' print $1 '}`
    if [ "$ADDRESS" != "" ]; then
        SERVER=$ADDRESS
      else
        SERVER=$SERVER
    fi

    ROUTE=`ip r get "$SERVER" | grep dev | cut -f -3 -d " "`
    ip r add $ROUTE

    echo "Remove default route"
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

    echo "Starting VPN network l2tp..."
    xl2tpd -c /etc/ppp/l2tpd.conf -s /etc/ppp/chap-secrets -p /var/lock/l2tpd.pid &
