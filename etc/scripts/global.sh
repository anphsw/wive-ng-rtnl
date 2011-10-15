#!/bin/sh

############################################################
# global.sh - correct enviroment helper for automatization #
############################################################

# include kernel config
. /etc/scripts/config.sh

# include profile variables
. /etc/profile

# set default variables
wan_if="eth2.2"
real_wan_if="eth2.2"
lan_if="br0"
lan2_if="br0:9"

# first get operation mode and wan mode
opmode=`nvram_get 2860 OperationMode`
wanmode=`nvram_get 2860 wanConnectionMode`

# get dns mode and relay mode
dnsPEnabled=`nvram_get 2860 dnsPEnabled`
wan_static_dns=`nvram_get 2860 wan_static_dns`

# get vpn mode and type
vpnEnabled=`nvram_get 2860 vpnEnabled`
vpnType=`nvram_get 2860 vpnType`

# get wireless, wan and lan mac adresses
getMacIf()
{
    WMAC=`nvram_get 2860 WLAN_MAC_ADDR`
    WANMAC=`nvram_get 2860 WAN_MAC_ADDR`
    LANMAC=`nvram_get 2860 LAN_MAC_ADDR`
}

# LAN interface name -> $lan_if
getLanIfName()
{
    if [ "$opmode" = "2" ]; then
	lan_if="eth2"
	lan2_if="eth2:9"
    else
	lan_if="br0"
	lan2_if="br0:9"
    fi
}

# WAN interface name -> $wan_if
getWanIfName()
{
    # real wan name
    if [ "$opmode" = "0" ]; then
	wan_if="br0"
    elif [ "$opmode" = "1" ] || [ "$opmode" = "4" ]; then
	wan_if="eth2.2"
    elif [ "$opmode" = "2" ]; then
	wan_if="ra0"
    elif [ "$opmode" = "3" ]; then
	if [ "$CONFIG_RT2860V2_AP_APCLI" != "" ]; then
	    wan_if="apcli0"
	else
	    echo "Driver not support APCLI mode."
	    wan_if="eth2.2"
	fi
    elif [ "$opmode" = "4" ]; then
	    wan_if="eth2.2"
    fi

    # upnp wan name
    if [ "$vpnEnabled" = "on" ]; then
	get_wan_if=`ls /proc/sys/net/ipv4/conf/ | grep ppp | tail -q -n1`
	if [ "$get_ppp_wan_if" != "" ]; then
    	    real_wan_if="$get_ppp_wan_if"
	else
    	    real_wan_if="ppp0"
	fi
    else
        real_wan_if=$wan_if
    fi
}

getWanIpaddr()
{
    # always return physical wan ip
    wan_ipaddr=`nvram_get 2860 wan_ipaddr`
    if [ "$wanmode" != "STATIC" ] || [ "$wan_ipaddr" = "" ]; then
	wan_ipaddr=`LC_ALL=C /bin/ifconfig $wan_if 2>&1 | grep 'inet addr' | awk '{print $2}' | sed -e 's/.*://'`
    fi

    # return vpn or physical wan ip
    real_wan_ipaddr=`LC_ALL=C /bin/ifconfig $real_wan_if 2>&1 | grep 'inet addr' | awk '{print $2}' | sed -e 's/.*://'`
    if [ "$real_wan_ipaddr" = "" ]; then
	real_wan_ipaddr=wan_ipaddr
    fi

}

# for memory save 16m device uses txqueuelen=100
get_txqlen()
{
    if [ -f /tmp/is_16ram_dev ]; then
	txqueuelen="100"
    else
	txqueuelen="1000"
    fi
}


# free memory
drop_disk_caches(){
    echo "Drop caches"
    for i in `seq 3 0`; do
        echo $i > /proc/sys/vm/drop_caches
    done
    sync
}

# wait connect to AP
wait_connect()
{
    if [ "$opmode" = "2" ]; then
	connected=`iwpriv ra0 connStatus | grep Connected -c`
	if [ "$connected" = "0" ] || [ ! -f /tmp/sta_connected ]; then
	    staCur_SSID=""
    	    exit 0
	fi
	staCur_SSID=`iwpriv ra0 connStatus | grep ra0 | awk ' { print $3 } ' | cut -f1 -d[`
    fi
}

