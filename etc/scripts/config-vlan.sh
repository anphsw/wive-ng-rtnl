#!/bin/sh

############################################################
# config-vlan.sh - configure vlan switch particion helper  #
#                                                          #
# usage: config-vlan.sh <switch_type> <vlan_type>          #
#   switch_type: 0=IC+, 1=vtss, 2=esw3050x                 #
#   vlan_type: 0=no_vlan, 1=vlan, LLLLW=wan_4, WLLLL=wan_0 #
############################################################

usage()
{
	echo "Usage:"
	echo "  $0 0 0 - restore IC+ to no VLAN partition"
	echo "  $0 0 LLLLW - config IC+ with VLAN and WAN at port 4"
	echo "  $0 0 WLLLL - config IC+ with VLAN and WAN at port 0"
	echo "  $0 1 0 - restore Vtss to no VLAN partition"
	echo "  $0 1 1 - config Vtss with VLAN partition"
	echo "  $0 2 0 - restore RT3052 to no VLAN partition"
	echo "  $0 2 EEEEE - config RT3052 Enable all ports 100FD"
	echo "  $0 2 DDDDD - config RT3052 Disable all ports"
	echo "  $0 2 RRRRR - config RT3052 Reset all ports"
	echo "  $0 2 FFFFF - config RT3052 Full reinit switch"
	echo "  $0 2 LLLLW - config RT3052 with LAN at ports 0-3 and WAN at port 4"
	echo "  $0 2 WLLLL - config RT3052 with LAN at ports 1-4 and WAN at port 0"
	echo "  $0 2 LLLWW - config RT3052 with LAN at ports 0-2 and WAN at port 3-4"
	echo "  $0 2 WWLLL - config RT3052 with LAN at ports 2-4 and WAN at port 0-1"
        echo "  $0 2 12345 - config RT3052 with individual VLAN 1~5 at port 0~4"
        echo "  $0 2 xxxxx - config RT3052 with user defined VLAN(1-6): 112345,443124 on ports 0-4 and Gigabit"
	echo "  $0 2 GW - config RT3052 with WAN at Giga port"
	echo "  $0 2 GS - config RT3052 with Giga port connecting to an external switch"
	exit 0
}

config3052_dt()
{
	vlan_double_tag=`nvram_get 2860 vlan_double_tag`
        if [ "$vlan_double_tag" = "1" ]; then
	    switch reg w e4 3f
            sysctl -w net.ipv4.vlan_double_tag=1
        else
	    switch reg w e4 0
            sysctl -w net.ipv4.vlan_double_tag=0
        fi
}

