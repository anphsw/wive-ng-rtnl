#!/bin/sh
#
# $Id: internet.sh,v 1.53.2.3 2008-10-20 05:15:44 steven Exp $
#
# usage: internet.sh
#

. /sbin/config.sh
. /sbin/global.sh

lan_ip=`nvram_get 2860 lan_ipaddr`
nat_en=`nvram_get 2860 natEnabled`
bssidnum=`nvram_get 2860 BssidNum`
radio_off=`nvram_get 2860 RadioOff`

if [ "$lan_ip" = "" ]; then
    lan_ip="192.168.1.1"
    nvram_set 2860 lan_ipaddr 192.168.1.1
fi

ifRaxWdsxDown()
{
	ifconfig ra0 down
	ifconfig ra1 down
	ifconfig ra2 down
	ifconfig ra3 down
	ifconfig ra4 down
	ifconfig ra5 down
	ifconfig ra6 down
	ifconfig ra7 down

	ifconfig wds0 down
	ifconfig wds1 down
	ifconfig wds2 down
	ifconfig wds3 down

	ifconfig apcli0 down
}

addBr0()
{
	brctl addbr br0
	brctl addif br0 ra0
}

addRax2Br0()
{
	if [ "$bssidnum" = "2" ]; then
		brctl addif br0 ra1
	elif [ "$bssidnum" = "3" ]; then
		brctl addif br0 ra1
		brctl addif br0 ra2
	elif [ "$bssidnum" = "4" ]; then
		brctl addif br0 ra1
		brctl addif br0 ra2
		brctl addif br0 ra3
	elif [ "$bssidnum" = "5" ]; then
		brctl addif br0 ra1
		brctl addif br0 ra2
		brctl addif br0 ra3
		brctl addif br0 ra4
	elif [ "$bssidnum" = "6" ]; then
		brctl addif br0 ra1
		brctl addif br0 ra2
		brctl addif br0 ra3
		brctl addif br0 ra4
		brctl addif br0 ra5
	elif [ "$bssidnum" = "7" ]; then
		brctl addif br0 ra1
		brctl addif br0 ra2
		brctl addif br0 ra3
		brctl addif br0 ra4
		brctl addif br0 ra5
		brctl addif br0 ra6
	elif [ "$bssidnum" = "8" ]; then
		brctl addif br0 ra1
		brctl addif br0 ra2
		brctl addif br0 ra3
		brctl addif br0 ra4
		brctl addif br0 ra5
		brctl addif br0 ra6
		brctl addif br0 ra7
	fi
}

addWds2Br0()
{
	wds_en=`nvram_get 2860 WdsEnable`
	if [ "$wds_en" != "0" ]; then
		ifconfig wds0 up
		ifconfig wds1 up
		ifconfig wds2 up
		ifconfig wds3 up
		brctl addif br0 wds0
		brctl addif br0 wds1
		brctl addif br0 wds2
		brctl addif br0 wds3
	fi
}

addRaix2Br0()
{
	inic_bssnum=`nvram_get inic BssidNum`
	if [ "$CONFIG_RT2880_INIC" == "" -a "$CONFIG_RT2880v2_INIC_MII" == "" -a "$CONFIG_RT2880v2_INIC_PCI" == "" ]; then
		return
	fi
	brctl addif br0 rai0

	if [ "$inic_bssnum" = "2" ]; then
		ifconfig rai1 up
		brctl addif br0 rai1
	elif [ "$inic_bssnum" = "3" ]; then
		ifconfig rai1 up
		ifconfig rai2 up
		brctl addif br0 rai1
		brctl addif br0 rai2
	elif [ "$inic_bssnum" = "4" ]; then
		ifconfig rai1 up
		ifconfig rai2 up
		ifconfig rai3 up
		brctl addif br0 rai1
		brctl addif br0 rai2
		brctl addif br0 rai3
	elif [ "$inic_bssnum" = "5" ]; then
		ifconfig rai1 up
		ifconfig rai2 up
		ifconfig rai3 up
		ifconfig rai4 up
		brctl addif br0 rai1
		brctl addif br0 rai2
		brctl addif br0 rai3
		brctl addif br0 rai4
	elif [ "$inic_bssnum" = "6" ]; then
		ifconfig rai1 up
		ifconfig rai2 up
		ifconfig rai3 up
		ifconfig rai4 up
		ifconfig rai5 up
		brctl addif br0 rai1
		brctl addif br0 rai2
		brctl addif br0 rai3
		brctl addif br0 rai4
		brctl addif br0 rai5
	elif [ "$inic_bssnum" = "7" ]; then
		ifconfig rai1 up
		ifconfig rai2 up
		ifconfig rai3 up
		ifconfig rai4 up
		ifconfig rai5 up
		ifconfig rai6 up
		brctl addif br0 rai1
		brctl addif br0 rai2
		brctl addif br0 rai3
		brctl addif br0 rai4
		brctl addif br0 rai5
		brctl addif br0 rai6
	elif [ "$inic_bssnum" = "8" ]; then
		ifconfig rai1 up
		ifconfig rai2 up
		ifconfig rai3 up
		ifconfig rai4 up
		ifconfig rai5 up
		ifconfig rai6 up
		ifconfig rai7 up
		brctl addif br0 rai1
		brctl addif br0 rai2
		brctl addif br0 rai3
		brctl addif br0 rai4
		brctl addif br0 rai5
		brctl addif br0 rai6
		brctl addif br0 rai7
	fi
}

