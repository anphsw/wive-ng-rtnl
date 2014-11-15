#!/bin/sh

############################################################
# config-vlan.sh - configure vlan switch particion helper  #
#                                                          #
# usage: config-vlan.sh <switch_type> <vlan_type>          #
#   switch_type: 0=IC+, 1=vtss, 2=esw3050x                 #
#   vlan_type: 0=no_vlan, 1=vlan, LLLLW=wan_4, WLLLL=wan_0 #
############################################################

usage() {
	echo "Usage:"
	echo "  $0 2 0 - restore RT3052 to no VLAN partition"
	echo "  $0 2 EEEEE - config RT3052 Enable all ports 100FD"
	echo "  $0 2 DDDDD - config RT3052 Disable all ports"
	echo "  $0 2 RRRRR - config RT3052 Reset all ports"
	echo "  $0 2 WWWWW - config RT3052 Reinit WAN port at switch"
	echo "  $0 2 FFFFF - config RT3052 Full reinit switch"
	echo "  $0 2 LLLLW - config RT3052 with LAN at ports 0-3 and WAN at port 4"
	echo "  $0 2 WLLLL - config RT3052 with LAN at ports 1-4 and WAN at port 0"
	echo "  $0 2 LLLWW - config RT3052 with LAN at ports 0-2 and WAN at port 3-4"
	echo "  $0 2 WWLLL - config RT3052 with LAN at ports 2-4 and WAN at port 0-1"
        echo "  $0 2 12345 - config RT3052 with individual VLAN 1~5 at port 0~4"
        echo "  $0 2 xxxxx - config RT3052 with user defined VLAN(1-6): 112345,443124 on ports 0-4 and Gigabit"
	echo "  $0 2 GW - config RT3052 with WAN at Giga port"
	echo "  $0 2 GS - config RT3052 with Giga port connecting to an external switch"
	echo "  $0 3 0 - restore Ralink RT6855/MT7620/MT7621 ESW to no VLAN partition"
	echo "  $0 3 LLLLW - config Ralink RT6855/MT7620/MT7621 ESW with VLAN and WAN at port 4"
	echo "  $0 3 WLLLL - config Ralink RT6855/MT7620/MT7621 ESW with VLAN and WAN at port 0"
	echo "  $0 3 12345 - config Ralink RT6855/MT7620/MT7621 ESW with VLAN 1~5 at port 0~4"
	echo "  $0 3 GW - config Ralink RT6855/MT7620/MT7621 ESW with WAN at Giga port"
	exit 0
}

configEsw() {
	# preinit
	switch reg w 14 405555
	switch reg w 50 2001
	switch reg w 90 7f7f
	switch reg w 98 7f3f #disable VLAN

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

        #clear mac table if vlan configuration changed
        switch clear
}

restoreEsw() {
        switch reg w 14 5555
        switch reg w 40 1001
        switch reg w 44 1001
        switch reg w 48 1001
        switch reg w 4c 1
        switch reg w 50 2001
        switch reg w 70 ffffffff
        switch reg w 98 7f7f
        switch reg w e4 7f

        #clear mac table if vlan configuration changed
        switch clear
}

disableEsw() {
    for i in `seq 0 4`; do
	mii_mgr -s -p $i -r 0 -v 0x0800
    done
}

enableEsw() {
    for i in `seq 0 4`; do
	mii_mgr -s -p $i -r 0 -v 0x9000
    done
}

# arg1:  phy address.
link_down() {
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
	mii_mgr -s -p "$1" -r 0 -v $new
}

link_up() {
	# get original register value
	get_mii=`mii_mgr -g -p "$1" -r 0`
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
	mii_mgr -s -p "$1" -r 0 -v $new
}

reset_all_phys() {
	if [ "$SWITCH_MODE" != "0" ] && [ "$SWITCH_MODE" != "2" ]; then
		return
	fi

	echo "Reset all phy port"
	eval `nvram_buf_get 2860 OperationMode wan_port`
	if [ "$OperationMode" = "1" ]; then
	    # Ports down skip WAN port
	    if [ "$wan_portN" = "0" ]; then
		start=0
		end=3
	    else
		start=1
		end=4
	    fi
	else
	    # All ports down
	    start=0
	    end=4
	fi

	# disable ports
	for i in `seq $start $end`; do
    	    link_down $i
	done

	# force Windows clients to renew IP and update DNS server
	sleep 1

	# enable ports
	for i in `seq $start $end`; do
    	    link_up $i
	done
}

