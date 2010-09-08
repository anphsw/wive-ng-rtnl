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
OPTFILE="/etc/ppp/options.pptp"

killall -q pppd > /dev/null 2>&1
killall -q xl2tpd > /dev/null 2>&1

modprobe -q ppp_generic > /dev/null 2>&1
modprobe -q pppox > /dev/null 2>&1
modprobe -q pptp > /dev/null 2>&1

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
    else
        SERVER=$SERVERNM
        $LOG "Not resolve adress for $SERVER"
    fi
}

echo "==================START-PPTP-CLIENT======================="
    check_param
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

    #store server IP
    nvram_set 2860 vpnServerIP $SERVER

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

    cp -f /etc/ppp/options.template $OPTFILE
    printf "                                                                                                                                                                                                                                                                        
    $PAP
    $CHAP
    " >> $OPTFILE

    $LOG "PPTP connect to $SERVER ....."
    $LOG "Start pppd"
    PPPDOPT="file $OPTFILE -detach $DEBUG $MTU $MRU $MPPE plugin"
    PLUGOPT="/lib/pptp.so pptp_server $SERVER call pptp persist $PEERDNS user $USER password $PASSWORD"
    FULLOPT="$PPPDOPT $PLUGOPT"
    pppd $FULLOPT &
