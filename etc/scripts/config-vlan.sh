#!/bin/sh

############################################################
# config-vlan.sh - configure vlan switch particion helper  #
#                                                          #
# usage: config-vlan.sh <switch_type> <vlan_type>          #
#   switch_type: 0=IC+, 1=vtss                             #
#   vlan_type: 0=no_vlan, 1=vlan, LLLLW=wan_4, WLLLL=wan_0 #
############################################################
 
#include kernel config
. /etc/scripts/config.sh
. /etc/scripts/global.sh

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
	echo "  $0 2 HHHHH - config RT3052 Enable all ports 10FD"
	echo "  $0 2 DDDDD - config RT3052 Disable all ports"
	echo "  $0 2 RRRRR - config RT3052 Reset all ports"
	echo "  $0 2 FFFFF - config RT3052 Full reinit switch"
	echo "  $0 2 LLLLW - config RT3052 with VLAN and WAN at port 4"
	echo "  $0 2 WLLLL - config RT3052 with VLAN and WAN at port 0"
        echo "  $0 2 W1234 - config RT3052 with VLAN 5 at port 0 and VLAN 1~4 at port 1~4"
        echo "  $0 2 12345 - config RT3052 with VLAN 1~5 at port 0~4"
	echo "  $0 2 GW - config RT3052 with WAN at Giga port"
	echo "  $0 2 GS - config RT3052 with Giga port connecting to an external switch"
	exit 0
}

config3052()
{
	#preinit
	switch reg w 14 405555
	switch reg w 50 2001
	switch reg w 98 7f3f
	switch reg w e4 3f

	if [ "$1" = "WLLLL" ]; then
		switch reg w 40 1001
		switch reg w 44 1001
		switch reg w 48 1002
		switch reg w 70 ffff506f
	elif [ "$1" = "LLLLW" ]; then
		switch reg w 40 1002
		switch reg w 44 1001
		switch reg w 48 1001
		switch reg w 70 ffff417e
        elif [ "$1" = "W1234" ]; then
                switch reg w 40 1005
                switch reg w 44 3002
                switch reg w 48 1004
                switch reg w 70 50484442
                switch reg w 74 ffffff41
        elif [ "$1" = "12345" ]; then
                switch reg w 40 2001
                switch reg w 44 4003
                switch reg w 48 1005
                switch reg w 70 7e7e7e41
                switch reg w 74 ffffff7e
	elif [ "$1" = "GW" ]; then
		switch reg w 40 1001
		switch reg w 44 1001
		switch reg w 48 2001
		switch reg w 70 ffff605f
	fi
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
	switch reg w e4 0
}

disable3052()
{
    for i in `seq 0 4`; do
	mii_mgr -s -p $i -r 0 -v 0x0800
    done
}

enable3052H()
{
    for i in `seq 0 4`; do
	mii_mgr -s -p $i -r 0 -v 0x8000
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
	if [ "$CONFIG_RAETH_ROUTER" != "y" ] && [ "$CONFIG_RT_3052_ESW" != "y" ]; then
		return
	fi

	echo "Reset all phy port"
	if [ "$opmode" = "1" ]; then
	    #Ports down skip WAN port
	    if [ "$wan_port" = "0" ]; then
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
		if [ "$CONFIG_RALINK_VISTA_BASIC" == "y" ]; then
			mii_mgr -s -p 30 -r 1 -v 0x2f3f
		else
			mii_mgr -s -p 30 -r 1 -v 0x2f00
		fi
		mii_mgr -s -p 30 -r 2 -v 0x0030
	elif [ "$1" = "WLLLL" ]; then
		mii_mgr -s -p 29 -r 24 -v 0x2
		mii_mgr -s -p 29 -r 25 -v 0x1
		mii_mgr -s -p 29 -r 26 -v 0x1
		mii_mgr -s -p 29 -r 27 -v 0x1
		mii_mgr -s -p 29 -r 28 -v 0x1
		mii_mgr -s -p 30 -r 9 -v 0x0189
		if [ "$CONFIG_RALINK_VISTA_BASIC" == "y" ]; then
			mii_mgr -s -p 30 -r 1 -v 0x3e3f
		else
			mii_mgr -s -p 30 -r 1 -v 0x3e00
		fi
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
	if [ "$2" = "0" ]; then
		restoreVtss
	elif [ "$2" = "1" ]; then
		configVtss
	else
		echo ">>> unknown vlan type $2 <<<"
		usage $0
	fi
elif [ "$1" = "2" ]; then
	if [ "$2" = "0" ]; then
		restore3052
	elif [ "$2" = "EEEEE" ]; then
		enable3052 
	elif [ "$2" = "HHHHH" ]; then
		enable3052H 
	elif [ "$2" = "DDDDD" ]; then
		disable3052
	elif [ "$2" = "RRRRR" ]; then
		reset_all_phys
	elif [ "$2" = "FFFFF" ]; then
		reinit_all_phys
	elif [ "$2" = "LLLLW" ]; then
		config3052 LLLLW
	elif [ "$2" = "WLLLL" ]; then
		config3052 WLLLL
        elif [ "$2" = "W1234" ]; then
                config3052 W1234
        elif [ "$2" = "12345" ]; then
                config3052 12345
	elif [ "$2" = "GW" ]; then
		config3052 GW
	elif [ "$2" = "GS" ]; then
		restore3052
		switch reg w e4 3f
	else
		echo "unknown vlan type $2"
		echo ""
		usage $0
	fi
else
	echo "unknown swith type $1"
	echo ""
	usage $0
fi

