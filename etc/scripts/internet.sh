#!/bin/sh

########################################################
# internet.sh - reconfigure network helper for goahead #
########################################################

#include global config
. /etc/scripts/global.sh

#restart mode
MODE=$1

LOG="logger -t reconfig"

ifRaxWdsxDown()
{
    #down all wireless interfaces and remove from bridge
    for i in `seq 0 7`; do
	ip link set ra$i down > /dev/null 2>&1
	brctl delif br0 ra$i > /dev/null 2>&1
    done
    for i in `seq 0 3`; do
        ip link set wds$i down > /dev/null 2>&1
	brctl delif br0 wds$i > /dev/null 2>&1
    done
    if [ "$CONFIG_RT2860V2_AP_APCLI" != "" ]; then
	ip link set apcli0 down > /dev/null 2>&1
	brctl delif br0 apcli0 > /dev/null 2>&1
    fi
    if [ "$CONFIG_RT2860V2_STA_MESH" != "" ] || [ "$CONFIG_RT2860V2_AP_MESH" != "" ]; then
	ip link set mesh0 down > /dev/null 2>&1
	brctl delif br0 mesh0 > /dev/null 2>&1
    fi
}

addMesh2Br0()
{
    #if kernel build without MESH support - exit
    if [ "$CONFIG_RT2860V2_STA_MESH" != "" ] || [ "$CONFIG_RT2860V2_AP_MESH" != "" ]; then
        meshenabled=`nvram_get 2860 MeshEnabled`
	if [ "$meshenabled" = "1" ]; then
	    getMacIf
	    ip addr flush dev mesh0 > /dev/null 2>&1
	    if [ -d /proc/sys/net/ipv6 ]; then
		ip -6 addr flush dev mesh0 /dev/null 2>&1
	    fi
	    ip link set mesh0 down > /dev/null 2>&1
	    ifconfig mesh0 hw ether $WMAC
    	    brctl addif br0 mesh0
    	    ip link set mesh0 up
	fi
    fi
}

addWds2Br0()
{
    #if kernel build without WDS support - exit
    if [ "$CONFIG_RT2860V2_AP_WDS" != "" ]; then
	wds_en=`nvram_get 2860 WdsEnable`
	if [ "$wds_en" != "0" ]; then
	    getMacIf
    	    for i in `seq 0 3`; do
    		ip addr flush dev wds$i > /dev/null 2>&1
		if [ -d /proc/sys/net/ipv6 ]; then
    		    ip -6 addr flush dev wds$i /dev/null 2>&1
		fi
		ip link set wds$i down > /dev/null 2>&1
		ifconfig wds$i hw ether $WMAC
		brctl addif br0 wds$i
    		ip link set wds$i up
    	    done
	fi
    fi
}

addMBSSID()
{
    #if kernel build without Multiple SSID support - exit
    if [ "$CONFIG_RT2860V2_AP_MBSS" != "" ]; then
	bssidnum=`nvram_get 2860 BssidNum`
	if [ "$bssidnum" != "0" ] && [ "$bssidnum" != "1" ]; then
	    getMacIf
	    let "bssrealnum=$bssidnum-1"
	    for i in `seq 1 $bssrealnum`; do
    		ip addr flush dev ra$i > /dev/null 2>&1
		if [ -d /proc/sys/net/ipv6 ]; then
    		    ip -6 addr flush dev ra$i /dev/null 2>&1
		fi
		ip link set ra$i down > /dev/null 2>&1
		ifconfig ra$i hw ether "$WMAC"
		brctl addif br0 ra$i
    		ip link set ra$i up
	    done
	fi
    fi
}

bridge_config() {
	$LOG "Bridge OperationMode: $opmode"
	#flush eth2 ip and remove from bridge
        ip addr flush dev eth2 > /dev/null 2>&1
	brctl delif br0 eth2 > /dev/null 2>&1
	#in bridge mode add only eth2 NOT ADD eth2.1 o eth2.2
	brctl addif br0 eth2
	#add wifi interface
	brctl addif br0 ra0
	addMBSSID
        addWds2Br0
        addMesh2Br0
}