addInicWds2Br0()
{
	if [ "$CONFIG_RT2880_INIC" == "" -a "$CONFIG_RT2880v2_INIC_MII" == "" -a "$CONFIG_RT2880v2_INIC_PCI" == "" ]; then
		return
	fi
	wds_en=`nvram_get inic WdsEnable`
	if [ "$wds_en" != "0" ]; then
		ifconfig wdsi0 up
		ifconfig wdsi1 up
		ifconfig wdsi2 up
		ifconfig wdsi3 up
		brctl addif br0 wdsi0
		brctl addif br0 wdsi1
		brctl addif br0 wdsi2
		brctl addif br0 wdsi3
	fi
}

addRaL02Br0()
{
	if [ "$CONFIG_RT2561_AP" != "" ]; then
		brctl addif br0 raL0
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

service pass start

if [ "$CONFIG_DWC_OTG" == "m" ]; then
isDWCOTGExist=`nvram_get 2860 IsDWCOTGExist`
if [ "$isDWCOTGExist" != "n" -a "$isDWCOTGExist" != "N" ]; then
modprobe lm
modprobe dwc_otg
fi
fi

# modprobe all
modprobe bridge
modprobe mii
modprobe raeth
ifconfig eth2 0.0.0.0

ifRaxWdsxDown
rmmod rt2860v2_ap
rmmod rt2860v2_sta
ralink_init make_wireless_config rt2860
if [ "$stamode" = "y" ]; then
	modprobe rt2860v2_sta
else
	if [ "$CONFIG_RT2860V2_AP_DFS" = "y" ]; then
		modprobe rt_timer
	fi
	modprobe rt2860v2_ap
fi
vpn-passthru.sh


# INIC support
if [ "$CONFIG_RT2880_INIC" != "" ]; then
	ifconfig rai0 down
	rmmod rt_pci_dev
	ralink_init make_wireless_config inic
	modprobe rt_pci_dev
	ifconfig rai0 up
	RaAP&
	sleep 3
fi
# INIC support
if [ "$CONFIG_RT2880v2_INIC_MII" != "" -o "$CONFIG_RT2880v2_INIC_PCI" != "" ]; then
        iNIC_Mii_en=`nvram_get inic InicMiiEnable`
        ifconfig rai0 down
        rmmod iNIC_pci
        rmmod iNIC_mii
        ralink_init make_wireless_config inic
if [ "$iNIC_Mii_en" != "1" ]; then
        modprobe iNIC_pci mode=ap
else
        modprobe iNIC_mii miimaster=eth2
fi
        ifconfig rai0 up
        sleep 3
fi

# RT2561(Legacy) support
if [ "$CONFIG_RT2561_AP" != "" ]; then
	ifconfig raL0 down
	rmmod rt2561ap
	ralink_init make_wireless_config rt2561
	modprobe rt2561ap
	ifconfig raL0 up
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
if [ "$bssidnum" = "2" ]; then
	ifconfig ra1 0.0.0.0
elif [ "$bssidnum" = "3" ]; then
	ifconfig ra1 0.0.0.0
	ifconfig ra2 0.0.0.0
elif [ "$bssidnum" = "4" ]; then
	ifconfig ra1 0.0.0.0
	ifconfig ra2 0.0.0.0
	ifconfig ra3 0.0.0.0
elif [ "$bssidnum" = "5" ]; then
	ifconfig ra1 0.0.0.0
	ifconfig ra2 0.0.0.0
	ifconfig ra3 0.0.0.0
	ifconfig ra4 0.0.0.0
elif [ "$bssidnum" = "6" ]; then
	ifconfig ra1 0.0.0.0
	ifconfig ra2 0.0.0.0
	ifconfig ra3 0.0.0.0
	ifconfig ra4 0.0.0.0
	ifconfig ra5 0.0.0.0
elif [ "$bssidnum" = "7" ]; then
	ifconfig ra1 0.0.0.0
	ifconfig ra2 0.0.0.0
	ifconfig ra3 0.0.0.0
	ifconfig ra4 0.0.0.0
	ifconfig ra5 0.0.0.0
	ifconfig ra6 0.0.0.0
elif [ "$bssidnum" = "8" ]; then
	ifconfig ra1 0.0.0.0
	ifconfig ra2 0.0.0.0
	ifconfig ra3 0.0.0.0
	ifconfig ra4 0.0.0.0
	ifconfig ra5 0.0.0.0
	ifconfig ra6 0.0.0.0
	ifconfig ra7 0.0.0.0
fi
if [ "$CONFIG_RAETH_ROUTER" = "y" -o "$CONFIG_MAC_TO_MAC_MODE" = "y" -o "$CONFIG_RT_3052_ESW" = "y" ]; then
	modprobe 8021q
	vconfig add eth2 1
	vconfig add eth2 2
	ifconfig eth2.2 down
	wan_mac=`nvram_get 2860 WAN_MAC_ADDR`
	if [ "$wan_mac" != "FF:FF:FF:FF:FF:FF" ]; then
	ifconfig eth2.2 hw ether $wan_mac
	fi
	ifconfig eth2.1 0.0.0.0
	ifconfig eth2.2 0.0.0.0
elif [ "$CONFIG_ICPLUS_PHY" = "y" ]; then
	#remove ip alias
	# it seems busybox has no command to remove ip alias...
	ifconfig eth2:1 0.0.0.0 1>&2 2>/dev/null
fi