config3052()
{
	#preinit
	switch reg w 14 405555
	switch reg w 50 2001
	switch reg w 98 7f3f

	# doble wlan tag config
	config3052_dt

	# Calculating PVID on ports 1 and 0
	r40=`printf "%x" $((($2<<12)|$1))`

	# Calculating PVID on ports 3 and 2
	r44=`printf "%x" $((($4<<12)|$3))`

	# Calculating PVID on port 4 and Gigabit (P5) if it exists
	if [ $# -lt 6 ]; then
		r48=`printf "%x" $5`
	else
		r48=`printf "%x" $((($6<<12)|$5))`
	fi

	# Writing configuration
	switch reg w 40 $r40
	switch reg w 44 $r44
	switch reg w 48 $r48

	# Calculating accessory of every port to different vlan

	# Each Vlan contains P6 (system port)
	r70=$(((1<<6)|(1<<14)|(1<<22)|(1<<30)))
	r74=$r70


	j=0

	# Calculating bitmasks
	for i 
		do
		if [ $i -lt 5 ]; then
			r70=$(($r70|(1<<(j+8*($i-1)))))
		else
			r74=$(($r74|(1<<(j+8*($i-5)))))
		fi
		j=$(($j+1))
	done

	# Translating to hex
	r70=`printf "%x" $r70`
	r74=`printf "%x" $r74`

	# Writing configuration
	switch reg w 70 $r70
	switch reg w 74 $r74
}

restore3052()
{
        switch reg w 14 5555
        switch reg w 40 1001
        switch reg w 44 1001
        switch reg w 48 1001
        switch reg w 4c 1
        switch reg w 50 2001
        switch reg w 70 ffffffff
        switch reg w 98 7f7f
        switch reg w e4 7f

	# doble wlan tag config
	config3052_dt
}

disable3052()
{
    for i in `seq 0 4`; do
	mii_mgr -s -p $i -r 0 -v 0x0800
    done
}

enable3052()
{
    for i in `seq 0 4`; do
	mii_mgr -s -p $i -r 0 -v 0x9000
    done
}

#  arg1:  phy address.
link_down()
{
	# get original register value
	get_mii=`mii_mgr -g -p $1 -r 0`
	orig=`echo $get_mii | sed 's/^.....................//'`

	# stupid hex value calculation.
	pre=`echo $orig | sed 's/...$//'`
	post=`echo $orig | sed 's/^..//'` 
	num_hex=`echo $orig | sed 's/^.//' | sed 's/..$//'`
	case $num_hex in
		"0")	rep="8"	;;
		"1")	rep="9"	;;
		"2")	rep="a"	;;
		"3")	rep="b"	;;
		"4")	rep="c"	;;
		"5")	rep="d"	;;
		"6")	rep="e"	;;
		"7")	rep="f"	;;
		# The power is already down
		*)		echo "Warning in PHY reset script";return;;
	esac
	new=$pre$rep$post
	# power down
	mii_mgr -s -p $1 -r 0 -v $new
}

link_up()
{
	# get original register value
	get_mii=`mii_mgr -g -p $1 -r 0`
	orig=`echo $get_mii | sed 's/^.....................//'`

	# stupid hex value calculation.
	pre=`echo $orig | sed 's/...$//'`
	post=`echo $orig | sed 's/^..//'` 
	num_hex=`echo $orig | sed 's/^.//' | sed 's/..$//'`
	case $num_hex in
		"8")	rep="0"	;;
		"9")	rep="1"	;;
		"a")	rep="2"	;;
		"b")	rep="3"	;;
		"c")	rep="4"	;;
		"d")	rep="5"	;;
		"e")	rep="6"	;;
		"f")	rep="7"	;;
		# The power is already up
		*)		echo "Warning in PHY reset script";return;;
	esac
	new=$pre$rep$post
	# power up
	mii_mgr -s -p $1 -r 0 -v $new
}

reset_all_phys()
{
	if [ "$SWITCH_MODE" != "0" ] && [ "$SWITCH_MODE" != "2" ]; then
		return
	fi

	echo "Reset all phy port"
        opmode=`nvram_get 2860 OperationMode`
	if [ "$opmode" = "1" ]; then
	    #Ports down skip WAN port
	    wan_port=`nvram_get 2860 wan_port`
	    if [ "$wan_portN" = "0" ]; then
		start=0
		end=3
	    else
		start=1	
		end=4
	    fi
	else
	    #All ports down
	    start=0
	    end=4
	fi

	#disable ports
	for i in `seq $start $end`; do
    	    link_down $i
	done

	if [ "$opmode" = "1" ]; then
	  #force Windows clients to renew IP and update DNS server
	  sleep 2
	fi

	#enable ports
	for i in `seq $start $end`; do
    	    link_up $i
	done
}

reinit_all_phys()
{
	disable3052
	enable3052 
	reset_all_phys
}

