#!/bin/sh

#################################################
# config-pptp.sh - configure PPTP client helper #
#################################################

#include global
. /etc/scripts/global.sh

killall -q pppd
killall -q xl2tpd

LOG="logger -t vpnhelper-pptp"

get_param() {
    SERVERNM=`nvram_get 2860 vpnServer`
    USER=`nvram_get 2860 vpnUser`
    PASSWORD=`nvram_get 2860 vpnPassword`
    MTU=`nvram_get 2860 vpnMTU`
    MPPE=`nvram_get 2860 vpnMPPE`
    PEERDNS=`nvram_get 2860 vpnPeerDNS`
    DEBUG=`nvram_get 2860 vpnDebug`
    AUTHMODE=`nvram_get 2860 vpnAuthProtocol`
    LCPECHO=`nvram_get 2860 vpnEnableLCP`
    LCPFAIL=`nvram_get 2860 vpnLCPFailure`
    LCPINTR=`nvram_get 2860 vpnLCPInterval`
    OPTFILE="/etc/ppp/options.pptp"
}

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
	echo "$SERVER" > /tmp/vpnip
    else
        SERVER=$SERVERNM
        $LOG "Not resolve adress for $SERVER"
    fi
}

load_modules() {
    mod="ppp_generic pppox pptp"
    for module in $mod
    do
	modprobe -q $module
    done
}

echo "==================START-PPTP-CLIENT======================="
    get_param
    check_param
    get_vpn_ip
    reachable=0;

    $LOG "Set route to vpn server."
    if [ "$wanmode" != "STATIC" ]; then
	if [ -f /tmp/default.gw ]; then
	    newdgw=`cat /tmp/default.gw`
	else
	    newdgw=""
	fi
    else
	newdgw=`nvram_get 2860 wan_gateway`
	if [ $newdgw = "0.0.0.0" ]; then
	    newdgw=""
	fi
    fi

    if [ "$newdgw" != "" ] && [ "$newdgw" != "$SERVER" ]; then
	dgw_net=`ipcalc "$newdgw" -n | cut -f 2- -d =`
	srv_net=`ipcalc "$SERVER" -n | cut -f 2- -d =`
	if [ "$dgw_net" != "$srv_net" ]; then
	    $LOG "Add route to $SERVER via $newdgw"
	    ip route replace $SERVER via $newdgw
	fi
    fi

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

    #load ppp* modules
    load_modules

    if [ "$PEERDNS" = "on" ]; then
	PEERDNS=usepeerdns
    else
	PEERDNS=
    fi

    if [ "$MPPE" = "on" ]; then
	mod="crypto_algapi cryptomgr blkcipher ppp_mppe"
	for module in $mod
	do
	    modprobe -q $module
	done
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

    if [ "$LCPFAIL" = "" ] || [ "$LCPINTR" = "" ]; then
	LCPFAIL=5
	LCPINTR=30
    fi

    cp -f /etc/ppp/options.template $OPTFILE
    printf "
    nomp
    lcp-echo-failure  $LCPFAIL
    lcp-echo-interval $LCPINTR
    $LCPECHO
    $PAP
    $CHAP
    " >> $OPTFILE

    $LOG "PPTP connect to $SERVER ....."
    $LOG "Start pppd"
    PPPDOPT="file $OPTFILE -detach $DEBUG $MTU $MRU $MPPE plugin"
    PLUGOPT="/lib/pptp.so pptp_server $SERVER call pptp persist $PEERDNS user $USER password $PASSWORD"
    FULLOPT="$PPPDOPT $PLUGOPT"
    pppd $FULLOPT &
