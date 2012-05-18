#######################################################
# configure LAN/WAN switch particion and mode per port
# This is high level switch configure helper for Wive
#######################################################

# include global
. /etc/scripts/global.sh

LOG="logger -t ESW"

# get need variables
eval `nvram_buf_get 2860 wan_port tv_port vlan_double_tag natFastpath ForceRenewDHCP`

##############################################################################
# BASE FOR ALL ESW
##############################################################################
start_sw_config() {
    ##########################################################################
    # get proc path for phy configure
    ##########################################################################
    if [ -f /proc/rt2880/gmac ]; then
	PROC="/proc/rt2880/gmac"
    elif [ -f /proc/rt3052/gmac ]; then
	PROC="/proc/rt3052/gmac"
    elif [ -f /proc/rt3352/gmac ]; then
	PROC="/proc/rt3352/gmac"
    elif [ -f /proc/rt5350/gmac ]; then
	PROC="/proc/rt5350/gmac"
    elif [ -f /proc/rt2883/gmac ]; then
	PROC="/proc/rt2883/gmac"
    elif [ -f /proc/rt3883/gmac ]; then
	PROC="/proc/rt3883/gmac"
    elif [ -f /proc/rt6855/gmac ]; then
	PROC="/proc/rt6855/gmac"
    elif [ -f /proc/rt63365/gmac ]; then
	PROC="/proc/rt63365/gmac"
    else
	$LOG "No switch in system!!!"
	PROC=
    fi

    ##########################################################################
    # Configure double vlan tag support in kernel. Only one per start
    ##########################################################################
    if [ ! -f /var/run/goahead.pid ]; then
	if [ -f /proc/sys/net/ipv4/vlan_double_tag ]; then
	    if [ "$vlan_double_tag" = "1" ] || [ "$natFastpath" = "2" ] || [ "$natFastpath" = "3" ]; then
		if [ "$natFastpath" = "2" ] || [ "$natFastpath" = "3" ]; then
		    $LOG "Double vlan tag and HW_NAT enabled. HW_VLAN offload disabled."
		else
		    $LOG "Double vlan tag enabled. HW_VLAN and HW_NAT offload disabled."
		fi
		DOUBLE_TAG=1
	    else
		$LOG "Double vlan tag and HW_NAT disabled. HW_VLAN offload enabled."
		DOUBLE_TAG=0
	    fi
	    sysctl -w net.ipv4.vlan_double_tag="$DOUBLE_TAG"
	fi
    fi
}

