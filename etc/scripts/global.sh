#!/bin/sh

. /sbin/config.sh

web_wait(){
#wait to start web and run from goahead code
    if [ ! -f /tmp/webrun ]; then
      exit 0
    fi
}

# WAN interface name -> $wan_if
getWanIfName()
{
	opmode=`nvram_get 2860 OperationMode`
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
	bssidnum=`nvram_get 2860 BssidNum`

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
	if [ "$opmode" = "2" -o "$ethconv" = "y" ]; then
		stamode="y"
	else
		stamode="n"
	fi
}

opmode=`nvram_get 2860 OperationMode`
ethconv="n"
stamode="n"
wan_if="br0"
wan_ppp_if="br0"
lan_if="br0"
getWanIfName
getLanIfName
getEthConv
getStaMode

# debug
#echo "opmode=$opmode"
#echo "ethconv=$ethconv"
#echo "stamode=$stamode"
#echo "wan_if=$wan_if"
#echo "lan_if=$lan_if"