gate_config() {
	$LOG "Gateway OperationMode: $opmode"
	#flush eth2.1 ip and remove from bridge
        ip addr flush dev eth2.1 > /dev/null 2>&1
	brctl delif br0 eth2.1 > /dev/null 2>&1
	#add lan interface
	brctl addif br0 eth2.1
	#add wifi interface
	brctl addif br0 ra0
	addMBSSID
	addWds2Br0
	addMesh2Br0
}

ethcv_config() {
	$LOG "Ethernet Converter OperationMode: $opmode"
}

apcli_config() {
	$LOG "ApClient OperationMode: $opmode"
	#flush eth2 ip and remove from bridge
        ip addr flush dev eth2 > /dev/null 2>&1
	brctl delif br0 eth2 > /dev/null 2>&1
	#in apcli mode add only eth2 NOT ADD eth2.1 o eth2.2
	brctl addif br0 eth2
	#add wifi interface
	brctl addif br0 ra0
	addMBSSID
}

spot_config() {
	$LOG "HotSpot OperationMode: $opmode"
	#flush eth2.1 ip and remove from bridge
        ip addr flush dev eth2.1 > /dev/null 2>&1
	brctl delif br0 eth2.1 > /dev/null 2>&1
	#add lan interface
	brctl addif br0 eth2.1
	#add wifi interface
	brctl addif br0 ra0
	addMBSSID
	addWds2Br0
	addMesh2Br0
}

retune_wifi() {
	#preconfigure wifi and 40Mhz workaround
	/etc/scripts/wifi.sh
}

#All WDS interfaces down and reload wifi modules
if [ "$MODE" != "connect_sta" ]; then
    if [ "$MODE" != "wifionly" ] || [ "$opmode" = "2" ]; then
	vpn_deadloop_fix
    fi
    $LOG "Shutdown wireless interfaces."
    ifRaxWdsxDown
    $LOG "Reload modules drivers for current mode."
    service modules restart
    $LOG "Tune wifi modules."
    retune_wifi
    if [ "$MODE" != "wifionly" ]; then
	$LOG "Reconfigure lan..."
	service lan restart
    fi
fi

# OperationMode adjustment:
# init ip address to all interfaces for different OperationMode:
#   0 = Bridge Mode
#   1 = Gateway Mode
#   2 = Ethernet Converter Mode
#   3 = AP Client
#
if [ "$opmode" = "0" ]; then
    bridge_config
elif [ "$opmode" = "1" ]; then
    gate_config
elif [ "$opmode" = "2" ] && [ "$CONFIG_RT2860V2_STA" != "" ]; then
    ethcv_config
elif [ "$opmode" = "3" ] && [ "$CONFIG_RT2860V2_AP_APCLI" != "" ]; then
    apcli_config
elif [ "$opmode" = "4" ] && [ -f /bin/chilli ]; then
    spot_config
else
    $LOG "unknown OperationMode use gate_config: $opmode"
    opmode=1
    gate_config
fi

if [ "$MODE" != "wifionly" ] || [ "$opmode" = "2" ]; then
    $LOG "Reconfigure wan..."
    service wan restart
fi

#some daemons need restart
services_restart.sh all

# in dhcp client mode restart from dhcp script
# in static/zeroconf or pure pppoe mode need restart anyway
vpnPurePPPOE=`nvram_get 2860 vpnPurePPPOE`
if [ "$wanmode" != "DHCP" ] || [ "$vpnPurePPPOE" = "1" ]; then
    (service vpnhelper stop && sleep 2 && service vpnhelper start) &
fi

# this is hook for exec user script after physycal connection configured
# may be used for add scripts needed by some provides
# example: http://sadnet.ru/downloads/wan_up.sh - load external routes for www.kvidex.ru ISP
if [ -f /etc/wan_up.sh ]; then
    chmod 777 /etc/wan_up.sh
    $LOG "Call user /etc/wan_up.sh script."
    sh /etc/wan_up.sh &
fi