reset_wan_phys() {
	if [ "$SWITCH_MODE" != "0" ] && [ "$SWITCH_MODE" != "2" ]; then
		return
	fi

	echo "Reset wan phy port"
	eval `nvram_buf_get 2860 OperationMode wan_port`
	if [ "$OperationMode" = "1" ]; then
	    if [ "$wan_portN" = "0" ]; then
		link_down 4
		link_up 4
	    else
		link_down 0
		link_up 0
	    fi
	fi
}

reinit_all_phys() {
	disableEsw
	enableEsw
	reset_all_phys
}

config6855Esw()
{
	#LAN/WAN ports as security mode
	switch reg w 2004 ff0003 #port0
	switch reg w 2104 ff0003 #port1
	switch reg w 2204 ff0003 #port2
	switch reg w 2304 ff0003 #port3
	switch reg w 2404 ff0003 #port4
	switch reg w 2504 ff0003 #port5
	#LAN/WAN ports as transparent port
	switch reg w 2010 810000c0 #port0
	switch reg w 2110 810000c0 #port1
	switch reg w 2210 810000c0 #port2
	switch reg w 2310 810000c0 #port3
	switch reg w 2410 810000c0 #port4
	switch reg w 2510 810000c0 #port5
	#set CPU/P7 port as user port
	switch reg w 2610 81000000 #port6
	switch reg w 2710 81000000 #port7

	switch reg w 2604 20ff0003 #port6, Egress VLAN Tag Attribution=tagged
	switch reg w 2704 20ff0003 #port7, Egress VLAN Tag Attribution=tagged
	switch reg w 2610 81000000 #port6, special tag disable

	if [ "$1" = "LLLLW" ]; then
		#set PVID
		switch reg w 2014 10001 #port0
		switch reg w 2114 10001 #port1
		switch reg w 2214 10001 #port2
		switch reg w 2314 10001 #port3
		switch reg w 2414 10002 #port4
		switch reg w 2514 10001 #port5
		#VLAN member port
		switch vlan set 0 1 11110111
		switch vlan set 1 2 00001011
	elif [ "$1" = "LLLLWW" ]; then
		#set PVID
		switch reg w 2014 10001 #port0
		switch reg w 2114 10001 #port1
		switch reg w 2214 10001 #port2
		switch reg w 2314 10002 #port3
		switch reg w 2414 10002 #port4
		switch reg w 2514 10001 #port5
		#VLAN member port
		switch vlan set 0 1 11110111
		switch vlan set 1 2 00001011
	elif [ "$1" = "WLLLL" ]; then
		#set PVID
		switch reg w 2014 10002 #port0
		switch reg w 2114 10001 #port1
		switch reg w 2214 10001 #port2
		switch reg w 2314 10001 #port3
		switch reg w 2414 10001 #port4
		switch reg w 2514 10001 #port5
		#VLAN member port
		switch vlan set 0 1 01111111
		switch vlan set 1 2 10000011
	elif [ "$1" = "WWLLL" ]; then
		#set PVID
		switch reg w 2014 10002 #port0
		switch reg w 2114 10002 #port1
		switch reg w 2214 10001 #port2
		switch reg w 2314 10001 #port3
		switch reg w 2414 10001 #port4
		switch reg w 2514 10001 #port5
		#VLAN member port
		switch vlan set 0 1 01111111
		switch vlan set 1 2 10000011
	elif [ "$1" = "W1234" ]; then
		echo "W1234"
		#set PVID
		switch reg w 2014 10005 #port0
		switch reg w 2114 10001 #port1
		switch reg w 2214 10002 #port2
		switch reg w 2314 10003 #port3
		switch reg w 2414 10004 #port4
		switch reg w 2514 10006 #port5
		#VLAN member port
		switch vlan set 4 5 10000011
		switch vlan set 0 1 01000011
		switch vlan set 1 2 00100011
		switch vlan set 2 3 00010011
		switch vlan set 3 4 00001011
		switch vlan set 5 6 00000111
	elif [ "$1" = "12345" ]; then
		echo "12345"
		#set PVID
		switch reg w 2014 10001 #port0
		switch reg w 2114 10002 #port1
		switch reg w 2214 10003 #port2
		switch reg w 2314 10004 #port3
		switch reg w 2414 10005 #port4
		switch reg w 2514 10006 #port5
		#VLAN member port
		switch vlan set 0 1 10000011
		switch vlan set 1 2 01000011
		switch vlan set 2 3 00100011
		switch vlan set 3 4 00010011
		switch vlan set 4 5 00001011
		switch vlan set 5 6 00000111
	elif [ "$1" = "GW" ]; then
		echo "GW"
		#set PVID
		switch reg w 2014 10001 #port0
		switch reg w 2114 10001 #port1
		switch reg w 2214 10001 #port2
		switch reg w 2314 10001 #port3
		switch reg w 2414 10001 #port4
		switch reg w 2514 10002 #port5
		#VLAN member port
		switch vlan set 0 1 11111011
		switch vlan set 1 2 00000111
	fi

	#clear mac table if vlan configuration changed
	switch clear
}

