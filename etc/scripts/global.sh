#!/bin/sh

. /sbin/config.sh

#first get operation mode
opmode=`nvram_get 2860 OperationMode`

web_wait()
{
#wait to start web and run from goahead code
    if [ ! -f /tmp/webrun ]; then
      exit 0
    fi
}

opModeAdj()
{
    # opmode adjustment:
    #   if AP client was not compiled and operation mode was set "3" -> set $opmode "1"
    #   if Station was not compiled and operation mode was set "2" -> set $opmode "1"

    if   [ "$opmode" = "3" ] && [ "$CONFIG_RT2860V2_AP_APCLI" != "y" ]; then
	nvram_set 2860 OperationMode 1
	opmode="1"
    elif [ "$opmode" = "2" ] && [ "$CONFIG_RT2860V2_STA" == "" ]; then
	nvram_set 2860 OperationMode 1
	opmode="1"
    fi
}

# WAN interface name -> $wan_if
getWanIfName()
{
	vpnEnabled=`nvram_get 2860 vpnEnabled`
	if [ "$opmode" = "0" ]; then
		wan_if="br0"
	elif [ "$opmode" = "1" ]; then
		wan_if="eth2.2"
	elif [ "$opmode" = "2" ]; then
		wan_if="ra0"
	elif [ "$opmode" = "3" ]; then
		wan_if="apcli0"
	fi

	if [ "$vpnEnabled" = "on"  ]; then
		wan_ppp_if="ppp0"
	else
		wan_ppp_if=$wan_if
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

ethconv="n"
stamode="n"
wan_if="br0"
wan_ppp_if="br0"
lan_if="br0"
opModeAdj
getWanIfName
getLanIfName
getEthConv
getStaMode
getSwType
