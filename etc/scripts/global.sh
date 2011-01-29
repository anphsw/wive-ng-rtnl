#!/bin/sh

############################################################
# global.sh - correct enviroment helper for automatization #
############################################################

web_wait()
{
#wait to start web and run from goahead code
    if [ ! -f /var/run/goahead.pid ]; then
      exit 0
    fi
}

#include kernel config
. /etc/scripts/config.sh

#first get operation mode
opmode=`nvram_get 2860 OperationMode`

#get wan mode
wanmode=`nvram_get 2860 wanConnectionMode`

#get vpn mode
vpnEnabled=`nvram_get 2860 vpnEnabled`

#get current config wan port
wan_port=`nvram_get 2860 wan_port`

#get wireless, wan and lan mac adresses
WMAC=`nvram_get 2860 WLAN_MAC_ADDR`
WANMAC=`nvram_get 2860 WAN_MAC_ADDR`
LANMAC=`nvram_get 2860 LAN_MAC_ADDR`


# WAN interface name -> $wan_if
getWanIfName()
{
    if [ "$opmode" = "0" ]; then
	wan_if="br0"
    elif [ "$opmode" = "1" ]; then
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
    fi
}

getWanPppIfName()
{
    if [ "$vpnEnabled" = "on"  ]; then
	wan_ppp_if="ppp0"
    else
	wan_ppp_if=$wan_if
    fi
}

getWanUpnpIfName()
{
    if [ "$vpnEnabled" = "on" ]; then
        wan_upnp_if="ppp0"
    else
        wan_upnp_if=$wan_if
    fi
}

# LAN interface name -> $lan_if
getLanIfName()
{
    if [ "$opmode" = "2" ]; then
	lan_if="eth2"
    else
	lan_if="br0"
    fi
}

# LAN interface name -> $lan_if
getLan2IfName()
{
    if [ "$opmode" = "2" ]; then
	lan2_if="eth2:9"
    else
	lan2_if="br0:9"
    fi
}

# ethernet converter enabled -> $ethconv "y"
getEthConv()
{
    ec=`nvram_get 2860 ethConvert`
    if [ "$opmode" = "0" ] && [ "$ec" = "1" ]; then
	ethconv="y"
    else
	ethconv="n"
    fi
}

# station driver loaded -> $stamode "y"
getStaMode()
{
    if [ "$opmode" = "2" ] || [ "$ethconv" = "y" ]; then
	stamode="y"
    else
	stamode="n"
    fi
}

#select switch type from config
getSwType()
{
    if [ "$CONFIG_RAETH_ROUTER" = "y" ]; then
	#VIA external switch
	SWITCH_MODE=0
    elif [ "$CONFIG_MAC_TO_MAC_MODE" = "y" ]; then
	#VTSS external switch
	SWITCH_MODE=1
    elif [ "$CONFIG_RT_3052_ESW" = "y" ]; then
	#internal 3052 ESW
	SWITCH_MODE=2
    else
	#default internal switch
	SWITCH_MODE=2
    fi
}

getHostName()
{
    HOSTNAME=`nvram_get 2860 HostName`
}

wait_connect()
#wait connect to ap
{
    getStaMode
    if [ "$stamode" = "y" ]; then
	connected=`iwpriv ra0 connStatus | grep Connected -c`
	if [ "$connected" = "0" ] || [ ! -f /tmp/sta_connected ]; then
	    staCur_SSID=""
    	    exit 0
	fi    
	staCur_SSID=`iwpriv ra0 connStatus | grep ra0 | awk ' { print $3 } ' | cut -f1 -d[`
    fi
}

udhcpc_opts()
{
	CL_SLEEP=1
	if [ "$stamode" = "y" ]; then
	    CL_SLEEP=5
	    #disable dhcp renew from driver
	    sysctl -w net.ipv4.send_sigusr_dhcpc=9
	else
	    ForceRenewDHCP=`nvram_get 2860 ForceRenewDHCP`
	    if [ "$ForceRenewDHCP" != "0" ] &&  [ "$wan_port" != "" ]; then
		#configure event wait port
		sysctl -w net.ipv4.send_sigusr_dhcpc=$wan_port
	    else
		#disable dhcp renew from driver
		sysctl -w net.ipv4.send_sigusr_dhcpc=9
	    fi

	fi
	UDHCPCOPTS="-i $wan_if -H $HOSTNAME -S -R -T 5 -a \
		    -s /sbin/udhcpc.sh -p /var/run/udhcpc.pid \
		    -O routes -O staticroutes -O msstaticroutes -f &"
}

setSwMode()
{
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
}

setLanWan()
{
if [ "$CONFIG_RT_3052_ESW" = "y" ]; then
    if [ "$wan_port" = "0" ]; then
	    echo '##### config vlan partition (WLLLL) #####'
	    config-vlan.sh $SWITCH_MODE WLLLL
    else
	    echo '##### config vlan partition (LLLLW) #####'
	    config-vlan.sh $SWITCH_MODE LLLLW
    fi
	echo '######## clear switch mac table  ########'
        switch clear
fi
}

resetLanWan()
{
if [ "$CONFIG_RT_3052_ESW" = "y" ]; then
    echo "##### restore to dump switch #####"
    config-vlan.sh $SWITCH_MODE 0
fi
}

resetPhy()
{
if [ "$CONFIG_RT_3052_ESW" = "y" ]; then
    $LOG "Reinit power mode for all switch ports" #workaroud for dir-300NRU
    config-vlan.sh 2 FFFFF
fi
}

vpn_deadloop_fix()
{
    #L2TP and PPTP kernel dead-loop fix
    if [ "$vpnEnabled" = "on" ]; then
	vpnType=`nvram_get 2860 vpnType`
	if [ "$vpnType" != "0" ]; then
	    # First vpn stop.. 
	    # Auto start later renew/bound
	    service vpnhelper stop > /dev/null 2>&1
	fi
    fi
    ip route flush cache > /dev/null 2>&1
    echo 1 > /proc/sys/net/nf_conntrack_flush
}

#set default
ethconv="n"
stamode="n"
wan_if="br0"
wan_ppp_if="br0"
lan_if="br0"
lan2_if="br0:9"

#get param
getLanIfName
getLan2IfName
getWanIfName
getWanPppIfName
getWanUpnpIfName
getEthConv
getStaMode
getSwType
getHostName
