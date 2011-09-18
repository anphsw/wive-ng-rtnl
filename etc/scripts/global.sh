#!/bin/sh

############################################################
# global.sh - correct enviroment helper for automatization #
############################################################

#include kernel config
. /etc/scripts/config.sh

# set default variables
wan_if="eth2.2"
wan_upnp_if="eth2.2"
lan_if="br0"
lan2_if="br0:9"

#first get operation mode and wan mode
opmode=`nvram_get 2860 OperationMode`
wanmode=`nvram_get 2860 wanConnectionMode`

#get vpn mode and type
vpnEnabled=`nvram_get 2860 vpnEnabled`
vpnType=`nvram_get 2860 vpnType`

#get wireless, wan and lan mac adresses
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
    #real wan name
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

    #upnp wan name
    if [ "$vpnEnabled" = "on" ]; then
        wan_upnp_if="ppp0"
    else
        wan_upnp_if=$wan_if
    fi
}

#wait connect to AP
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

#configure and start dhcp client
udhcpc_opts()
{
    CL_SLEEP=1
    if [ "$opmode" = "0" ] || [ "$opmode" = "2" ]; then
	CL_SLEEP=5
	#disable dhcp renew from driver
	sysctl -w net.ipv4.send_sigusr_dhcpc=9
    else
	ForceRenewDHCP=`nvram_get 2860 ForceRenewDHCP`
	wan_port=`nvram_get 2860 wan_port`
	if [ "$ForceRenewDHCP" != "0" ] && [ "$wan_port" != "" ]; then
	    #configure event wait port
	    sysctl -w net.ipv4.send_sigusr_dhcpc=$wan_port
	else
	    #disable dhcp renew from driver
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

#select switch type from config
getSwType()
{
    if [ "$CONFIG_RAETH_ROUTER" != "" ]; then
	#VIA external switch
	SWITCH_MODE=0
    elif [ "$CONFIG_MAC_TO_MAC_MODE" != "" ]; then
	#VTSS external switch
	SWITCH_MODE=1
    elif [ "$CONFIG_RT_3052_ESW" != "" ]; then
	#internal 3052 ESW
	SWITCH_MODE=2
    else
	#default internal switch
	SWITCH_MODE=2
    fi
}

#configure LAN/WAN switch particion
setLanWan()
{
getSwType
if [ "$SWITCH_MODE" = "2" ]; then
    if [ ! -f /var/run/goahead.pid ]; then
	##################################################
	# workaroud for dir-300NRU and some devices
	# with not correct configured from uboot
	# need only start boot
	##################################################
	echo "Reinit power mode for all switch ports"
	config-vlan.sh $SWITCH_MODE FFFFF > /dev/null 2>&1
    fi
    ##################################################
    echo '######## clear switch partition  ########'
    config-vlan.sh $SWITCH_MODE 0 > /dev/null 2>&1
    ##################################################
    echo '######## clear switch mac table  ########'
    switch clear > /dev/null 2>&1
    ##################################################
    # In gate mode and hotspot mode configure vlans
    ##################################################
    if [ "$opmode" = "1" ] || [ "$opmode" = "4" ]; then
	wan_port=`nvram_get 2860 wan_port`
	tv_port=`nvram_get 2860 tv_port`
	if [ "$wan_port" = "0" ]; then
	    if [ "$tv_port" = "1" ]; then
		echo '##### config vlan partition (WWLLL) #####'
		config-vlan.sh $SWITCH_MODE WWLLL
	    else
		echo '##### config vlan partition (WLLLL) #####'
		config-vlan.sh $SWITCH_MODE WLLLL
	    fi
	else
	    if [ "$tv_port" = "1" ]; then
		echo '##### config vlan partition (LLLWW) #####'
		config-vlan.sh $SWITCH_MODE LLLWW
	    else
		echo '##### config vlan partition (LLLLW) #####'
		config-vlan.sh $SWITCH_MODE LLLLW
	    fi
	fi
    fi
    ##################################################
    # Set speed and duplex modes per port
    ##################################################
    phys_portN=4
    for i in `seq 1 5`; do
	port_swmode=`nvram_get 2860 port"$i"_swmode`
	if [ "$port_swmode" != "auto" ] && [ "$port_swmode" != "" ]; then
	    echo ">>> Port $phys_portN set mode $port_swmode <<<"
	    if [ "$port_swmode" = "100f" ]; then
		mii_mgr -s -p$phys_portN -r0 -v 0x2100
	    elif [ "$port_swmode" = "100h" ]; then
		mii_mgr -s -p$phys_portN -r0 -v 0x2000
	    elif [ "$port_swmode" = "10f" ]; then
		mii_mgr -s -p$phys_portN -r0 -v 0x0100
	    elif [ "$port_swmode" = "10h" ]; then
		mii_mgr -s -p$phys_portN -r0 -v 0x0000
	    fi
	fi
	let "phys_portN=$phys_portN-1"
    done
fi
}

vpn_deadloop_fix()
{
    #L2TP and PPTP kernel dead-loop fix
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

get_txqlen()
{
    #for memory save 16m device uses txqueuelen=100
    if [ -f /tmp/is_16ram_dev ]; then
	txqueuelen="100"
    else
	txqueuelen="1000"
    fi
}

zero_conf()
{
    vpnPurePPPOE=`nvram_get 2860 vpnPurePPPOE`
    wan_is_not_null=`ip -4 addr show $wan_if | grep inet -c`
    if [ "$wan_is_not_null" = "0" ]; then
	$LOG "Call zeroconf for get wan ip address."
	killall -q zcip
	killall -q -SIGKILL zcip
	zcip $wan_if /etc/scripts/zcip.script > /dev/null 2>&1
	$LOG "Wait zeroconf for get wan ip address."
	sleep 10
    fi
}

drop_disk_caches(){
    echo "Drop caches"
    for i in `seq 3 0`; do
        echo $i > /proc/sys/vm/drop_caches
    done
    sync
}

# get params
getLanIfName
getWanIfName
