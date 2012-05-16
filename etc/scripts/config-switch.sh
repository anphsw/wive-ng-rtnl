#######################################################
# configure LAN/WAN switch particion and mode per port
# This is high level switch configure helper for Wive
#######################################################

# include kernel config
. /etc/scripts/config.sh

LOG="logger -t ESW"

# get need variables
eval `nvram_buf_get 2860 wan_port OperationMode tv_port vlan_double_tag natFastpath ForceRenewDHCP LAN_MAC_ADDR`

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
    PROC="/proc/rt23883/gmac"
elif [ -f /proc/rt6855/gmac ]; then
    PROC="/proc/rt6855/gmac"
elif [ -f /proc/rt63365/gmac ]; then
    PROC="/proc/rt63365/gmac"
else
    $LOG "No switch in system!!!"
    PROC=
fi

##############################################################################
# BASE FOR ALL ESW
##############################################################################
if [ ! -f /var/run/goahead.pid ]; then
    ##########################################################################
    # Configure touch dhcp from driver in kernel. Only one per start
    ##########################################################################
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
    ##########################################################################
    # Configure double vlan tag support in kernel. Only one per start
    ##########################################################################
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

##############################################################################
# Internal 3052 ESW
##############################################################################
if [ "$CONFIG_RT_3052_ESW" != "" ]; then
    SWITCH_MODE=2
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
    $LOG '######## clear switch partition (DUAL_PHY) ########'
    /etc/scripts/config-vlan.sh $SWITCH_MODE 0 > /dev/null 2>&1
    if [ "$CONFIG_RTL8367M" != "" ]; then
	# put code for configure switch port mode and others
	$LOG "Need add code for config RTL switch mode"
    fi
##############################################################################
# VTSS OR RTL8367M external switch one phy mode
##############################################################################
elif [ "$CONFIG_MAC_TO_MAC_MODE" != "" ] && [ "$CONFIG_RAETH_GMAC2" = "" ]; then
    SWITCH_MODE=1
    ##########################################################################
    $LOG '######## clear switch partition  ########'
    /etc/scripts/config-vlan.sh $SWITCH_MODE 0 > /dev/null 2>&1
    if [ "$CONFIG_RTL8367M" != "" ]; then
	$LOG '##### config vlan partition (RTL ONE PHY) #####'
	$LOG '##### RTL ONE PHY - NOT SUPPORTED #####'
    else
	$LOG '##### config vlan partition (VTTS ONE PHY) #####'
	/etc/scripts/config-vlan.sh $SWITCH_MODE 1 > /dev/null 2>&1
    fi
##############################################################################
# IC+ external switch
##############################################################################
elif [ "$CONFIG_RAETH_ROUTER" != "" ]; then
    SWITCH_MODE=0
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
