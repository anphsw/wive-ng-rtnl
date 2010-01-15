#!/bin/sh
#
# $Id: nat.sh,v 1.1 2007-09-26 01:33:21 winfred Exp $
#
# usage: nat.sh
#

. /sbin/global.sh

lan_ip=`nvram_get 2860 lan_ipaddr`
nat_en=`nvram_get 2860 natEnabled`


echo 1 > /proc/sys/net/ipv4/ip_forward

if [ "$nat_en" = "1" ]; then
	echo 180 > /proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout
	echo 180 > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established
	if [ "$wanmode" = "PPPOE" -o "$wanmode" = "L2TP" -o "$wanmode" = "PPTP" ]; then
		wan_if="ppp0"
	fi
	iptables -t nat -A POSTROUTING -s $lan_ip/24 -o $wan_if -j MASQUERADE
fi

