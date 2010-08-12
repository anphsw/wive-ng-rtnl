#!/bin/sh

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

modprobe ppp_generic > /dev/null 2>&1
modprobe pppox > /dev/null 2>&1
modprobe pptp > /dev/null 2>&1

LOG="logger -t vpnhelper"

echo "==================START-PPTP-CLIENT======================="
    get_vpn_ip
    reachable=0;
    while [ $reachable -eq 0 ]; do
	$LOG "Check for PPTP server $SERVER reachable"
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
    ROUTE=`ip route get $SERVER | grep dev | cut -f -3 -d " "`
    if [ "$ROUTE" != "" ]; then
	$LOG "Add route to vpn server $ROUTE"
	ip route replace $ROUTE
    fi
    if [ -f /etc/default.gw ]; then
	newdgw="via `cat /etc/default.gw`"
    else
	newdgw=""
    fi
    DEV="eth2.2"
    if [ "$opmode" = "0" ]; then
	    DEV="br0"
        elif [ "$opmode" = "1" ]; then
	    DEV="br0"
        elif [ "$opmode" = "2" ]; then
	    DEV="ra0"
        elif [ "$opmode" = "3" ]; then
	    DEV="apcli0"
    fi
    $LOG "Add route to $SERVER $newdgw over $DEV"
    ip route replace $SERVER dev $DEV $newdgw

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

get_vpn_ip() {
    $LOG "Get vpn server ip adress"
    NS=`ipget $SERVERNM | tail -n1`
    if [ "$NS" != "" ]; then
        SERVER=$NS
        $LOG "Server adress is $ADDRESS"
    else
        SERVER=$SERVERNM
        $LOG "Not resolve adress for $SERVER"
    fi
}
