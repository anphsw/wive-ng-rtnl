#!/bin/sh

#################################################
# config-pptp.sh - configure PPTP client helper #
#################################################

# include global
. /etc/scripts/global.sh

# stop all pppd/xl2tpd daemons
killall_vpn

LOG="logger -t vpnhelper-pptp"

get_param() {
    eval `nvram_buf_get 2860 vpnServer vpnUser vpnPassword vpnMTU vpnMPPE vpnPeerDNS vpnDebug vpnAuthProtocol vpnEnableLCP \
	    vpnLCPFailure vpnLCPInterval vpnTestReachable wan_gateway`
    OPTFILE="/etc/ppp/options.pptp"
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
    if [ ! -d /sys/module/pptp ]; then
	mod="ppp_generic pppox pptp"
	for module in $mod
	do
	    modprobe -q $module
	done
    fi
}

echo "==================START-PPTP-CLIENT======================="
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
	newdgw="$wan_gateway"
	if [ "$newdgw" = "0.0.0.0" ]; then
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

    if [ "$vpnMTU" = "" ] || [ "$vpnMTU" = "AUTO" ]; then
        vpnMTU=""
        vpnMRU=""
    else
        vpnMRU="mru $vpnMTU"
        vpnMTU="mtu $vpnMTU"
    fi

    if [ "$vpnDebug" = "on" ]; then
        vpnDebug="debug"
    else
	vpnDebug=""
    fi

    if [ "$vpnAuthProtol" = "1" ]; then
	PAP="require-pap"
	CHAP="refuse-chap"
    elif [ "$vpnAuthProtol" = "2" ]; then
	PAP="refuse-pap"
	CHAP="require-chap"
    elif [ "$vpnAuthProtol" = "3" ]; then
	PAP="refuse-pap"
	CHAP="refuse-chap"
    else
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

    cp -f /etc/ppp/options.template $OPTFILE
    printf "
    nomp
    lcp-echo-failure  $vpnLCPFailure
    lcp-echo-interval $vpnLCPInterval
    $vpnEnableLCP
    $PAP
    $CHAP
    " >> $OPTFILE

    $LOG "PPTP connect to $SERVER ....."
    $LOG "Start pppd"
    PPPDOPT="file $OPTFILE ifname $vpn_if -detach $vpnDebug $vpnMTU $vpnMRU $vpnMPPE plugin"
    PLUGOPT="/lib/pptp.so pptp_server $SERVER call pptp persist $vpnPeerDNS user $vpnUser password $vpnPassword"
    FULLOPT="$PPPDOPT $PLUGOPT"
    pppd $FULLOPT &
