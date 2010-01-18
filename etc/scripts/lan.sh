#!/bin/sh
#
# $Id: lan.sh,v 1.22.6.1 2008-10-02 12:57:42 winfred Exp $
#
# usage: wan.sh
#

. /sbin/global.sh

# stop all
service lld2d stop
service igmpproxy stop
service dnsserver stop
service pppoe-relay stop
service radvd stop

killall -q udhcpd
killall -q upnpd

echo "" > /var/udhcpd.leases

# ip address
ip=`nvram_get 2860 lan_ipaddr`
nm=`nvram_get 2860 lan_netmask`

if [ "$ip" = "" ]; then
    ip="192.168.1.1"
    nvram_set 2860 lan_ipaddr 192.168.1.1
fi

ifconfig $lan_if down
ifconfig $lan_if $ip netmask $nm
opmode=`nvram_get 2860 OperationMode`
if [ "$opmode" = "0" ]; then
	gw=`nvram_get 2860 wan_gateway`
	pd=`nvram_get 2860 wan_primary_dns`
	sd=`nvram_get 2860 wan_secondary_dns`
	route del default
	route add default gw $gw
	config-dns.sh $pd $sd
fi

ifconfig "br0:9" down
ifconfig "eth2:9" down
lan2enabled=`nvram_get 2860 Lan2Enabled`
if [ "$lan2enabled" = "1" ]; then
	ip_2=`nvram_get 2860 lan2_ipaddr`
	nm_2=`nvram_get 2860 lan2_netmask`
	if [ "$opmode" = "0" ]; then
		ifconfig "br0:9" "$ip_2" netmask "$nm_2"
		echo "ifconfig "br0:9" "$ip_2" netmask "$nm_2""
	elif [ "$opmode" = "1" ]; then
		ifconfig "br0:9" "$ip_2" netmask "$nm_2"
		echo "ifconfig "br0:9" "$ip_2" netmask "$nm_2""
	elif [ "$opmode" = "2" ]; then
		ifconfig "eth2:9" "$ip_2" netmask "$nm_2"
		echo "ifconfig "eth2:9" "$ip_2" netmask "$nm_2""
	elif [ "$opmode" = "3" ]; then
		ifconfig "br0:9" "$ip_2" netmask "$nm_2"
		echo "ifconfig "br0:9" "$ip_2" netmask "$nm_2""
	fi
fi


service hostname start
service lld2d start
service igmpproxy start
service dnsserver start
service pppoe-relay start
service radvd start
service stp start

# dhcp server
dhcp=`nvram_get 2860 dhcpEnabled`
if [ "$dhcp" = "1" ]; then
	start=`nvram_get 2860 dhcpStart`
	end=`nvram_get 2860 dhcpEnd`
	mask=`nvram_get 2860 dhcpMask`
	pd=`nvram_get 2860 dhcpPriDns`
	sd=`nvram_get 2860 dhcpSecDns`
	gw=`nvram_get 2860 dhcpGateway`
	lease=`nvram_get 2860 dhcpLease`
	static1=`nvram_get 2860 dhcpStatic1 | sed -e 's/;/ /'`
	static2=`nvram_get 2860 dhcpStatic2 | sed -e 's/;/ /'`
	static3=`nvram_get 2860 dhcpStatic3 | sed -e 's/;/ /'`
	config-udhcpd.sh -s $start
	config-udhcpd.sh -e $end
	config-udhcpd.sh -i $lan_if
	config-udhcpd.sh -m $mask
	if [ "$pd" != "" -o "$sd" != "" ]; then
		config-udhcpd.sh -d $pd $sd
	fi
	if [ "$gw" != "" ]; then
		config-udhcpd.sh -g $gw
	fi
	if [ "$lease" != "" ]; then
		config-udhcpd.sh -t $lease
	fi
	if [ "$static1" != "" ]; then
		config-udhcpd.sh -S $static1
	fi
	if [ "$static2" != "" ]; then
		config-udhcpd.sh -S $static2
	fi
	if [ "$static3" != "" ]; then
		config-udhcpd.sh -S $static3
	fi
	config-udhcpd.sh -r
fi

# upnp
if [ "$opmode" = "0" -o "$opmode" = "1" ]; then
	upnp=`nvram_get 2860 upnpEnabled`
	if [ "$upnp" = "1" ]; then
		route add -net 239.0.0.0 netmask 255.0.0.0 dev $lan_if
		upnp_xml.sh $ip
		upnpd -f $wan_ppp_if $lan_if &
	fi
fi