restore6855Esw()
{
	echo "restore GSW to dump switch mode"
	#port matrix mode
	switch reg w 2004 ff0000 #port0
	switch reg w 2104 ff0000 #port1
	switch reg w 2204 ff0000 #port2
	switch reg w 2304 ff0000 #port3
	switch reg w 2404 ff0000 #port4
	switch reg w 2504 ff0000 #port5
	switch reg w 2604 ff0000 #port6
	switch reg w 2704 ff0000 #port7

	#LAN/WAN ports as transparent mode
	switch reg w 2010 810000c0 #port0
	switch reg w 2110 810000c0 #port1
	switch reg w 2210 810000c0 #port2
	switch reg w 2310 810000c0 #port3
	switch reg w 2410 810000c0 #port4
	switch reg w 2510 810000c0 #port5
	switch reg w 2610 810000c0 #port6
	switch reg w 2710 810000c0 #port7

	#clear mac table if vlan configuration changed
	switch clear
}

if [ "$1" = "2" ]; then
	SWITCH_MODE=2
	if [ "$2" = "0" ]; then
		restoreEsw
	elif [ "$2" = "EEEEE" ]; then
		enableEsw 
	elif [ "$2" = "DDDDD" ]; then
		disableEsw
	elif [ "$2" = "RRRRR" ]; then
		reset_all_phys
	elif [ "$2" = "WWWWW" ]; then
		reset_wan_phys
	elif [ "$2" = "FFFFF" ]; then
		reinit_all_phys
	elif [ "$2" = "LLLLW" ]; then
		wan_portN=4
		configEsw 2 1 1 1 1 1
	elif [ "$2" = "WLLLL" ]; then
		wan_portN=0
		configEsw 1 1 1 1 2 1
	elif [ "$2" = "LLLWW" ]; then
		wan_portN=4
		configEsw 2 2 1 1 1 1
	elif [ "$2" = "WWLLL" ]; then
		wan_portN=0
		configEsw 1 1 1 2 2 1
        elif [ "$2" = "W1234" ]; then
		wan_portN=0
                configEsw 5 1 2 3 4
        elif [ "$2" = "12345" ]; then
		wan_portN=5
                configEsw 1 2 3 4 5
	elif [ "$2" = "GW" ]; then
		wan_portN=5
		configEsw 1 1 1 1 1 2
	elif [ "$2" = "GS" ]; then
		restoreEsw
		switch reg w e4 3f
	else
		a1=`echo $2| sed 's/^//'| sed 's/....$//'`
		a2=`echo $2| sed 's/^.//'| sed 's/...$//'`
		a3=`echo $2| sed 's/^..//'| sed 's/..$//'`
		a4=`echo $2| sed 's/^...//'| sed 's/.$//'`
		a5=`echo $2| sed 's/^....//'| sed 's/$//'`
		configEsw $a1 $a2 $a3 $a4 $a5 1
	fi
elif [ "$1" = "3" ]; then
	SWITCH_MODE=3
	if [ "$2" = "0" ]; then
		restore6855Esw
	elif [ "$2" = "LLLLW" ]; then
		config6855Esw LLLLW
	elif [ "$2" = "WLLLL" ]; then
		config6855Esw WLLLL
	elif [ "$2" = "12345" ]; then
		config6855Esw 12345
	elif [ "$2" = "GW" ]; then
		config6855Esw GW
	else
		echo "unknown vlan type $2"
		echo ""
		usage $0
	fi
else
	echo "unknown swith type $1"
	echo ""
	usage "$0"
fi
