#!/bin/sh

#######################################################
# configure LAN/WAN switch particion and mode per port
# This is high level switch configure helper for Wive
#######################################################

# include global
. /etc/scripts/global.sh

LOG="logger -t ESW"

# get need variables
eval `nvram_buf_get 2860 wan_port tv_port vlan_double_tag offloadMode ForceRenewDHCP`

##############################################################################
# BASE FOR ALL ESW
##############################################################################
start_sw_config() {
    ##########################################################################
    # get proc path for phy configure
    ##########################################################################
    if [ -f /proc/rt2880/gmac ]; then
	PROC="/proc/rt2880/gmac"
	SWITCH_MODE=1
    elif [ -f /proc/rt3052/gmac ]; then
	PROC="/proc/rt3052/gmac"
	SWITCH_MODE=2
    elif [ -f /proc/rt3352/gmac ]; then
	PROC="/proc/rt3352/gmac"
	SWITCH_MODE=2
    elif [ -f /proc/rt5350/gmac ]; then
	PROC="/proc/rt5350/gmac"
	SWITCH_MODE=2
    elif [ -f /proc/rt2883/gmac ]; then
	PROC="/proc/rt2883/gmac"
	SWITCH_MODE=2
    elif [ -f /proc/rt3883/gmac ]; then
	PROC="/proc/rt3883/gmac"
	SWITCH_MODE=2
    elif [ -f /proc/rt6855/gmac ]; then
	PROC="/proc/rt6855/gmac"
	SWITCH_MODE=3
    elif [ -f /proc/rt63365/gmac ]; then
	PROC="/proc/rt63365/gmac"
	SWITCH_MODE=3
    elif [ -f /proc/mt7620/gmac ]; then
	PROC="/proc/mt7620/gmac"
	SWITCH_MODE=3
    else
	$LOG "No switch in system!!!"
	PROC=
	SWITCH_MODE=
    fi

    ##########################################################################
    # Configure double vlan tag support in kernel. Only one per start
    ##########################################################################
    if [ ! -f /tmp/bootgood ] && [ -f /proc/sys/net/ipv4/vlan_double_tag ]; then
	if [ "$vlan_double_tag" = "1" ]; then
	    if [ "$offloadMode" = "2" ] || [ "$offloadMode" = "3" ]; then
	        $LOG "Double vlan tag and HW_NAT enabled. HW_VLAN offload disabled."
	    else
	        $LOG "Double vlan tag enabled. HW_VLAN and HW_NAT offload disabled."
	    fi
	    DOUBLE_TAG=1
	else
	    $LOG "Double vlan tag and HW_NAT disabled. HW_VLAN offload enabled."
	    DOUBLE_TAG=0
	fi
	sysctl -wq net.ipv4.vlan_double_tag="$DOUBLE_TAG"
    fi
}

##########################################################################
# call this function only if VLAN as WAN need
##########################################################################
configs_system_vlans() {
    if [ ! -f /tmp/bootgood ]; then
	##########################################################################
	# Configure vlans in kernel. Only one per start
	##########################################################################
	$LOG "ROOT_MACADDR $LAN_MAC_ADDR"
	ifconfig eth2 hw ether "$LAN_MAC_ADDR"
	ip link set eth2 up
	# only if not bridge and not ethernet converter mode
	if [ "$OperationMode" != "0" ] && [ "$OperationMode" != "2" ]  && [ "$OperationMode" != "3" ]; then
	    $LOG "Add vlans interfaces"
	    if [ ! -d /proc/sys/net/ipv4/conf/eth2.1 ]; then
		vconfig add eth2 1
	    fi
	    if [ ! -d /proc/sys/net/ipv4/conf/eth2.2 ]; then
		vconfig add eth2 2
	    fi
	fi
    fi
}

