#!/bin/sh
#
# $Id: internet.sh,v 1.53.2.3 2008-10-20 05:15:44 steven Exp $
#
# usage: internet.sh
#

. /sbin/config.sh
. /sbin/global.sh

lan_ip=`nvram_get 2860 lan_ipaddr`
stp_en=`nvram_get 2860 stpEnabled`
nat_en=`nvram_get 2860 natEnabled`
bssidnum=`nvram_get 2860 BssidNum`
radio_off=`nvram_get 2860 RadioOff`

MODE=$1 #restart mode

ifRaxWdsxDown()
{
	num=0
        for i in `seq 0 7`; do
	    ip link set ra$num down > /dev/null 2>&1
            num=`expr $num + 1`
        done
	num=0
        for i in `seq 0 3`; do
    	    ip link set wds$num down > /dev/null 2>&1
            num=`expr $num + 1`
        done

	ip link set apcli0 down > /dev/null 2>&1
	ip link set mesh0 down > /dev/null 2>&1
}

addBr0()
{
    brset=`brctl show  | grep br0 -c`
    if [ "$brset" = "0" ]; then
        echo "Add bridge in the system for ra0"
        brctl addbr br0
    fi
    brctl addif br0 ra0
}

addMesh2Br0()
{
    meshenabled=`nvram_get 2860 MeshEnabled`
    if [ "$meshenabled" = "1" ]; then
        ip link set mesh0 up
        brctl addif br0 mesh0
        meshhostname=`nvram_get 2860 MeshHostName`
        iwpriv mesh0 set  MeshHostName="$meshhostname"
    fi
}

addWds2Br0()
{
    wds_en=`nvram_get 2860 WdsEnable`
    if [ "$wds_en" != "0" ]; then
	num=0
        for i in `seq 0 3`; do
    	    ip link set wds$num up
	    brctl addif br0 wds$num
            num=`expr $num + 1`
        done
    fi
}

setLanWan()
{
    if [ "$CONFIG_RAETH_ROUTER" = "y" ]; then
	if [ "$CONFIG_WAN_AT_P0" = "y" ]; then
	    echo '##### config vlan partition (WLLLL) #####'
	    config-vlan.sh 0 WLLLL
	else
	    echo '##### config vlan partition (LLLLW) #####'
	    config-vlan.sh 0 LLLLW
	fi
    fi
    if [ "$CONFIG_RT_3052_ESW" = "y" ]; then
	if [ "$CONFIG_WAN_AT_P0" = "y" ]; then
    	    echo '##### config RT3052 vlan partition (WLLLL) #####'
	    config-vlan.sh 2 WLLLL
	else
	    echo '##### config RT3052 vlan partition (LLLLW) #####'
	    config-vlan.sh 2 LLLLW
	fi
    fi
}

resetLanWan()
{
    if [ "$CONFIG_MAC_TO_MAC_MODE" = "y" ]; then
	echo "##### restore Vtss to dump switch #####"
	config-vlan.sh 1 0
    elif [ "$CONFIG_RT_3052_ESW" = "y" ]; then
	echo "##### restore RT3052 to dump switch #####"
	config-vlan.sh 2 0
    fi
}


# opmode adjustment:
#   if AP client was not compiled and operation mode was set "3" -> set $opmode "1"
#   if Station was not compiled and operation mode was set "2" -> set $opmode "1"

if [ "$opmode" = "3" ] && [ "$CONFIG_RT2860V2_AP_APCLI" != "y" ]; then
	nvram_set 2860 OperationMode 1
	opmode="1"
fi
if [ "$opmode" = "2" ] && [ "$CONFIG_RT2860V2_STA" == "" ]; then
	nvram_set 2860 OperationMode 1
	opmode="1"
fi
if [ "$CONFIG_DWC_OTG" == "m" ]; then
    isDWCOTGExist=`nvram_get 2860 IsDWCOTGExist`
fi

ifRaxWdsxDown

#clear conntrack tables
if [ "$MODE" != "wifionly" ]; then
    echo 0 > /proc/cleannat
fi

#reload wifi modules
service modules restart

# config interface
ip addr flush dev ra0
ip -6 addr flush dev ra0
ip link set ra0 up

#restart lan interfaces
service lan restart

if [ "$ethconv" = "y" ]; then
	iwpriv ra0 set EthConvertMode=dongle
fi
if [ "$radio_off" = "1" ]; then
	iwpriv ra0 set RadioOn=0
fi
if [ "$MODE" != "wifionly" ]; then
    m2uenabled=`nvram_get 2860 M2UEnabled`
    if [ "$m2uenabled" = "1" ]; then
	iwpriv ra0 set IgmpSnEnable=1
	echo "iwpriv ra0 set IgmpSnEnable=1"
    fi
fi

if [ "$bssidnum" != "0" ] && [ "$bssidnum" != "1" ]; then
    for i in `seq 1 $bssidnum`; do
	ip addr flush dev ra$i
	ip -6 addr flush dev ra$i
	ip link set ra$i up 
    done
fi

#
# init ip address to all interfaces for different OperationMode:
#   0 = Bridge Mode
#   1 = Gateway Mode
#   2 = Ethernet Converter Mode
#   3 = AP Client
#
if [ "$opmode" = "0" ]; then
    echo "Bridge OperationMode: $opmode"
	addBr0
    if [ "$MODE" != "wifionly" ]; then
	resetLanWan
    fi
	#in bridge mode add only eth2 NOT ADD eth2.1 o eth2.2
	brctl addif br0 eth2
        addWds2Br0
        addMesh2Br0

elif [ "$opmode" = "1" ]; then
    echo "Gateway OperationMode: $opmode"
    if [ "$CONFIG_MAC_TO_MAC_MODE" = "y" ]; then
	echo '##### config Vtss vlan partition #####'
    	config-vlan.sh 1 1
    fi
    if [ "$MODE" != "wifionly" ]; then
	resetLanWan
	setLanWan
    fi
	addBr0
	brctl addif br0 eth2.1
	addWds2Br0
	addMesh2Br0

elif [ "$opmode" = "2" ]; then
    echo "Ethernet Converter OperationMode: $opmode"
    if [ "$MODE" != "wifionly" ]; then
	resetLanWan
    fi

elif [ "$opmode" = "3" ]; then
    echo "ApClient OperationMode: $opmode"
    if [ "$MODE" != "wifionly" ]; then
	resetLanWan
    fi
	addBr0
	brctl addif br0 eth2
else
    echo "unknown OperationMode: $opmode"
    if [ "$MODE" != "wifionly" ]; then
	resetLanWan
	setLanWan
    fi
        addBr0
        brctl addif br0 eth2.1
        addWds2Br0
	exit 1
fi

#reconfigure wan and services restart
service wan restart
services_restart.sh all
