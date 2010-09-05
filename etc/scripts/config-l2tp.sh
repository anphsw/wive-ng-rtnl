#!/bin/sh

#get params
. /sbin/global.sh

SERVERNM=`nvram_get 2860 vpnServer`
USER=`nvram_get 2860 vpnUser`
PASSWORD=`nvram_get 2860 vpnPassword`
MTU=`nvram_get 2860 vpnMTU`
MPPE=`nvram_get 2860 vpnMPPE`
PEERDNS=`nvram_get 2860 vpnPeerDNS`
DEBUG=`nvram_get 2860 vpnDebug`
opmode=`nvram_get 2860 OperationMode`

killall -q pppd > /dev/null 2>&1
killall -q xl2tpd > /dev/null 2>&1

modprobe -q ppp_generic > /dev/null 2>&1
modprobe -q pppox > /dev/null 2>&1
modprobe -q pppol2tp > /dev/null 2>&1

LOG="logger -t vpnhelper"

get_vpn_ip() {
    $LOG "Get vpn server $SERVERNM ip adress"
    NS=`ipget $SERVERNM | tail -n1`
    if [ "$NS" != "" ]; then
        SERVER=$NS
        $LOG "Server adress is $SERVER"
    else
        SERVER=$SERVERNM
        $LOG "Not resolve adress for $SERVER"
    fi
}

echo "==================START-L2TP-CLIENT======================="
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
    
    #store server IP
    nvram_set 2860 vpnServerIP $SERVER

    $LOG "Get route to vpn server."
    if [ -f /etc/default.gw ]; then
	newdgw=`cat /etc/default.gw`
    else
	newdgw=""
    fi

    $LOG "Add route to $SERVER via $newdgw over $wan_if"
    if [ "$newdgw" != "" ] && [ "$newdgw" != "$SERVER" ]; then
	ip route replace $SERVER via $newdgw metric 0
    else
	ip route replace $SERVER dev $wan_if metric 0
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

    #clear all configs
    ppp=/etc/ppp
    echo > $ppp/l2tpd.conf
    echo > $ppp/options.l2tp

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
