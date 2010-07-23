#!/bin/sh

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

echo "==================START-PPTP-CLIENT======================="

    $LOG "Check for PPTP server reachable"
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
    ROUTE=`ip route get $SERVER | grep dev | cut -f -3 -d " "`
    if [ "$ROUTE" != "" ] || [ "$ROUTE" != "0.0.0.0" ]; then
	$LOG "Add route to vpn server."
	ip route add $ROUTE
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

    $LOG "PPTP connect to $SERVER ....."
    $LOG "Start pppd"
    PPPDOPT="file /etc/ppp/options.pptp -detach $DEBUG $MTU $MRU $MPPE plugin"
    PLUGOPT="/lib/pptp.so allow-mppe-128 pptp_server $SERVER call pptp persist $PEERDNS user $USER password $PASSWORD"
    FULLOPT="$PPPDOPT $PLUGOPT"
    pppd $FULLOPT &
