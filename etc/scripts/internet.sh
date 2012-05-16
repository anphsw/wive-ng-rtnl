#!/bin/sh

########################################################
# internet.sh - reconfigure network helper for goahead #
########################################################

# include global config
. /etc/scripts/global.sh

# restart mode
MODE="$1"

LOG="logger -t reconfig"

addMesh() {
    # if kernel build without MESH support - exit
    if [ "$CONFIG_RT2860V2_STA_MESH" != "" ] || [ "$CONFIG_RT2860V2_AP_MESH" != "" ]; then
        meshenabled=`nvram_get 2860 MeshEnabled`
	if [ "$meshenabled" = "1" ]; then
	    ip addr flush dev mesh0 > /dev/null 2>&1
	    if [ -d /proc/sys/net/ipv6 ] && [ "$IPv6_Enable" = "1" ]; then
		ip -6 addr flush dev mesh0 /dev/null 2>&1
	    fi
	    ip link set mesh0 down > /dev/null 2>&1
	    ifconfig mesh0 hw ether $WLAN_MAC_ADDR
    	    brctl addif br0 mesh0
    	    ip link set mesh0 up
	fi
    fi
}

addWds() {
    # if kernel build without WDS support - exit
    if [ "$CONFIG_RT2860V2_AP_WDS" != "" ]; then
	wds_en=`nvram_get 2860 WdsEnable`
	if [ "$wds_en" != "0" ]; then
    	    for i in `seq 0 3`; do
    		ip addr flush dev wds$i > /dev/null 2>&1
		if [ -d /proc/sys/net/ipv6 ] && [ "$IPv6_Enable" = "1" ]; then
    		    ip -6 addr flush dev wds$i /dev/null 2>&1
		fi
		ip link set wds$i down > /dev/null 2>&1
		ifconfig wds$i hw ether $WLAN_MAC_ADDR
		brctl addif br0 wds$i
    		ip link set wds$i up
    	    done
	fi
    fi
    if [ "$CONFIG_RT3090_AP_WDS" != "" ]; then
	wds_en=`nvram_get 2860 WdsEnable`
	if [ "$wds_en" != "0" ]; then
    	    for i in `seq 0 3`; do
    		ip addr flush dev wdsi$i > /dev/null 2>&1
		if [ -d /proc/sys/net/ipv6 ] && [ "$IPv6_Enable" = "1" ]; then
    		    ip -6 addr flush dev wdsi$i /dev/null 2>&1
		fi
		ip link set wdsi$i down > /dev/null 2>&1
		ifconfig wdsi$i hw ether $WLAN_MAC_ADDR
		brctl addif br0 wdsi$i
    		ip link set wdsi$i up
    	    done
	fi
    fi
}

addMBSSID() {
    # if kernel build without Multiple SSID support - exit
    if [ "$CONFIG_RT2860V2_AP_MBSS" != "" ]; then
	bssidnum=`nvram_get 2860 BssidNum`
	if [ "$bssidnum" != "0" ] && [ "$bssidnum" != "1" ]; then
	    let "bssrealnum=$bssidnum-1"
	    for i in `seq 1 $bssrealnum`; do
    		ip addr flush dev ra$i > /dev/null 2>&1
		if [ -d /proc/sys/net/ipv6 ] && [ "$IPv6_Enable" = "1" ]; then
    		    ip -6 addr flush dev ra$i /dev/null 2>&1
		fi
		ip link set ra$i down > /dev/null 2>&1
		ifconfig ra$i hw ether "$WLAN_MAC_ADDR"
		brctl addif br0 ra$i
    		ip link set ra$i up
	    done
	fi
    fi
    if [ "$CONFIG_RT3090_AP_MBSS" != "" ]; then
	bssidnum=`nvram_get 2860 BssidNum`
	if [ "$bssidnum" != "0" ] && [ "$bssidnum" != "1" ]; then
	    let "bssrealnum=$bssidnum-1"
	    for i in `seq 1 $bssrealnum`; do
    		ip addr flush dev rai$i > /dev/null 2>&1
		if [ -d /proc/sys/net/ipv6 ] && [ "$IPv6_Enable" = "1" ]; then
    		    ip -6 addr flush dev rai$i /dev/null 2>&1
		fi
		ip link set ra$i down > /dev/null 2>&1
		ifconfig rai$i hw ether "$WLAN_MAC_ADDR"
		brctl addif br0 rai$i
    		ip link set rai$i up
	    done
	fi
    fi
}

