#!/bin/sh

#################################################
# config-l2tp.sh - configure L2TP client helper #
#################################################

# include global config
. /etc/scripts/global.sh

# static interface name
IFNAME="ppp0"

#correct terminate xl2tpd daemon
if [ "`pidof xl2tpd`" ]; then
    #Kill daemons
    killall -q xl2tpd
    sleep 2
    killall -q -SIGKILL xl2tpd
fi
if [ "`pidof pppd`" ]; then
    # send hup signal to pppd for correct link down
    killall -q -SIGHUP pppd
    sleep 3
    # Kill daemons
    killall -q pppd
    killall -q -SIGKILL pppd
fi

LOG="logger -t vpnhelper-l2tp"

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
    PINGTST=`nvram_get 2860 vpnTestReachable`
}

check_param() {
    if [ "$SERVERNM" = "" ] || [ "$USER" = "" ] || [ "$PASSWORD" = "" ]; then
	$LOG "Server adress, username or password not set. Exit..."
	exit 1
    fi
}

get_vpn_ip() {
    $LOG "Get vpn server $SERVERNM ip adress"
    NS=`ipget $SERVERNM | tail -q -n1`
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
    if [ ! -d /sys/module/pppol2tp ]; then
	mod="ppp_generic pppox pppol2tp"
	for module in $mod
	do
    	    modprobe -q $module
	done
    fi
}

echo "==================START-L2TP-CLIENT======================="
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

    if [ "$vpnTestReachable" = "1" ]; then
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
    fi

    # load ppp* modules
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

    if [ "$DEBUG" = "on" ]; then
        DEBUG="-D"
    else
        DEBUG=""
    fi

    if [ "$MTU" = "" ] || [ "$MTU" = "AUTO" ]; then
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

    if [ "$LCPECHO" = "on" ]; then
        LCPECHO="lcp-echo-adaptive"
    else
	LCPECHO=""
    fi

    if [ "$LCPFAIL" = "" ] || [ "$LCPINTR" = "" ]; then
	LCPFAIL=5
	LCPINTR=30
    fi

    # clear all configs
    ppp=/etc/ppp
    echo > $ppp/l2tpd.conf
    echo > $ppp/options.l2tp

    printf "[global]
    access control = yes
    rand source = dev
    [lac $SERVER]
    redial = yes
    redial timeout = 20
    $L2TPPAP
    $L2TPCHAP
    require authentication = no
    lns = $SERVER
    name = $USER
    autodial = yes
    tx bps = 100000000
    rx bps = 100000000
    tunnel rws = 8
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
    nomp
    noproxyarp
    $MTU
    $MRU
    $MPPE
    $PEERDNS
    lcp-echo-failure  $LCPFAIL
    lcp-echo-interval $LCPINTR
    $LCPECHO
    ifname $IFNAME
    " >> $ppp/options.l2tp

    printf "$USER * $PASSWORD" >> $ppp/chap-secrets
    printf "$USER * $PASSWORD" >> $ppp/pap-secrets

    $LOG "Starting VPN network l2tp..."
    $LOG "Start xl2tpd"
    FULLOPTS="$DEBUG -c /etc/ppp/l2tpd.conf -s /etc/ppp/chap-secrets -p /var/lock/l2tpd.pid"
    xl2tpd $FULLOPTS &