##########################################################################
# call this function only if VLAN as WAN need
##########################################################################
configs_system_vlans() {
    if [ ! -f /var/run/goahead.pid ]; then
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
	RTL8367M_IOCTL_SPEED_PORT_XXXX=90
	RTL8367M_WAN_BWAN_ISOLATION_NONE=0
	RTL8367M_WAN_BWAN_ISOLATION_FROM_CPU=1
	RTL8367M_WAN_BWAN_ISOLATION_BETWEEN=2
	RTL8367M_WAN_BRIDGE_DISABLE=0
	RTL8367M_WAN_BRIDGE_LAN1=1
	RTL8367M_WAN_BRIDGE_LAN2=2
	RTL8367M_WAN_BRIDGE_LAN3=3
	RTL8367M_WAN_BRIDGE_LAN4=4
	RTL8367M_WAN_BRIDGE_LAN3_LAN4=5
	RTL8367M_WAN_BRIDGE_LAN1_LAN2=6
	RTL8367M_WAN_BRIDGE_LAN1_LAN2_LAN3=7
	##########################################################################
	# In gate mode and hotspot mode configure WAN bridge
	##########################################################################
	if [ "$OperationMode" = "1" ] || [ "$OperationMode" = "4" ]; then
	    if [ "$wan_port" = "0" ]; then
		if [ "$tv_port" = "1" ]; then
		    $LOG '##### ESW config vlan partition (WWLLL) #####'
		    rtl8367m $RTL8367M_IOCTL_BRIDGE_MODE $RTL8367M_WAN_BRIDGE_LAN1 $RTL8367M_WAN_BWAN_ISOLATION_FROM_CPU
		else
		    $LOG '##### ESW config vlan partition (WLLLL) #####'
		    rtl8367m $RTL8367M_IOCTL_BRIDGE_MODE $RTL8367M_WAN_BRIDGE_DISABLE
		fi
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
if [ "$CONFIG_RT_3052_ESW" != "" ]; then
    SWITCH_MODE=2
    configs_system_vlans
    if [ ! -f /var/run/goahead.pid ]; then
	######################################################################
	# workaroud for dir-300NRU and some devices
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
        if [ "$OperationMode" = "0" ] || [ "$OperationMode" = "2" ] || [ "$ApCliBridgeOnly" = "1" ]; then
	    # disable dhcp renew from driver
	    sysctl -w net.ipv4.send_sigusr_dhcpc=9
	else
	    if [ "$ForceRenewDHCP" != "0" ] && [ "$wan_port" != "" ]; then
		# configure event wait port
		sysctl -w net.ipv4.send_sigusr_dhcpc=$wan_port
	    else
		# disable dhcp renew from driver
		sysctl -w net.ipv4.send_sigusr_dhcpc=9
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
    ##########################################################################
    $LOG '######### Clear switch mac table  ###########'
    switch clear > /dev/null 2>&1
##############################################################################
# VTSS OR RTL8367M external switch dual phy mode
##############################################################################
elif [ "$CONFIG_MAC_TO_MAC_MODE" != "" ] && [ "$CONFIG_RAETH_GMAC2" != "" ]; then
    SWITCH_MODE=1
    ##########################################################################
    if [ "$CONFIG_RTL8367M" != "" ]; then
	# put code for configure switch port mode and others
	$LOG '######## need add code for config RTL switch mode ######'
	esw_rtl8367_config 2
    else
	$LOG '######## clear switch partition (VTTS DUAL_PHY) ########'
	/etc/scripts/config-vlan.sh $SWITCH_MODE 0 > /dev/null 2>&1
    fi
##############################################################################
# VTSS OR RTL8367M external switch one phy mode
##############################################################################
elif [ "$CONFIG_MAC_TO_MAC_MODE" != "" ] && [ "$CONFIG_RAETH_GMAC2" = "" ]; then
    SWITCH_MODE=1
    configs_system_vlans
    ##########################################################################
    if [ "$CONFIG_RTL8367M" != "" ]; then
	$LOG '##### config vlan partition (RTL ONE PHY) #####'
	# this is sub need kernel level 8367 support driver fix
	esw_rtl8367_config 1
    else
	$LOG '##### clear switch partition (VTTS ONE_PHY) ########'
	/etc/scripts/config-vlan.sh $SWITCH_MODE 0 > /dev/null 2>&1
	$LOG '##### config vlan partition (VTTS ONE_PHY)  ########'
	/etc/scripts/config-vlan.sh $SWITCH_MODE 1 > /dev/null 2>&1
    fi
##############################################################################
# IC+ external switch
##############################################################################
elif [ "$CONFIG_RAETH_ROUTER" != "" ]; then
    SWITCH_MODE=0
    configs_system_vlans
    ##########################################################################
    $LOG '######## clear switch partition  ########'
    /etc/scripts/config-vlan.sh $SWITCH_MODE 0 > /dev/null 2>&1
    ##########################################################################
    # In gate mode and hotspot mode configure vlans
    ##########################################################################
    if [ "$OperationMode" = "1" ] || [ "$OperationMode" = "4" ]; then
	if [ "$wan_port" = "0" ]; then
	    $LOG '##### IC+ config vlan partition (WLLLL) #####'
	    /etc/scripts/config-vlan.sh $SWITCH_MODE WLLLL > /dev/null 2>&1
	else
	    $LOG '##### IC+ config vlan partition (LLLLW) #####'
	    /etc/scripts/config-vlan.sh $SWITCH_MODE LLLLW > /dev/null 2>&1
	fi
    fi
fi

##############################################################################
# set hwaddresses to wan/lan interfaces
set_mac_wan_lan
##############################################################################
