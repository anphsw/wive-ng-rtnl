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
	echo "ifRaxWdsxDown"
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
	echo "addBr0"
	brctl addbr br0
	brctl addif br0 ra0
}

addRax2Br0()
{
	echo "addRax2Br0 $bssidnum"
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
	echo "addWds2Br0"
	wds_en=`nvram_get 2860 WdsEnable`
	if [ "$wds_en" != "0" ]; then
		echo "addWds2Br0 test if"
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
	echo "addRaix2Br0"
	inic_bssnum=`nvram_get inic BssidNum`
	if [ "$CONFIG_RT2880_INIC" == "" -a "$CONFIG_RT2880v2_INIC_MII" == "" -a "$CONFIG_RT2880v2_INIC_PCI" == "" ]; then
		echo "r2b test 1"
		return
	fi
	brctl addif br0 rai0

	echo "r2b test 2 $inic_bassnum"
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
	echo "addInicWds2Br0"
	if [ "$CONFIG_RT2880_INIC" == "" -a "$CONFIG_RT2880v2_INIC_MII" == "" -a "$CONFIG_RT2880v2_INIC_PCI" == "" ]; then
		echo "InicWds2Br0 test 1"
		return
	fi
	wds_en=`nvram_get inic WdsEnable`
	if [ "$wds_en" != "0" ]; then
		echo "InicWds2Br0 test 2"
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
	echo "addRaL02Br0"
	if [ "$CONFIG_RT2561_AP" != "" ]; then
		echo "test addRaL02Br0 1"
		brctl addif br0 raL0
	fi
}

# opmode adjustment:
#   if AP client was not compiled and operation mode was set "3" -> set $opmode "1"
#   if Station was not compiled and operation mode was set "2" -> set $opmode "1"
if [ "$opmode" = "3" -a "$CONFIG_RT2860V2_AP_APCLI" != "y" ]; then
	echo "opmode test run 1"
	nvram_set 2860 OperationMode 1
	opmode="1"
fi
if [ "$opmode" = "2" -a "$CONFIG_RT2860V2_STA" == "" ]; then
	echo "opmode test run 2"
	nvram_set 2860 OperationMode 1
	opmode="1"
fi

service pass start

if [ "$CONFIG_DWC_OTG" == "m" ]; then
echo "dwc_otg test 1"
isDWCOTGExist=`nvram_get 2860 IsDWCOTGExist`
if [ "$isDWCOTGExist" != "n" -a "$isDWCOTGExist" != "N" ]; then
echo "dwc_otg test 2"
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

echo "Test Run middle 1"

if [ "$stamode" = "y" ]; then
	echo "test stamode 1"
	modprobe rt2860v2_sta
else
	if [ "$CONFIG_RT2860V2_AP_DFS" = "y" ]; then
		echo "test ap_dfs"
		modprobe rt_timer
	fi
	modprobe rt2860v2_ap
fi
vpn-passthru.sh


# INIC support
if [ "$CONFIG_RT2880_INIC" != "" ]; then
	echo "INIC test"
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
	echo "inic 2"
        iNIC_Mii_en=`nvram_get inic InicMiiEnable`
        ifconfig rai0 down
        rmmod iNIC_pci
        rmmod iNIC_mii
        ralink_init make_wireless_config inic
if [ "$iNIC_Mii_en" != "1" ]; then
	echo "inic 3"
        modprobe iNIC_pci mode=ap
else
	echo "inic 4"
        modprobe iNIC_mii miimaster=eth2
fi
	echo "inic 5"
        ifconfig rai0 up
        sleep 3
fi

# RT2561(Legacy) support
if [ "$CONFIG_RT2561_AP" != "" ]; then
	echo "rt2561 support "
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
	echo "test iwpriv 1"
	iwpriv ra0 set EthConvertMode=dongle
fi
if [ "$radio_off" = "1" ]; then
	echo "test iwpriv 2"
	iwpriv ra0 set RadioOn=0
fi
if [ "$bssidnum" = "2" ]; then
	echo "test iwpriv 3"
	ifconfig ra1 0.0.0.0
elif [ "$bssidnum" = "3" ]; then
	echo "test iwpriv 4"
	ifconfig ra1 0.0.0.0
	ifconfig ra2 0.0.0.0
