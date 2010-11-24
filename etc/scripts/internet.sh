#!/bin/sh

########################################################
# internet.sh - reconfigure network helper for goahead #
########################################################

#include kernel config and global
. /etc/scripts/config.sh
. /etc/scripts/global.sh

#restart mode
MODE=$1

ifRaxWdsxDown()
{
    for i in `seq 0 7`; do
	ip link set ra$i down > /dev/null 2>&1
    done
    for i in `seq 0 3`; do
        ip link set wds$i down > /dev/null 2>&1
    done

    ip link set apcli0 down > /dev/null 2>&1
    ip link set mesh0 down > /dev/null 2>&1
}

addBr0()
{
    #if kernel build without bridge support - exit
    if [ "$CONFIG_BRIDGE" != "" ]; then
	brset=`brctl show  | grep br0 -c`
	if [ "$brset" = "0" ]; then
    	    echo "Add bridge in the system for ra0"
    	    brctl addbr br0
	fi
	brctl addif br0 ra0
    fi
}

addMesh2Br0()
{
    #if kernel build without MESH support - exit
    if [ "$CONFIG_RT2860V2_STA_MESH" != "" ] || [ "$CONFIG_RT2860V2_STA_MESH" != "" ]; then
        meshenabled=`nvram_get 2860 MeshEnabled`
	if [ "$meshenabled" = "1" ]; then
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
    	    for i in `seq 0 3`; do
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
		if [ "$CONFIG_IPV6" != "" ] ; then
    		    ip -6 addr flush dev ra$i
		fi
		ifconfig ra$i hw ether $WMAC
    		ip link set ra$i up
	    done
	fi
    fi
}

retune_wifi() {
    #preconfigure wifi and 40Mhz workaround
    /etc/scripts/wifi.sh $MODE
}

bridge_config() {
	echo "Bridge OperationMode: $opmode"
	addMBSSID
	addBr0
	#in flush eth2 ip. workaround for change mode to bridge from ethernet converter
        ip addr flush dev eth2
	#in bridge mode add only eth2 NOT ADD eth2.1 o eth2.2
	brctl addif br0 eth2
        addWds2Br0
        addMesh2Br0
}

gate_config() {
	echo "Gateway OperationMode: $opmode"
	addMBSSID
	addBr0
	brctl addif br0 eth2.1
	addWds2Br0
	addMesh2Br0
}

ethcv_config() {
	echo "Ethernet Converter OperationMode: $opmode"
}

apcli_config() {
	echo "ApClient OperationMode: $opmode"
	addMBSSID
	addBr0
	brctl addif br0 eth2
}

#clear conntrack tables
if [ "$MODE" != "wifionly" ] && [ "$CONFIG_USER_CLEAN_NAT" != "" ]; then
    echo 0 > /proc/cleannat
fi

#All WDS interfaces down and reload wifi modules
if [ "$MODE" != "lanonly" ] && [ "$MODE" != "connect_sta" ]; then
    ifRaxWdsxDown
    service modules restart
fi

#restart lan interfaces
if [ "$MODE" != "connect_sta" ]; then
    service lan restart
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
else
    echo "unknown OperationMode use gate_config: $opmode"
    opmode=1
    gate_config
fi

#set flag for init scripts
if [ "$stamode" = "y" ] && [ "$MODE" = "connect_sta" ]; then
    touch /tmp/sta_connected
fi

#reconfigure wan and services restart
service wan restart

if [ "$MODE" != "lanonly" ]; then
    retune_wifi
fi

#some daemons need restart
services_restart.sh all

#retune shaper
if [ "$MODE" = "lanonly" ]; then
    service shaper restart
fi

#restart vpn helper
service vpnhelper restart &

exit 0