# L2TP and PPTP kernel dead-loop fix
vpn_deadloop_fix()
{
    if [ "$vpnEnabled" = "on" ]; then
	if [ "$vpnType" != "0" ] || [ "$opmode" = "2" ]; then
	    # First vpn stop.. 
	    # Auto start later renew/bound
	    service vpnhelper stop > /dev/null 2>&1
	    ip route flush cache > /dev/null 2>&1
	    echo 1 > /proc/sys/net/nf_conntrack_flush
	fi
    fi
}

# configure and start dhcp client
udhcpc_opts()
{
    CL_SLEEP=1
    if [ "$opmode" = "0" ] || [ "$opmode" = "2" ]; then
	CL_SLEEP=5
	# disable dhcp renew from driver
	sysctl -w net.ipv4.send_sigusr_dhcpc=9
    else
	ForceRenewDHCP=`nvram_get 2860 ForceRenewDHCP`
	wan_port=`nvram_get 2860 wan_port`
	if [ "$ForceRenewDHCP" != "0" ] && [ "$wan_port" != "" ]; then
	    # configure event wait port
	    sysctl -w net.ipv4.send_sigusr_dhcpc=$wan_port
	else
	    # disable dhcp renew from driver
	    sysctl -w net.ipv4.send_sigusr_dhcpc=9
	fi

    fi
    dhcpRequestIP=`nvram_get 2860 dhcpRequestIP`
    if [ "$dhcpRequestIP" != "" ]; then
	REQIP="-r $dhcpRequestIP"
    else
	REQIP=""
    fi
    wan_manual_mtu=`nvram_get 2860 wan_manual_mtu`
    if [ "$wan_manual_mtu" = "0" ]; then
	GETMTU="-O mtu"
    else
	GETMTU=""
    fi
    HostName=`nvram_get 2860 HostName`
    UDHCPCOPTS="-i $wan_if -H $HostName $REQIP -S -R -T 5 -a \
		-s /bin/udhcpc.sh -p /var/run/udhcpc.pid \
		-O routes -O staticroutes -O msstaticroutes $GETMTU -f &"
}

# configure and start zeroconf daemon
zero_conf()
{
    vpnPurePPPOE=`nvram_get 2860 vpnPurePPPOE`
    wan_is_not_null=`ip -4 addr show $wan_if | grep inet -c`
    if [ "$wan_is_not_null" = "0" ]; then
	killall -q zcip
	killall -q -SIGKILL zcip
	zcip $wan_if /etc/scripts/zcip.script > /dev/null 2>&1
	sleep 10
    fi
}

