#!/bin/sh
#
# $Id: wan.sh,v 1.18 2009-02-09 13:29:37 michael Exp $
#
# usage: wan.sh
#

. /sbin/global.sh

kill_ppp(){
	echo "Killall ppp connections"
	killall -q -9 config-pptp.sh
	killall -q -9 config-l2tp.sh
	killall -q -9 config-pppoe.sh
	killall -q -9 pppd
	killall -q -9 xl2tpd
}

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

UDHCPCOPTS="-r -S -R -T 10 -A 30 -b -s /sbin/udhcpc.sh -p /var/run/udhcpc.pid -O staticroutes &"

clone_en=`nvram_get 2860 macCloneEnabled`
clone_mac=`nvram_get 2860 macCloneMac`
#MAC Clone: bridge mode doesn't support MAC Clone
if [ "$opmode" != "0" -a "$clone_en" = "1" ]; then
	ifconfig $wan_if down > /dev/null 2>&1
        if [ "$opmode" = "2" ]; then
                rmmod rt2860v2_sta
                insmod rt2860v2_sta mac=$clone_mac
        else
                ifconfig $wan_if hw ether $clone_mac
        fi
	ifconfig $wan_if up
fi

if [ "$wanmode" = "STATIC" -o "$opmode" = "0" ]; then
	#always treat bridge mode having static wan connection
	ip=`nvram_get 2860 wan_ipaddr`
	nm=`nvram_get 2860 wan_netmask`
	gw=`nvram_get 2860 wan_gateway`
	pd=`nvram_get 2860 wan_primary_dns`
	sd=`nvram_get 2860 wan_secondary_dns`
	
	#killall ppp connections
	kill_ppp
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
	#killall ppp connections
	kill_ppp
	udhcpc -i $wan_if -H $HOSTNAME $UDHCPCOPTS > /dev/null 2>&1 &
elif [ "$wanmode" = "PPPOE" ]; then
	u=`nvram_get 2860 wan_pppoe_user`
	pw=`nvram_get 2860 wan_pppoe_pass`
	pppoe_opmode=`nvram_get 2860 wan_pppoe_opmode`
	pppoe_optime=`nvram_get 2860 wan_pppoe_optime`

	#killall ppp connections
	kill_ppp

	config-pppoe.sh $u $pw $wan_if $pppoe_opmode $pppoe_optime &

elif [ "$wanmode" = "L2TP" ]; then
	mode=`nvram_get 2860 wan_l2tp_mode`
	l2tp_opmode=`nvram_get 2860 wan_l2tp_opmode`
	l2tp_optime=`nvram_get 2860 wan_l2tp_optime`

	#killall ppp connections
	kill_ppp

        if [ "$mode" = "0" ]; then
	#static
    	ip=`nvram_get 2860 wan_l2tp_ip`
	nm=`nvram_get 2860 wan_l2tp_netmask`
	gw=`nvram_get 2860 wan_l2tp_gateway`
    	ifconfig $wan_if $ip netmask $nm up
	service resolv start
    	    if [ "$gw" != "" ] && [ "$gw" != "0.0.0.0" ]; then
		route add -host $ip dev $wan_if gw $gw
    		route del default
    		route add default gw $gw
    	    else
		route add -host $ip dev $wan_if
	    fi
	else
	#dhcp
    	    udhcpc -i $wan_if -H $HOSTNAME $UDHCPCOPTS > /dev/null 2>&1 &
	    sleep 5
	fi
	config-l2tp.sh &

elif [ "$wanmode" = "PPTP" ]; then
        srv=`nvram_get 2860 wan_pptp_server`
        mode=`nvram_get 2860 wan_pptp_mode`
        pptp_opmode=`nvram_get 2860 wan_pptp_opmode`
        pptp_optime=`nvram_get 2860 wan_pptp_optime`

	#killall ppp connections
	kill_ppp

        if [ "$mode" = "0" ]; then
	#static
            ip=`nvram_get 2860 wan_pptp_ip`
            nm=`nvram_get 2860 wan_pptp_netmask`
            gw=`nvram_get 2860 wan_pptp_gateway`
    	    ifconfig $wan_if $ip netmask $nm up
	    service resolv start
            if [ "$gw" != "" ] && [ "$gw" != "0.0.0.0" ]; then
		route add -host $ip dev $wan_if gw $gw
    		route del default
        	route add default gw $gw
    	    else
		route add -host $ip dev $wan_if
	    fi
	else
	#dhcp
    	    udhcpc -i $wan_if -H $HOSTNAME $UDHCPCOPTS  > /dev/null 2>&1 &
	    sleep 5
	fi
	config-pptp.sh &
else
	echo "wan.sh: unknown wan connection type: $wanmode"
	exit 1
fi

