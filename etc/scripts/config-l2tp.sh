#!/bin/sh

#################################################
# config-l2tp.sh - configure L2TP client helper #
#################################################

# include global config
. /etc/scripts/global.sh

# static interface name
IFNAME="ppp0"

# stop all pppd/xl2tpd daemons
killall_vpn

LOG="logger -t vpnhelper-l2tp"

get_param() {
    eval `nvram_buf_get 2860 vpnServer vpnUser vpnPassword vpnMTU vpnMPPE vpnPeerDNS vpnDebug vpnAuthProtocol vpnEnableLCP \
	    vpnLCPFailure vpnLCPInterval vpnTestReachable`
}

check_param() {
    if [ "$vpnServer" = "" ] || [ "$vpnUser" = "" ] || [ "$vpnPassword" = "" ]; then
	$LOG "Server adress, username or password not set. Exit..."
	exit 1
    fi
}

get_vpn_ip() {
    $LOG "Get vpn server $vpnServer ip adress"
    NS=`ipget $vpnServer | tail -q -n1`
    if [ "$NS" != "" ]; then
        SERVER=$NS
        $LOG "Server adress is $SERVER"
	echo "$SERVER" > /tmp/vpnip
    else
        SERVER=$vpnServer
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
    if [ "$wanConnectionMode" != "STATIC" ]; then
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

    if [ "$vpnPeerDNS" = "on" ]; then
	vpnPeerDNS=usepeerdns
    else
	vpnPeerDNS=
    fi

    if [ "$vpnMPPE" = "on" ]; then
	mod="crypto_algapi cryptomgr blkcipher ppp_mppe"
	for module in $mod
	do
	    modprobe -q $module
	done
        vpnMPPE=allow-mppe-128
    else
        vpnMPPE=
    fi

    if [ "$vpnDebug" = "on" ]; then
        vpnDebug="-D"
    else
        vpnDebug=""
    fi

    if [ "$vpnMTU" = "" ] || [ "$vpnMTU" = "AUTO" ]; then
        vpnMTU=""
        vpnMRU=""
        else
        vpnMRU="mru $vpnMTU"
        vpnMTU="mtu $vpnMTU"
    fi

    if [ "$vpnAuthProtol" = "1" ]; then
	L2TPPAP="require pap = yes"
	L2TPCHAP="require chap = no"
	PAP="require-pap"
	CHAP="refuse-chap"
    elif [ "$vpnAuthProtol" = "2" ]; then
	L2TPPAP="require pap = no"
	L2TPCHAP="require chap = yes"
	PAP="refuse-pap"
	CHAP="require-chap"
    elif [ "$vpnAuthProtol" = "3" ]; then
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

    if [ "$vpnEnableLCP" = "on" ]; then
        vpnEnableLCP="lcp-echo-adaptive"
    else
	vpnEnableLCP=""
    fi

    if [ "$vpnLCPFailure" = "" ] || [ "$vpnLCPInterval" = "" ]; then
	vpnLCPFailure=5
	vpnLCPInterval=30
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
    name = $vpnUser
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
    $vpnMTU
    $vpnMRU
    $vpnMPPE
    $vpnPeerDNS
    lcp-echo-failure  $vpnLCPFailure
    lcp-echo-interval $vpnLCPInterval
    $vpnEnableLCP
    ifname $IFNAME
    " >> $ppp/options.l2tp

    printf "$vpnUser * $vpnPassword" >> $ppp/chap-secrets
    printf "$vpnUser * $vpnPassword" >> $ppp/pap-secrets

    $LOG "Starting VPN network l2tp..."
    $LOG "Start xl2tpd"
    FULLOPTS="$vpnDebug -c /etc/ppp/l2tpd.conf -s /etc/ppp/chap-secrets -p /var/lock/l2tpd.pid"
    xl2tpd $FULLOPTS &
