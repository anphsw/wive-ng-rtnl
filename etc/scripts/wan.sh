#!/bin/sh
#
# $Id: wan.sh,v 1.18 2009-02-09 13:29:37 michael Exp $
#
# usage: wan.sh
#

. /sbin/global.sh

# stop all
killall -q udhcpc

#workaround fix me!!!
if [ "$wan_if" = "" ]; then
    wan_if="eth2.2"
fi

HOSTNAME_SYS=`hostname`
HOSTNAME_NVRAM=`nvram_get 2860 HostName`

if [ "$HOSTNAME_NVRAM" != "" ]; then
   HOSTNAME="$HOSTNAME_NVRAM"
else
   HOSTNAME="$HOSTNAME_SYS"
fi

UDHCPCOPTS="-r -S -R -T 10 -A 30 -s /sbin/udhcpc.sh -p /var/run/udhcpc.pid -O staticroutes -f &"

opmode=`nvram_get 2860 OperationMode`
clone_en=`nvram_get 2860 macCloneEnabled`
clone_mac=`nvram_get 2860 macCloneMac`
#MAC Clone: bridge mode doesn't support MAC Clone
if [ "$opmode" != "0" -a "$clone_en" = "1" ]; then
	ip link set $wan_if down > /dev/null 2>&1
        if [ "$opmode" = "2" ]; then
		#reload wifi modules
		service modules restart
        else
                ifconfig $wan_if hw ether $clone_mac
        fi
	ip link set $wan_if up
fi

if [ "$wanmode" = "STATIC" -o "$opmode" = "0" ]; then
	#always treat bridge mode having static wan connection
	ip=`nvram_get 2860 wan_ipaddr`
	nm=`nvram_get 2860 wan_netmask`
	gw=`nvram_get 2860 wan_gateway`
	pd=`nvram_get 2860 wan_primary_dns`
	sd=`nvram_get 2860 wan_secondary_dns`
	
	#lan and wan ip should not be the same except in bridge mode
	if [ "$opmode" != "0" ]; then
		lan_ip=`nvram_get 2860 lan_ipaddr`
		if [ "$ip" = "$lan_ip" ]; then
			echo "wan.sh: warning: WAN's IP address is set identical to LAN"
			exit 0
		fi
	else
		#use lan's ip address instead
		ip=`nvram_get 2860 lan_ipaddr`
		nm=`nvram_get 2860 lan_netmask`
	fi
	ifconfig $wan_if $ip netmask $nm
	route del default
	if [ "$gw" != "" ] && [ "$gw" != "0.0.0.0" ]; then
	    route add default gw $gw
	fi

elif [ "$wanmode" = "DHCP" ]; then
	udhcpc -i $wan_if -H $HOSTNAME $UDHCPCOPTS > /dev/null 2>&1 &
else
	echo "wan.sh: unknown wan connection type: $wanmode"
	exit 1
fi

PPP_STARTED=`pidof pppd`
XL2TPD_STARTED=`pidof xl2tpd`

if [ "$PPP_STARTED" = "" ] && [ "$XL2TPD_STARTED" = "" ]; then
    (sleep 20 && service vpnhelper restart) &
fi
