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

service lan stop
service lan start
service pass start
service vpn-passthru start

ifRaxWdsxDown()
{
	num=0
        for i in `seq 0 7`; do
	    ifconfig ra$num down > /dev/null 2>&1
            num=`expr $num + 1`
        done
	num=0
        for i in `seq 0 3`; do
    	    ifconfig wds$num down > /dev/null 2>&1
            num=`expr $num + 1`
        done

	ifconfig apcli0 down > /dev/null 2>&1
	ifconfig mesh0 down > /dev/null 2>&1
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
        ifconfig mesh0 up
        brctl addif br0 mesh0
        meshhostname=`nvram_get 2860 MeshHostName`
        iwpriv mesh0 set  MeshHostName="$meshhostname"
    fi
}

addRax2Br0()
{
    inic_bssnum=`nvram_get inic BssidNum`
    if [ "$CONFIG_RT2880_INIC" == "" -a "$CONFIG_RT2880v2_INIC_MII" == "" -a "$CONFIG_RT2880v2_INIC_PCI" == "" ]; then
	return
    fi
    num=0
    for i in `seq 0 $bssidnum`; do
	    ifconfig ra$num up
	    brctl addif br0 ra$num
            num=`expr $num + 1`
    done
}

addRaix2Br0()
{
    inic_bssnum=`nvram_get inic BssidNum`
    if [ "$CONFIG_RT2880_INIC" == "" -a "$CONFIG_RT2880v2_INIC_MII" == "" -a "$CONFIG_RT2880v2_INIC_PCI" == "" ]; then
	return
    fi
    num=0
    for i in `seq 0 $inic_bssnum`; do
	    ifconfig rai$num up
	    brctl addif br0 rai$num
            num=`expr $num + 1`
    done
}

addWds2Br0()
{
    if [ "$CONFIG_RT2880_INIC" == "" -a "$CONFIG_RT2880v2_INIC_MII" == "" -a "$CONFIG_RT2880v2_INIC_PCI" == "" ]; then
	wds_en=`nvram_get 2860 WdsEnable`
    else
	wds_en=`nvram_get inic WdsEnable`
    fi
    if [ "$wds_en" != "0" ]; then
	num=0
        for i in `seq 0 3`; do
    	    ifconfig wds$num up
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
if [ "$opmode" = "3" -a "$CONFIG_RT2860V2_AP_APCLI" != "y" ]; then
	nvram_set 2860 OperationMode 1
	opmode="1"
fi
if [ "$opmode" = "2" -a "$CONFIG_RT2860V2_STA" == "" ]; then
	nvram_set 2860 OperationMode 1
	opmode="1"
fi
if [ "$CONFIG_DWC_OTG" == "m" ]; then
    isDWCOTGExist=`nvram_get 2860 IsDWCOTGExist`
fi

ifRaxWdsxDown
rmmod rt2860v2_ap > /dev/null 2>&1
rmmod rt2860v2_sta > /dev/null 2>&1
ralink_init make_wireless_config rt2860

if [ "$stamode" = "y" ]; then
	modprobe rt2860v2_sta
else
	modprobe rt2860v2_ap
fi


# INIC support
if [ "$CONFIG_RT2880_INIC" != "" ]; then
	ifconfig rai0 down > /dev/null 2>&1
	rmmod rt_pci_dev > /dev/null 2>&1
	ralink_init make_wireless_config inic
	modprobe rt_pci_dev
	ifconfig rai0 up
	RaAP&
	sleep 3
fi
# INIC support
if [ "$CONFIG_RT2880v2_INIC_MII" != "" -o "$CONFIG_RT2880v2_INIC_PCI" != "" ]; then
        iNIC_Mii_en=`nvram_get inic InicMiiEnable`
        ifconfig rai0 down > /dev/null 2>&1
        rmmod iNIC_pci > /dev/null 2>&1
        rmmod iNIC_mii > /dev/null 2>&1
        ralink_init make_wireless_config inic
if [ "$iNIC_Mii_en" != "1" ]; then
        modprobe iNIC_pci mode=ap
else
        modprobe iNIC_mii miimaster=eth2
fi
        ifconfig rai0 up
        sleep 3
fi

# config interface
ifconfig ra0 0.0.0.0
if [ "$ethconv" = "y" ]; then
	iwpriv ra0 set EthConvertMode=dongle
fi
if [ "$radio_off" = "1" ]; then
	iwpriv ra0 set RadioOn=0
fi

num=1;
if [ "$bssidnum" != "0" ] && [ "$bssidnum" != "1" ]; then
    for i in `seq 1 $bssidnum`; do
        ifconfig ra$num 0.0.0.0
        num=`expr $num + 1`
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
	resetLanWan
	brctl addif br0 eth2
	if [ "$CONFIG_RT2860V2_AP_MBSS" = "y" -a "$bssidnum" != "1" ]; then
		addRax2Br0
	fi
        #start mii iNIC after network interface is working
        iNIC_Mii_en=`nvram_get inic InicMiiEnable`
        if [ "$iNIC_Mii_en" == "1" ]; then
             ifconfig rai0 down
             rmmod iNIC_mii
             insmod -q iNIC_mii miimaster=eth2
             ifconfig rai0 up
        fi
 
        addWds2Br0
        addMesh2Br0
	addRaix2Br0
	wan.sh
	lan.sh

elif [ "$opmode" = "1" ]; then
    echo "Gateway OperationMode: $opmode"
	if [ "$CONFIG_MAC_TO_MAC_MODE" = "y" ]; then
		echo '##### config Vtss vlan partition #####'
		config-vlan.sh 1 1
	fi
	resetLanWan
	setLanWan
	addBr0
	brctl addif br0 eth2.1
	if [ "$CONFIG_RT2860V2_AP_MBSS" = "y" -a "$bssidnum" != "1" ]; then
		addRax2Br0
	fi
	addWds2Br0
	addMesh2Br0
	addRaix2Br0
	wan.sh
	lan.sh

elif [ "$opmode" = "2" ]; then
    echo "Ethernet Converter OperationMode: $opmode"
	resetLanWan
	wan.sh
	lan.sh

elif [ "$opmode" = "3" ]; then
    echo "ApClient OperationMode: $opmode"
	resetLanWan
	addBr0
	brctl addif br0 eth2
	wan.sh
	lan.sh
else
    echo "unknown OperationMode: $opmode"
	resetLanWan
	setLanWan
        addBr0
        brctl addif br0 eth2.1
        if [ "$CONFIG_RT2860V2_AP_MBSS" = "y" -a "$bssidnum" != "1" ]; then
                addRax2Br0
        fi
        addWds2Br0
        addRaix2Br0
	exit 1
fi

# INIC support
if [ "$CONFIG_RT2880_INIC" != "" ]; then
       ifconfig rai0 down
       rmmod rt_pci_dev
       ralink_init make_wireless_config inic
       insmod -q rt_pci_dev
       ifconfig rai0 up
       RaAP&
       sleep 3
fi

route add -host 255.255.255.255 dev $lan_if

m2uenabled=`nvram_get 2860 M2UEnabled`
if [ "$m2uenabled" = "1" ]; then
	iwpriv ra0 set IgmpSnEnable=1
	echo "iwpriv ra0 set IgmpSnEnable=1"
fi