# configure LAN/WAN switch particion and mode per port
setLanWan()
{
##############################################################################
# Internal 3052 ESW
##############################################################################
if [ "$CONFIG_RT_3052_ESW" != "" ]; then
    SWITCH_MODE=2
    if [ ! -f /var/run/goahead.pid ]; then
	######################################################################
	# workaroud for dir-300NRU and some devices
	# with not correct configured from uboot
	# need only start boot
	######################################################################
	echo "Reinit power mode for all switch ports"
	config-vlan.sh $SWITCH_MODE FFFFF > /dev/null 2>&1
    fi
    ##########################################################################
    echo '######## clear switch partition  ########'
    config-vlan.sh $SWITCH_MODE 0 > /dev/null 2>&1
    ##########################################################################
    echo '######## clear switch mac table  ########'
    switch clear > /dev/null 2>&1
    ##########################################################################
    # In gate mode and hotspot mode configure vlans
    ##########################################################################
    if [ "$opmode" = "1" ] || [ "$opmode" = "4" ]; then
	wan_port=`nvram_get 2860 wan_port`
	tv_port=`nvram_get 2860 tv_port`
	if [ "$wan_port" = "0" ]; then
	    if [ "$tv_port" = "1" ]; then
		echo '##### ESW config vlan partition (WWLLL) #####'
		config-vlan.sh $SWITCH_MODE WWLLL > /dev/null 2>&1
	    else
		echo '##### ESW config vlan partition (WLLLL) #####'
		config-vlan.sh $SWITCH_MODE WLLLL > /dev/null 2>&1
	    fi
	else
	    if [ "$tv_port" = "1" ]; then
		echo '##### ESW config vlan partition (LLLWW) #####'
		config-vlan.sh $SWITCH_MODE LLLWW > /dev/null 2>&1
	    else
		echo '##### ESW config vlan partition (LLLLW) #####'
		config-vlan.sh $SWITCH_MODE LLLLW > /dev/null 2>&1
	    fi
	fi
    fi
    ##########################################################################
    # Set speed and duplex modes per port
    ##########################################################################
    if [ -f /bin/mii_mgr ]; then
	phys_portN=4
	for i in `seq 1 5`; do
	    port_swmode=`nvram_get 2860 port"$i"_swmode`
	    if [ "$port_swmode" != "auto" ] && [ "$port_swmode" != "" ]; then
		echo ">>> Port $phys_portN set mode $port_swmode <<<"
		if [ "$port_swmode" = "100f" ]; then
		    mii_mgr -s -p$phys_portN -r0 -v 0x2100 > /dev/null 2>&1
		elif [ "$port_swmode" = "100h" ]; then
		    mii_mgr -s -p$phys_portN -r0 -v 0x2000 > /dev/null 2>&1
		elif [ "$port_swmode" = "10f" ]; then
		    mii_mgr -s -p$phys_portN -r0 -v 0x0100 > /dev/null 2>&1
		elif [ "$port_swmode" = "10h" ]; then
		    mii_mgr -s -p$phys_portN -r0 -v 0x0000 > /dev/null 2>&1
		fi
	    fi
	    let "phys_portN=$phys_portN-1"
	done
    fi
##############################################################################
# VTSS external switch
##############################################################################
elif [ "$CONFIG_MAC_TO_MAC_MODE" != "" ]; then
    SWITCH_MODE=1
    ##########################################################################
    echo '######## clear switch partition  ########'
    config-vlan.sh $SWITCH_MODE 0 > /dev/null 2>&1
    echo '##### config vlan partition (VTSS) #####'
    config-vlan.sh $SWITCH_MODE 1 > /dev/null 2>&1
##############################################################################
# IC+ external switch
##############################################################################
elif [ "$CONFIG_RAETH_ROUTER" != "" ]; then
    SWITCH_MODE=0
    ##########################################################################
    echo '######## clear switch partition  ########'
    config-vlan.sh $SWITCH_MODE 0 > /dev/null 2>&1
    ##########################################################################
    # In gate mode and hotspot mode configure vlans
    ##########################################################################
    if [ "$opmode" = "1" ] || [ "$opmode" = "4" ]; then
	wan_port=`nvram_get 2860 wan_port`
	if [ "$wan_port" = "0" ]; then
	    echo '##### IC+ config vlan partition (WLLLL) #####'
	    config-vlan.sh $SWITCH_MODE WLLLL > /dev/null 2>&1
	else
	    echo '##### IC+ config vlan partition (LLLLW) #####'
	    config-vlan.sh $SWITCH_MODE LLLLW > /dev/null 2>&1
	fi
    fi
fi
}

killall_vpn()
{
    #correct terminate xl2tpd daemon
    if [ "`pidof xl2tpd`" ]; then
	#Kill daemons
	killall -q xl2tpd
	sleep 2
	killall -q -SIGKILL xl2tpd
    fi

    # first send HUP for terminate connections and try some times
    # second send TERM for exit pppd process
    # if process not terminated send KILL
    count=0
    while killall -q -SIGHUP pppd; do
	if [ "$count" = "3" ]; then
	    killall -q pppd
	    sleep 2
	    count=0
	fi
	if [ "$count" = "5" ]; then
	    killall -q -SIGKILL pppd
	    sleep 3
	    count=0
	fi
	sleep 2
	count="$(($count+1))"
    done

    # Remove VPN server IP file
    rm -f /tmp/vpnip
}

# get params
getLanIfName
getWanIfName
getWanIpaddr
