#!/bin/sh

############################################################
# global.sh - correct enviroment helper for automatization #
############################################################

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

#get wireless mac adress
WMAC=`nvram_get 2860 WLAN_MAC_ADDR`

web_wait()
{
#wait to start web and run from goahead code
    if [ ! -f /var/run/goahead.pid ]; then
      exit 0
    fi
}

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
	wan_if="apcli0"
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
    	    exit 0
	fi    
    fi
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