bridge_config() {
	$LOG "Bridge OperationMode: $OperationMode"
	# flush eth2 ip and remove from bridge
        ip addr flush dev eth2 > /dev/null 2>&1
	brctl delif br0 eth2 > /dev/null 2>&1
	# in bridge mode add only eth2 NOT ADD "$phys_lan_if" or "$phys_wan_if"
	brctl addif br0 eth2
	# add wifi interface
	brctl addif br0 ra0
	if [ "$CONFIG_RT3090_AP" != "" ]; theb
	    # add wifi interface
	    brctl addif br0 rai0
	fi
	addMBSSID
        addWds
        addMesh
}

gate_config() {
	$LOG "Gateway OperationMode: $OperationMode"
	# flush "$phys_lan_if" ip and remove from bridge
        ip addr flush dev "$phys_lan_if" > /dev/null 2>&1
	brctl delif br0 "$phys_lan_if" > /dev/null 2>&1
	# add lan interface
	brctl addif br0 "$phys_lan_if"
	# add wifi interface
	brctl addif br0 ra0
	if [ "$CONFIG_RT3090_AP" != "" ]; theb
	    # add wifi interface
	    brctl addif br0 rai0
	fi
	addMBSSID
	addWds
	addMesh
}

ethcv_config() {
	$LOG "Ethernet Converter OperationMode: $OperationMode"
}

apcli_config() {
	$LOG "ApClient OperationMode: $OperationMode"
	# flush eth2 ip and remove from bridge
        ip addr flush dev eth2 > /dev/null 2>&1
	brctl delif br0 eth2 > /dev/null 2>&1
	# in apcli mode add only eth2 NOT ADD "$phys_lan_if" or "$phys_wan_if"
	brctl addif br0 eth2
	# add ap wifi interface
	brctl addif br0 ra0
	if [ "$CONFIG_RT3090_AP" != "" ]; theb
	    # add wifi interface
	    brctl addif br0 rai0
	fi
	if [ "$ApCliBridgeOnly" = "1" ]; then
	    # add client wifi interface
	    brctl addif br0 apcli0
	fi
	addMBSSID
}

spot_config() {
	$LOG "HotSpot OperationMode: $OperationMode"
	# flush "$phys_lan_if" ip and remove from bridge
        ip addr flush dev "$phys_lan_if" > /dev/null 2>&1
	brctl delif br0 "$phys_lan_if" > /dev/null 2>&1
	# add lan interface
	brctl addif br0 "$phys_lan_if"
	# add wifi interface
	brctl addif br0 ra0
	if [ "$CONFIG_RT3090_AP" != "" ]; theb
	    # add wifi interface
	    brctl addif br0 rai0
	fi
	addMBSSID
	addWds
	addMesh
}

# WiFi modules reloand and reconfigure
if [ "$MODE" != "connect_sta" ]; then
    if [ "$MODE" != "wifionly" ]; then
	service vpnhelper stop_safe
    fi
    $LOG "Reload wireless modules..."
    service modules restart
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
if [ "$OperationMode" = "0" ]; then
    bridge_config
elif [ "$OperationMode" = "1" ]; then
    gate_config
elif [ "$OperationMode" = "2" ] && [ "$CONFIG_RT2860V2_STA" != "" ]; then
    ethcv_config
elif [ "$OperationMode" = "3" ] && [ "$CONFIG_RT2860V2_AP_APCLI" != "" ]; then
    apcli_config
elif [ "$OperationMode" = "4" ] && [ -f /bin/chilli ]; then
    spot_config
else
    $LOG "unknown OperationMode use gate_config: $OperationMode"
    OperationMode=1
    gate_config
fi

if [ "$MODE" != "wifionly" ] && [ "$MODE" != "connect_sta" ]; then
    $LOG "Reconfigure wan..."
    service wan restart
fi

##########################################################
# Regenerate resolv only if wan_static_dns on		 #
##########################################################
if [ "$wan_static_dns" = "on" ]; then
    service resolv start
fi

# some daemons need restart
services_restart.sh all

# in dhcp client mode restart from dhcp script
# in static/zeroconf or pure pppoe mode need restart anyway
if [ "$vpnEnabled" = "on" -a "$vpnType" = "0" -a "$vpnPurePPPOE" = "1" ] || [ "$wanConnectionMode" != "DHCP" ]; then
    (service vpnhelper stop && sleep 2 && service vpnhelper start) &
fi

# this is hook for exec user script after physycal connection configured
# may be used for add scripts needed by some provides
# example: http://wive-ng.sf.net/downloads/wan_up.sh - load external routes for www.kvidex.ru ISP
if [ -f /etc/wan_up.sh ]; then
    chmod 755 /etc/wan_up.sh
    $LOG "Call user /etc/wan_up.sh script."
    sh /etc/wan_up.sh &
fi