config175C()
{
	mii_mgr -s -p 29 -r 23 -v 0x07c2
	mii_mgr -s -p 29 -r 22 -v 0x8420

	if [ "$1" = "LLLLW" ]; then
		mii_mgr -s -p 29 -r 24 -v 0x1
		mii_mgr -s -p 29 -r 25 -v 0x1
		mii_mgr -s -p 29 -r 26 -v 0x1
		mii_mgr -s -p 29 -r 27 -v 0x1
		mii_mgr -s -p 29 -r 28 -v 0x2
		mii_mgr -s -p 30 -r 9 -v 0x1089
		mii_mgr -s -p 30 -r 1 -v 0x2f00
		mii_mgr -s -p 30 -r 2 -v 0x0030
	elif [ "$1" = "WLLLL" ]; then
		mii_mgr -s -p 29 -r 24 -v 0x2
		mii_mgr -s -p 29 -r 25 -v 0x1
		mii_mgr -s -p 29 -r 26 -v 0x1
		mii_mgr -s -p 29 -r 27 -v 0x1
		mii_mgr -s -p 29 -r 28 -v 0x1
		mii_mgr -s -p 30 -r 9 -v 0x0189
		mii_mgr -s -p 30 -r 1 -v 0x3e00
		mii_mgr -s -p 30 -r 2 -v 0x0021
	else
		echo "LAN WAN layout $0 is not suported"
	fi
}

restore175C()
{
	mii_mgr -s -p 29 -r 23 -v 0x0
	mii_mgr -s -p 29 -r 22 -v 0x420
	mii_mgr -s -p 29 -r 24 -v 0x1
	mii_mgr -s -p 29 -r 25 -v 0x1
	mii_mgr -s -p 29 -r 26 -v 0x1
	mii_mgr -s -p 29 -r 27 -v 0x1
	mii_mgr -s -p 29 -r 27 -v 0x2
	mii_mgr -s -p 30 -r 9 -v 0x1001
	mii_mgr -s -p 30 -r 1 -v 0x2f3f
	mii_mgr -s -p 30 -r 2 -v 0x3f30
}

restore175D()
{
	mii_mgr -s -p 20 -r  4 -v 0xa000
	mii_mgr -s -p 20 -r 13 -v 0x20
	mii_mgr -s -p 21 -r  1 -v 0x1800
	mii_mgr -s -p 22 -r  0 -v 0x0
	mii_mgr -s -p 22 -r  2 -v 0x0
	mii_mgr -s -p 22 -r 10 -v 0x0
	mii_mgr -s -p 22 -r 14 -v 0x1
	mii_mgr -s -p 22 -r 15 -v 0x2
	mii_mgr -s -p 23 -r  8 -v 0x0
	mii_mgr -s -p 23 -r 16 -v 0x0

	mii_mgr -s -p 22 -r 4 -v 0x1
	mii_mgr -s -p 22 -r 5 -v 0x1
	mii_mgr -s -p 22 -r 6 -v 0x1
	mii_mgr -s -p 22 -r 7 -v 0x1
	mii_mgr -s -p 22 -r 8 -v 0x1
	mii_mgr -s -p 23 -r 0 -v 0x3f3f
}

config175D()
{
	mii_mgr -s -p 20 -r  4 -v 0xa000
	mii_mgr -s -p 20 -r 13 -v 0x21
	mii_mgr -s -p 21 -r  1 -v 0x1800
	mii_mgr -s -p 22 -r  0 -v 0x27ff
	mii_mgr -s -p 22 -r  2 -v 0x20
	mii_mgr -s -p 22 -r  3 -v 0x8100
	mii_mgr -s -p 22 -r 10 -v 0x3
	mii_mgr -s -p 22 -r 14 -v 0x1001
	mii_mgr -s -p 22 -r 15 -v 0x2002
	mii_mgr -s -p 23 -r  8 -v 0x2020
	mii_mgr -s -p 23 -r 16 -v 0x1f1f
	if [ "$1" = "LLLLW" ]; then
		mii_mgr -s -p 22 -r 4 -v 0x1
		mii_mgr -s -p 22 -r 5 -v 0x1
		mii_mgr -s -p 22 -r 6 -v 0x1
		mii_mgr -s -p 22 -r 7 -v 0x1
		mii_mgr -s -p 22 -r 8 -v 0x2
		mii_mgr -s -p 23 -r 0 -v 0x302f
	elif [ "$1" = "WLLLL" ]; then
		mii_mgr -s -p 22 -r 4 -v 0x2
		mii_mgr -s -p 22 -r 5 -v 0x1
		mii_mgr -s -p 22 -r 6 -v 0x1
		mii_mgr -s -p 22 -r 7 -v 0x1
		mii_mgr -s -p 22 -r 8 -v 0x1
		mii_mgr -s -p 23 -r 0 -v 0x213e
	else
		echo "LAN WAN layout $0 is not suported"
	fi
}