##########################################################################
# call this function for set HW_ADDR to interfaces
##########################################################################
set_mac_wan_lan() {
    # set MAC adresses LAN for phys iface (always set for physycal external switch one or dual phy mode)
    if [ "$OperationMode" = "1" ] || [ "$OperationMode" = "4" ] || [ "$CONFIG_MAC_TO_MAC_MODE" = "y" ]; then
	# ALWAYS UP ROOT IFACE BEFORE CONFIGURE SECOND
	$LOG "$phys_lan_if MACADDR $LAN_MAC_ADDR"
	ifconfig "$phys_lan_if" down
	ifconfig "$phys_lan_if" hw ether "$LAN_MAC_ADDR" txqueuelen "$txqueuelen" up
    fi

    # set MAC adresses LAN/WAN if not bridge and not ethernet converter modes
    # in gw/hotspot modes set mac to wan (always set for physycal external dual phy mode swicth)
    if [ "$OperationMode" = "1" ] || [ "$OperationMode" = "4" ] || [ "$CONFIG_RAETH_GMAC2" = "y" ]; then
	# ROOT IFACE MUST BE READY AND ENABLED
	$LOG "$phys_wan_if MACADDR $WAN_MAC_ADDR"
	ifconfig "$phys_wan_if" down
	ifconfig "$phys_wan_if" hw ether "$WAN_MAC_ADDR" txqueuelen "$txqueuelen"
    fi
}

##########################################################################
# call this function only for rtl8367 external switch
##########################################################################
esw_rtl8367_config() {
    if [ "$CONFIG_RTL8367M" != "" ] && [ -f /bin/rtl8367m ]; then
	# defines from rtl8367m_drv.h
	RTL8367M_IOCTL_BRIDGE_MODE=50
	RTL8367M_IOCTL_VLAN_RESET_TABLE=60
	RTL8367M_IOCTL_SPEED_PORT_XXXX=90
	RTL8367M_WAN_BWAN_ISOLATION_NONE=0
	RTL8367M_WAN_BWAN_ISOLATION_FROM_CPU=1
	RTL8367M_WAN_BRIDGE_DISABLE=0
	RTL8367M_WAN_BRIDGE_LAN1=1
	RTL8367M_WAN_BRIDGE_DISABLE_WAN=8
	##########################################################################
	# In gate mode and hotspot mode configure WAN bridge
	##########################################################################
	if [ "$OperationMode" = "1" ] || [ "$OperationMode" = "4" ]; then
		if [ "$tv_port" = "1" ]; then
		    $LOG '##### ESW config vlan partition (WWLLL) #####'
		    rtl8367m $RTL8367M_IOCTL_BRIDGE_MODE $RTL8367M_WAN_BRIDGE_LAN1 $RTL8367M_WAN_BWAN_ISOLATION_FROM_CPU
		else
		    $LOG '##### ESW config vlan partition (WLLLL) #####'
		rtl8367m $RTL8367M_IOCTL_BRIDGE_MODE $RTL8367M_WAN_BRIDGE_DISABLE $RTL8367M_WAN_BWAN_ISOLATION_NONE
	    fi
		fi
	##########################################################################
	# In bridge, eth converter and apcli mode, neded config switch to LLLLL
	##########################################################################
	if [ "$OperationMode" = "0" ] || [ "$OperationMode" = "2" ] || [ "$OperationMode" = "3" ]; then
	    # reset switch to LLLLL and disable VLAN only for one PHY mode (eth2 used in the soft bridge)
	    if [ "$CONFIG_RAETH_GMAC2" = "" ]; then
		$LOG '##### ESW disable vlan partitions (LLLLL) #####'
		rtl8367m $RTL8367M_IOCTL_VLAN_RESET_TABLE
		rtl8367m $RTL8367M_IOCTL_BRIDGE_MODE $RTL8367M_WAN_BRIDGE_DISABLE_WAN
	    else
		$LOG '##### ESW set default partition (WLLLL) #####'
		rtl8367m $RTL8367M_IOCTL_BRIDGE_MODE $RTL8367M_WAN_BRIDGE_DISABLE $RTL8367M_WAN_BWAN_ISOLATION_NONE
	    fi
	fi
	##########################################################################
	# Set speed and duplex modes per port
	##########################################################################
	for i in `seq 1 5`; do
	    # assume that port id is 1=WAN, 2=LAN1, 3=LAN2, 4=LAN3, 5=LAN4
	    ioctl_arg=$(( $RTL8367M_IOCTL_SPEED_PORT_XXXX + $i - 1 ))
	    # get mode for current port
	    port_swmode=`nvram_get 2860 port"$i"_swmode`
	    if [ "$port_swmode" != "auto" ] && [ "$port_swmode" != "" ]; then
		$LOG ">>> Port ID $i set mode $port_swmode <<<"
		if [ "$port_swmode" = "1000f" ]; then
		    #set 1000Mbit full duplex and start negotinate
		    rtl8367m $ioctl_arg 1
		elif [ "$port_swmode" = "100f" ]; then
		    #set 100Mbit full duplex and start negotinate
		    rtl8367m $ioctl_arg 2
		elif [ "$port_swmode" = "100h" ]; then
		    #set 100Mbit half duplex and start negotinate
		    rtl8367m $ioctl_arg 3
		elif [ "$port_swmode" = "10f" ]; then
		    #set 10Mbit full duplex and start negotinate
		    rtl8367m $ioctl_arg 4
		elif [ "$port_swmode" = "10h" ]; then
		    #set 10Mbit half duplex and start negotinate
		    rtl8367m $ioctl_arg 5
		fi
	    elif [ "$port_swmode" = "auto" ]; then
		# enable full auto and start negotinate
		rtl8367m $ioctl_arg 0
	    fi
	done
    else
	$LOG "rtl8367m tool not found in firmware or kernel support rtl8367 not configured !!!"
    fi
}

