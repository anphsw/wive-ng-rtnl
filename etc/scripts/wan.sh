#!/bin/sh
#
# $Id: wan.sh,v 1.11 2008-09-10 06:39:13 steven Exp $
#
# usage: wan.sh
#

. /sbin/global.sh

# stop all
killall -q udhcpc
HOSTNAME=`hostname`

#workaround fix me!!!
if [ "$wan_if" = "" ]; then
    wan_if="eth2.2"
fi

clone_en=`nvram_get 2860 macCloneEnabled`
clone_mac=`nvram_get 2860 macCloneMac`
#MAC Clone: bridge mode doesn't support MAC Clone
if [ "$opmode" != "0" -a "$clone_en" = "1" ]; then
	ifconfig $wan_if down
	ifconfig $wan_if hw ether $clone_mac
	ifconfig $wan_if up
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
	if [ "$gw" != "" ]; then
	route add default gw $gw
	fi
	config-dns.sh $pd $sd
elif [ "$wanmode" = "DHCP" ]; then
	udhcpc -i $wan_if -H "$HOSTNAME" -S -R -T 10 -A 30 -b -s /sbin/udhcpc.sh -p /var/run/udhcpc.pid > /dev/null 2>&1 &
elif [ "$wanmode" = "PPPOE" ]; then
	u=`nvram_get 2860 wan_pppoe_user`
	pw=`nvram_get 2860 wan_pppoe_pass`
	pppoe_opmode=`nvram_get 2860 wan_pppoe_opmode`
	pppoe_optime=`nvram_get 2860 wan_pppoe_optime`

	killall -9 config-pptp.sh > /dev/null &

	config-pppoe.sh $u $pw $wan_if $pppoe_opmode $pppoe_optime &

elif [ "$wanmode" = "L2TP" ]; then
	mode=`nvram_get 2860 wan_l2tp_mode`
	l2tp_opmode=`nvram_get 2860 wan_l2tp_opmode`
	l2tp_optime=`nvram_get 2860 wan_l2tp_optime`

	killall -9 config-pptp.sh > /dev/null &

        if [ "$mode" = "0" ]; then
	#static
    	    ip=`nvram_get 2860 wan_l2tp_ip`
	    nm=`nvram_get 2860 wan_l2tp_netmask`
	    gw=`nvram_get 2860 wan_l2tp_gateway`
    	    ifconfig $wan_if $ip netmask $nm up
            if [ "$gw" != "" ] && [ "$gw" != "0.0.0.0" ]; then
		route add -host $ip dev $wan_ip gw $gw
    		route del default
        	route add default gw $gw
    	    else
		route add -host $ip dev $wan_ip
	    fi
	else
	#dhcp
            killall -q udhcpc
    	    udhcpc -i $wan_if -H "$HOSTNAME" -S -R -T 10 -A 30 -b -s /sbin/udhcpc.sh -p /var/run/udhcpc.pid > /dev/null 2>&1 &
	    sleep 5
	fi
	config-l2tp.sh &

elif [ "$wanmode" = "PPTP" ]; then
        srv=`nvram_get 2860 wan_pptp_server`
        mode=`nvram_get 2860 wan_pptp_mode`
        pptp_opmode=`nvram_get 2860 wan_pptp_opmode`
        pptp_optime=`nvram_get 2860 wan_pptp_optime`

	killall -9 config-pptp.sh > /dev/null &

        if [ "$mode" = "0" ]; then
	#static
            ip=`nvram_get 2860 wan_pptp_ip`
            nm=`nvram_get 2860 wan_pptp_netmask`
            gw=`nvram_get 2860 wan_pptp_gateway`
    	    ifconfig $wan_if $ip netmask $nm up
            if [ "$gw" != "" ] && [ "$gw" != "0.0.0.0" ]; then
		route add -host $ip dev $wan_ip gw $gw
    		route del default
        	route add default gw $gw
    	    else
		route add -host $ip dev $wan_ip
	    fi
	else
	#dhcp
            killall -q udhcpc
    	    udhcpc -i $wan_if -H "$HOSTNAME" -S -R -T 10 -A 30 -b -s /sbin/udhcpc.sh -p /var/run/udhcpc.pid  > /dev/null 2>&1 &
	    sleep 5
	fi
	config-pptp.sh &
else
	echo "wan.sh: unknown wan connection type: $wanmode"
	exit 1
fi