configVtss()
{
	spicmd vtss vlan
}

restoreVtss()
{
	spicmd vtss novlan
}

if [ "$1" = "0" ]; then
	SWITCH_MODE=0
	#isc is used to distinguish between 175C and 175D
	isc=`mii_mgr -g -p 29 -r 31`
	if [ "$2" = "0" ]; then
		if [ "$isc" = "Get: phy[29].reg[31] = 175c" ]; then
			restore175C
		else
			restore175D
		fi
	elif [ "$2" = "LLLLW" ]; then
		if [ "$isc" = "Get: phy[29].reg[31] = 175c" ]; then
			config175C "LLLLW"
		else
			config175D "LLLLW"
		fi
	elif [ "$2" = "WLLLL" ]; then
		if [ "$isc" = "Get: phy[29].reg[31] = 175c" ]; then
			config175C "WLLLL"
		else
			config175D "WLLLL"
		fi
	else
		echo ">>> unknown vlan type $2 <<<"
		usage $0
	fi
elif [ "$1" = "1" ]; then
	SWITCH_MODE=1
	if [ "$2" = "0" ]; then
		restoreVtss
	elif [ "$2" = "1" ]; then
		configVtss
	else
		echo ">>> unknown vlan type $2 <<<"
		usage $0
	fi
elif [ "$1" = "2" ]; then
	SWITCH_MODE=2
	if [ "$2" = "0" ]; then
		restore3052
	elif [ "$2" = "EEEEE" ]; then
		enable3052 
	elif [ "$2" = "DDDDD" ]; then
		disable3052
	elif [ "$2" = "RRRRR" ]; then
		reset_all_phys
	elif [ "$2" = "FFFFF" ]; then
		reinit_all_phys
	elif [ "$2" = "LLLLW" ]; then
		wan_portN=4
		config3052 2 1 1 1 1 1
	elif [ "$2" = "WLLLL" ]; then
		wan_portN=0
		config3052 1 1 1 1 2 1
	elif [ "$2" = "LLLWW" ]; then
		wan_portN=4
		config3052 2 2 1 1 1 1
	elif [ "$2" = "WWLLL" ]; then
		wan_portN=0
		config3052 1 1 1 2 2 1
        elif [ "$2" = "W1234" ]; then
		wan_portN=0
                config3052 5 1 2 3 4
        elif [ "$2" = "12345" ]; then
		wan_portN=5
                config3052 1 2 3 4 5
	elif [ "$2" = "GW" ]; then
		wan_portN=5
		config3052 1 1 1 1 1 2
	elif [ "$2" = "GS" ]; then
		restore3052
		switch reg w e4 3f
	else
		a1=`echo $2| sed 's/^//'| sed 's/....$//'`
		a2=`echo $2| sed 's/^.//'| sed 's/...$//'`
		a3=`echo $2| sed 's/^..//'| sed 's/..$//'`
		a4=`echo $2| sed 's/^...//'| sed 's/.$//'`
		a5=`echo $2| sed 's/^....//'| sed 's/$//'`
		config3052 $a1 $a2 $a3 $a4 $a5 1
	fi
else
	echo "unknown swith type $1"
	echo ""
	usage $0
fi
