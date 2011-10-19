#######################################################
# configure LAN/WAN switch particion and mode per port
# This is high level switch configure helper for Wive
#######################################################

# include kernel config
. /etc/scripts/config.sh

# get need variables
wan_port=`nvram_get 2860 wan_port`
opmode=`nvram_get 2860 OperationMode`
tv_port=`nvram_get 2860 tv_port`
vlan_double_tag=`nvram_get 2860 vlan_double_tag`
natFastpath=`nvram_get 2860 natFastpath`

##############################################################################
# Internal 3052 ESW
##############################################################################
if [ "$CONFIG_RT_3052_ESW" != "" ]; then
    SWITCH_MODE=2
    if [ ! -f /var/run/goahead.pid ]; then
	######################################################################
	# workaroud for dir-300NRU and some devices
	# with not correct configured from uboot
	# need only start boot
	######################################################################
	echo "Reinit power mode for all switch ports"
	/etc/scripts/config-vlan.sh $SWITCH_MODE FFFFF > /dev/null 2>&1
    fi
    ##########################################################################
    echo '######### Clear switch partition  ###########'
    /etc/scripts/config-vlan.sh $SWITCH_MODE 0 > /dev/null 2>&1
    ##########################################################################
    # Set speed and duplex modes per port
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
	echo "No swith in system!!!"
	PROC=
    fi
    if [ -f /bin/ethtool ] && [ "$PROC" != "" ]; then
	##################################
	# start configure by ethtool
	##################################
	phys_portN=4
	for i in `seq 1 5`; do
	    port_swmode=`nvram_get 2860 port"$i"_swmode`
	    if [ "$port_swmode" != "auto" ] && [ "$port_swmode" != "" ]; then
		echo ">>> Port $phys_portN set mode $port_swmode <<<"
		echo "$phys_portN" > $PROC
		#first disable autoneg
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
	    fi
	let "phys_portN=$phys_portN-1"
	done
    fi
    ##########################################################################
    # In gate mode and hotspot mode configure vlans
    ##########################################################################
    if [ "$opmode" = "1" ] || [ "$opmode" = "4" ]; then
	if [ "$wan_port" = "0" ]; then
	    if [ "$tv_port" = "1" ]; then
		echo '##### ESW config vlan partition (WWLLL) #####'
		/etc/scripts/config-vlan.sh $SWITCH_MODE WWLLL > /dev/null 2>&1
	    else
		echo '##### ESW config vlan partition (WLLLL) #####'
		/etc/scripts/config-vlan.sh $SWITCH_MODE WLLLL > /dev/null 2>&1
	    fi
	else
	    if [ "$tv_port" = "1" ]; then
		echo '##### ESW config vlan partition (LLLWW) #####'
		/etc/scripts/config-vlan.sh $SWITCH_MODE LLLWW > /dev/null 2>&1
	    else
		echo '##### ESW config vlan partition (LLLLW) #####'
		/etc/scripts/config-vlan.sh $SWITCH_MODE LLLLW > /dev/null 2>&1
	    fi
	fi
    fi
    ##########################################################################
    # Configure double vlan tag and eneble forward
    ##########################################################################
    if [ -f /proc/sys/net/ipv4/vlan_double_tag ]; then
	if [ "$vlan_double_tag" = "1" ] || [ "$natFastpath" = "2" ] || [ "$natFastpath" = "3" ]; then
	    DOUBLE_TAG=3f
	else
	    DOUBLE_TAG=0
	fi
    fi
    # double vlan tag support enable/disable
    switch reg w e4 $DOUBLE_TAG
    ##########################################################################
    echo '######### Clear switch mac table  ###########'
    switch clear > /dev/null 2>&1
##############################################################################
# VTSS external switch
##############################################################################
elif [ "$CONFIG_MAC_TO_MAC_MODE" != "" ]; then
    SWITCH_MODE=1
    ##########################################################################
    echo '######## clear switch partition  ########'
    /etc/scripts/config-vlan.sh $SWITCH_MODE 0 > /dev/null 2>&1
    echo '##### config vlan partition (VTSS) #####'
    /etc/scripts/config-vlan.sh $SWITCH_MODE 1 > /dev/null 2>&1
##############################################################################
# IC+ external switch
##############################################################################
elif [ "$CONFIG_RAETH_ROUTER" != "" ]; then
    SWITCH_MODE=0
    ##########################################################################
    echo '######## clear switch partition  ########'
    /etc/scripts/config-vlan.sh $SWITCH_MODE 0 > /dev/null 2>&1
    ##########################################################################
    # In gate mode and hotspot mode configure vlans
    ##########################################################################
    if [ "$opmode" = "1" ] || [ "$opmode" = "4" ]; then
	if [ "$wan_port" = "0" ]; then
	    echo '##### IC+ config vlan partition (WLLLL) #####'
	    /etc/scripts/config-vlan.sh $SWITCH_MODE WLLLL > /dev/null 2>&1
	else
	    echo '##### IC+ config vlan partition (LLLLW) #####'
	    /etc/scripts/config-vlan.sh $SWITCH_MODE LLLLW > /dev/null 2>&1
	fi
    fi
fi
