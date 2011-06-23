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
    for i in `seq 0 7`; do
	ip link set ra$i down > /dev/null 2>&1
    done
    for i in `seq 0 3`; do
        ip link set wds$i down > /dev/null 2>&1
    done
    if [ "$CONFIG_RT2860V2_AP_APCLI" != "" ]; then
	ip link set apcli0 down > /dev/null 2>&1
    fi
    if [ "$CONFIG_RT2860V2_STA_MESH" != "" ] || [ "$CONFIG_RT2860V2_AP_MESH" != "" ]; then
	ip link set mesh0 down > /dev/null 2>&1
    fi
}

addMesh2Br0()
{
    #if kernel build without MESH support - exit
    if [ "$CONFIG_RT2860V2_STA_MESH" != "" ] || [ "$CONFIG_RT2860V2_AP_MESH" != "" ]; then
        meshenabled=`nvram_get 2860 MeshEnabled`
	if [ "$meshenabled" = "1" ]; then
	    getMacIf
	    ip link set mesh0 down > /dev/null 2>&1
	    ifconfig mesh0 hw ether $WMAC
    	    ip link set mesh0 up
    	    brctl addif br0 mesh0
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
		ip link set wds$i down > /dev/null 2>&1
		ifconfig wds$i hw ether $WMAC
    		ip link set wds$i up
		brctl addif br0 wds$i
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
	    for i in `seq 1 $bssidnum`; do
    		ip addr flush dev ra$i
		if [ -d /proc/sys/net/ipv6 ]; then
    		    ip -6 addr flush dev ra$i
		fi
		#workaround for apcli mode
		getMacIf
		if [ "$opmode" = "3" ]; then
		    ip link set ra$i down > /dev/null 2>&1
		    ifconfig ra$i hw ether "$WANMAC"
    		    ip link set ra$i up
		else
		    ip link set ra$i down > /dev/null 2>&1
		    ifconfig ra$i hw ether "$WMAC"
    		    ip link set ra$i up
		fi
	    done
	fi
    fi
}

retune_wifi() {
    #preconfigure wifi and 40Mhz workaround
    /etc/scripts/wifi.sh
}

bridge_config() {
	$LOG "Bridge OperationMode: $opmode"
	addMBSSID
	brctl addif br0 ra0
	#in flush eth2 ip. workaround for change mode to bridge from ethernet converter
        ip addr flush dev eth2
	#in bridge mode add only eth2 NOT ADD eth2.1 o eth2.2
	brctl addif br0 eth2
        addWds2Br0
        addMesh2Br0
}

gate_config() {
	$LOG "Gateway OperationMode: $opmode"
	addMBSSID
	brctl addif br0 ra0
	brctl addif br0 eth2.1
	addWds2Br0
	addMesh2Br0
}

ethcv_config() {
	$LOG "Ethernet Converter OperationMode: $opmode"
}

apcli_config() {
	$LOG "ApClient OperationMode: $opmode"
	addMBSSID
	brctl addif br0 ra0
	brctl addif br0 eth2
}

spot_config() {
	$LOG "HotSpot OperationMode: $opmode"
	addMBSSID
	brctl addif br0 ra0
	brctl addif br0 eth2.1
	addWds2Br0
	addMesh2Br0
}


#All WDS interfaces down and reload wifi modules
if [ "$MODE" != "connect_sta" ]; then
    echo 1 > /proc/sys/net/nf_conntrack_flush
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

#
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
elif [ "$opmode" = "2" ]; then
    ethcv_config
elif [ "$opmode" = "3" ]; then
    apcli_config
elif [ "$opmode" = "4" ]; then
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

exit 0