elif [ "$bssidnum" = "4" ]; then
	echo "test iwpriv 5"
	ifconfig ra1 0.0.0.0
	ifconfig ra2 0.0.0.0
	ifconfig ra3 0.0.0.0
elif [ "$bssidnum" = "5" ]; then
	echo "test iwpriv 6"
	ifconfig ra1 0.0.0.0
	ifconfig ra2 0.0.0.0
	ifconfig ra3 0.0.0.0
	ifconfig ra4 0.0.0.0
elif [ "$bssidnum" = "6" ]; then
	echo "test iwpriv 7"
	ifconfig ra1 0.0.0.0
	ifconfig ra2 0.0.0.0
	ifconfig ra3 0.0.0.0
	ifconfig ra4 0.0.0.0
	ifconfig ra5 0.0.0.0
elif [ "$bssidnum" = "7" ]; then
	echo "test iwpriv 8"
	ifconfig ra1 0.0.0.0
	ifconfig ra2 0.0.0.0
	ifconfig ra3 0.0.0.0
	ifconfig ra4 0.0.0.0
	ifconfig ra5 0.0.0.0
	ifconfig ra6 0.0.0.0
elif [ "$bssidnum" = "8" ]; then
	echo "test iwpriv 9"
	ifconfig ra1 0.0.0.0
	ifconfig ra2 0.0.0.0
	ifconfig ra3 0.0.0.0
	ifconfig ra4 0.0.0.0
	ifconfig ra5 0.0.0.0
	ifconfig ra6 0.0.0.0
	ifconfig ra7 0.0.0.0
fi
if [ "$CONFIG_RAETH_ROUTER" = "y" -o "$CONFIG_MAC_TO_MAC_MODE" = "y" -o "$CONFIG_RT_3052_ESW" = "y" ]; then
	echo "test iwpriv 10"
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
	echo "test iwpriv 11"
	#remove ip alias
	# it seems busybox has no command to remove ip alias...
	ifconfig eth2:1 0.0.0.0 1>&2 2>/dev/null
fi

ifconfig lo 127.0.0.1
ifconfig br0 down
brctl delbr br0

# stop all
iptables --flush
iptables --flush -t nat
iptables --flush -t mangle


#
# init ip address to all interfaces for different OperationMode:
#   0 = Bridge Mode
#   1 = Gateway Mode
#   2 = Ethernet Converter Mode
#   3 = AP Client
#

echo "opmode is $opmode"
if [ "$opmode" = "0" ]; then
	addBr0
	if [ "$CONFIG_RAETH_ROUTER" = "y" -a "$CONFIG_LAN_WAN_SUPPORT" = "y" ]; then
		echo "##### restore IC+ to dump switch #####"
		config-vlan.sh 0 0
	elif [ "$CONFIG_MAC_TO_MAC_MODE" = "y" ]; then
		echo "##### restore Vtss to dump switch #####"
		config-vlan.sh 1 0
	elif [ "$CONFIG_RT_3052_ESW" = "y" ]; then
		echo "##### restore RT3052 to dump switch #####"
		config-vlan.sh 2 0
	fi
	brctl addif br0 eth2
	if [ "$CONFIG_RT2860V2_AP_MBSS" = "y" -a "$bssidnum" != "1" ]; then
		addRax2Br0
	fi
	addWds2Br0
	addRaix2Br0
	addInicWds2Br0
	addRaL02Br0
	wan.sh
	lan.sh
	echo 0 > /proc/sys/net/ipv4/ip_forward