##############################################################################
# preconfig
start_sw_config
##############################################################################

##############################################################################
# Internal 3052 ESW
##############################################################################
if [ "$CONFIG_RT_3052_ESW" != "" ] && [ "$SWITCH_MODE" != "" ]; then
    configs_system_vlans
    if [ ! -f /tmp/bootgood ] && [ "$CONFIG_RALINK_RT3052" != "" ]; then
	######################################################################
	# workaroud for dir-300NRU and some ithers devices
	# with not correct configured from uboot
	# need only start boot
	######################################################################
	$LOG "Reinit power mode for all switch ports"
	/etc/scripts/config-vlan.sh $SWITCH_MODE FFFFF > /dev/null 2>&1
    fi
    ##########################################################################
    $LOG '######### Clear switch partition  ###########'
    /etc/scripts/config-vlan.sh $SWITCH_MODE 0 > /dev/null 2>&1
    ##########################################################################
    # Set speed and duplex modes per port
    ##########################################################################
    if [ -f /bin/ethtool ] && [ "$PROC" != "" ]; then
	##################################
	# start configure by ethtool
	##################################
	phys_portN=4
	for i in `seq 1 5`; do
	    # select switch port for tune
	    echo "$phys_portN" > $PROC
	    # get mode for current port
	    port_swmode=`nvram_get 2860 port"$i"_swmode`
	    if [ "$port_swmode" != "auto" ] && [ "$port_swmode" != "" ]; then
		$LOG ">>> Port $phys_portN set mode $port_swmode <<<"
		# first disable autoneg
		ethtool -s eth2 autoneg off > /dev/null 2>&1
		if [ "$port_swmode" = "100f" ]; then
		    #set 100Mbit full duplex and start negotinate
		    ethtool -s eth2 autoneg on speed 100 duplex full	> /dev/null 2>&1
		elif [ "$port_swmode" = "100h" ]; then
		    #set 100Mbit half duplex and start negotinate
		    ethtool -s eth2 autoneg on speed 100 duplex half	> /dev/null 2>&1
		elif [ "$port_swmode" = "10f" ]; then
		    #set 10Mbit full duplex and start negotinate
		    ethtool -s eth2 autoneg on speed 10 duplex full	> /dev/null 2>&1
		elif [ "$port_swmode" = "10h" ]; then
		    #set 10Mbit half duplex and start negotinate
		    ethtool -s eth2 autoneg on speed 10 duplex half	> /dev/null 2>&1
		fi
	    elif [ "$port_swmode" = "auto" ]; then
		# enable autoneg
		ethtool -s eth2 autoneg on > /dev/null 2>&1
	    fi
	let "phys_portN=$phys_portN-1"
	done
    fi
    ##########################################################################
    # In gate mode and hotspot mode configure vlans
    ##########################################################################
    if [ "$OperationMode" = "1" ] || [ "$OperationMode" = "4" ]; then
	if [ "$wan_port" = "0" ]; then
	    if [ "$tv_port" = "1" ]; then
		$LOG '##### ESW config vlan partition (WWLLL) #####'
		/etc/scripts/config-vlan.sh $SWITCH_MODE WWLLL > /dev/null 2>&1
	    else
		$LOG '##### ESW config vlan partition (WLLLL) #####'
		/etc/scripts/config-vlan.sh $SWITCH_MODE WLLLL > /dev/null 2>&1
	    fi
	else
	    if [ "$tv_port" = "1" ]; then
		$LOG '##### ESW config vlan partition (LLLWW) #####'
		/etc/scripts/config-vlan.sh $SWITCH_MODE LLLWW > /dev/null 2>&1
	    else
		$LOG '##### ESW config vlan partition (LLLLW) #####'
		/etc/scripts/config-vlan.sh $SWITCH_MODE LLLLW > /dev/null 2>&1
	    fi
	fi
    fi
    ##########################################################################
    # Configure touch dhcp from driver in kernel.
    ##########################################################################
    if [ "$CONFIG_RAETH_DHCP_TOUCH" != "" ]; then
	if [ "$OperationMode" = "0" ] || [ "$OperationMode" = "2" ] || [ "$OperationMode" = "3" ]; then
	    # disable dhcp renew from driver
	    sysctl -wq net.ipv4.send_sigusr_dhcpc=9
	else
	    if [ "$ForceRenewDHCP" != "0" ] && [ "$wan_port" != "" ]; then
		# configure event wait port
		sysctl -wq net.ipv4.send_sigusr_dhcpc="$wan_port"
	    else
		# disable dhcp renew from driver
		sysctl -wq net.ipv4.send_sigusr_dhcpc=9
	    fi
	fi
    fi
    ##########################################################################
    # Configure double vlan tag and eneble forward
    ##########################################################################
    if [ -f /proc/sys/net/ipv4/vlan_double_tag ]; then
	if [ "$DOUBLE_TAG" = "1" ]; then
	    DOUBLE_TAG=3f
	else
	    DOUBLE_TAG=0
	fi
	# double vlan tag support enable/disable
	switch reg w e4 $DOUBLE_TAG
    fi
##############################################################################
# RTL8367M external switch dual phy mode
##############################################################################
elif [ "$CONFIG_MAC_TO_MAC_MODE" != "" ] && [ "$CONFIG_RAETH_GMAC2" != "" ]; then
    SWITCH_MODE=1
    ##########################################################################
    if [ "$CONFIG_RTL8367M" != "" ]; then
	$LOG '##### config switch partition (RTL DUAL PHY) #####'
	esw_rtl8367_config
    fi
##############################################################################
# RTL8367M external switch one phy mode
##############################################################################
elif [ "$CONFIG_MAC_TO_MAC_MODE" != "" ] && [ "$CONFIG_RAETH_GMAC2" = "" ]; then
    SWITCH_MODE=1
    configs_system_vlans
    ##########################################################################
    if [ "$CONFIG_RTL8367M" != "" ]; then
	$LOG '##### config vlan partition (RTL ONE PHY) #####'
	esw_rtl8367_config
    fi
fi

##############################################################################
# set hwaddresses to wan/lan interfaces
set_mac_wan_lan
##############################################################################