elif [ "$opmode" = "1" ]; then
	if [ "$CONFIG_RAETH_ROUTER" = "y" -o "$CONFIG_MAC_TO_MAC_MODE" = "y" -o "$CONFIG_RT_3052_ESW" = "y" ]; then
		if [ "$CONFIG_RAETH_ROUTER" = "y" -a "$CONFIG_LAN_WAN_SUPPORT" = "y" ]; then
			if [ "$CONFIG_WAN_AT_P0" = "y" ]; then
				echo '##### config IC+ vlan partition (WLLLL) #####'
				config-vlan.sh 0 WLLLL
			else
				echo '##### config IC+ vlan partition (LLLLW) #####'
				config-vlan.sh 0 LLLLW
			fi
		fi
		if [ "$CONFIG_MAC_TO_MAC_MODE" = "y" ]; then
			echo '##### config Vtss vlan partition #####'
			config-vlan.sh 1 1
		fi
		if [ "$CONFIG_RT_3052_ESW" = "y" -a "$CONFIG_LAN_WAN_SUPPORT" = "y" ]; then
			if [ "$CONFIG_WAN_AT_P0" = "y" ]; then
				echo '##### config RT3052 vlan partition (WLLLL) #####'
				config-vlan.sh 2 WLLLL
			else
				echo '##### config RT3052 vlan partition (LLLLW) #####'
				config-vlan.sh 2 LLLLW
			fi
		fi
		addBr0
		brctl addif br0 eth2.1
		if [ "$CONFIG_RT2860V2_AP_MBSS" = "y" -a "$bssidnum" != "1" ]; then
			addRax2Br0
		fi
		addWds2Br0
		addRaix2Br0
		addInicWds2Br0
		addRaL02Br0
	fi

	# IC+ 100 PHY (one port only)
	if [ "$CONFIG_ICPLUS_PHY" = "y" ]; then
		echo '##### connected to one port 100 PHY #####'
		if [ "$CONFIG_RT2860V2_AP_MBSS" = "y" -a "$bssidnum" != "1" ]; then
			addBr0
			addRax2Br0
		fi
		addWds2Br0

		#
		# setup ip alias for user to access web page.
		#
		ifconfig eth2:1 172.32.1.254 netmask 255.255.255.0 up
	fi

	wan.sh
	lan.sh
	nat.sh
elif [ "$opmode" = "2" ]; then
	if [ "$CONFIG_RAETH_ROUTER" = "y" -a "$CONFIG_LAN_WAN_SUPPORT" = "y" ]; then
		echo "##### restore IC+ to dump switch #####"
		config-vlan.sh 0 0
	fi
	if [ "$CONFIG_MAC_TO_MAC_MODE" = "y" ]; then
		echo "##### restore Vtss to dump switch #####"
		config-vlan.sh 1 0
	fi
	if [ "$CONFIG_RT_3052_ESW" = "y" ]; then
		echo "##### restore RT3052 to dump switch #####"
		config-vlan.sh 2 0
	fi
	wan.sh
	lan.sh
	nat.sh
elif [ "$opmode" = "3" ]; then
	if [ "$CONFIG_RAETH_ROUTER" = "y" -o "$CONFIG_MAC_TO_MAC_MODE" = "y" -o "$CONFIG_RT_3052_ESW" = "y" ]; then
		if [ "$CONFIG_LAN_WAN_SUPPORT" = "y" ]; then
			echo "##### restore IC+ to dump switch #####"
			config-vlan.sh 0 0
		fi
		if [ "$CONFIG_MAC_TO_MAC_MODE" = "y" ]; then
			echo "##### restore Vtss to dump switch #####"
			config-vlan.sh 1 0
		fi
		if [ "$CONFIG_RT_3052_ESW" = "y" ]; then
			echo "##### restore RT3052 to dump switch #####"
			config-vlan.sh 2 0
		fi
		addBr0
		brctl addif br0 eth2
	fi
	wan.sh
	lan.sh
	nat.sh
else
	echo "unknown OperationMode: $opmode"
                                echo '##### config RT3052 vlan partition (WLLLL) #####'
                                config-vlan.sh 2 WLLLL
                addBr0
                brctl addif br0 eth2.1
                if [ "$CONFIG_RT2860V2_AP_MBSS" = "y" -a "$bssidnum" != "1" ]; then
                        addRax2Br0
                fi
                addWds2Br0
                addRaix2Br0
                addInicWds2Br0
                addRaL02Br0
	exit 1
fi

# in order to use broadcast IP address in L2 management daemon
if [ "$CONFIG_ICPLUS_PHY" = "y" ]; then
	route add -host 255.255.255.255 dev $wan_if
else
	route add -host 255.255.255.255 dev $lan_if
fi


m2uenabled=`nvram_get 2860 M2UEnabled`
if [ "$m2uenabled" = "1" ]; then
	iwpriv ra0 set IgmpSnEnable=1
	echo "iwpriv ra0 set IgmpSnEnable=1"
fi

