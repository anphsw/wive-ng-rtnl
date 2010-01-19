#!/bin/sh
#
# $Id: config-pptp.sh,v 1.5 2008-08-21 09:42:47 chhung Exp $
#
# usage: config-pptp.sh <mode> <mode_params> <server> <user> <password>
#

usage()
{
	echo "Usage:"
	echo "  $0 <mode> <mode_params> <server> <user> <password>"
	echo "Modes:"
	echo "  static - <mode_params> = <wan_if_name> <wan_ip> <wan_netmask> <gateway>"
	echo "  dhcp - <mode_params> = <wan_if_name>"
	echo "Example:"
	echo "  $0 static eth2.2 10.10.10.254 255.255.255.0 10.10.10.253 192.168.1.1 user pass"
	echo "  $0 dhcp eth2.2 192.168.1.1 user pass"
	exit 1
}

if [ "$5" = "" ]; then
	echo "$0: insufficient arguments"
	usage $0
fi

route_add()
{
    echo "Add static route to vpn server"
    ADDRESS=`nslookup $pptp_srv | grep Address | tail -n1 | cut -c 12- | awk {' print $1 '}`
    if [ "$ADDRESS" != "" ]; then
        PPTP_SERVER=$ADDRESS
     else
	PPTP_SERVER=$pptp_srv
    fi
    ROUTE=`ip r get $ADDRESS | grep dev | cut -f -3 -d " "`
    ip r add $ROUTE

    echo "Save default route to /var/tmp/dgw.ppp"
    old_dgw=`route -n -e | awk '{def=$1} def=="0.0.0.0" {print $2}'`
    if [ "$old_dgw" != "0.0.0.0" ] ; then
        echo $old_dgw > /var/tmp/dgw.ppp
    fi
    echo "Remove default route"
    ip route del default 2> /dev/null
}

#clear all configs
ppp=/etc/ppp
echo > $ppp/chap-secrets
echo > $ppp/pap-secrets

if [ "$1" = "static" ]; then
	if [ "$7" = "" ]; then
		echo "$0: insufficient arguments"
		usage $0
	fi
	ifconfig $2 $3 netmask $4
	route del default
	if [ "$5" != "0.0.0.0" ]; then
		route add default gw $5
	fi
	pptp_srv=$6
	pptp_u=$7
	pptp_pw=$8
	pptp_opmode=$9
	pptp_optime=${10}
	route_add
elif [ "$1" = "dhcp" ]; then
	killall -q udhcpc
	udhcpc -i $2 -s /sbin/udhcpc.sh -p /var/run/udhcpd.pid &
	pptp_srv=$3
	pptp_u=$4
	pptp_pw=$5
	pptp_opmode=$6
	pptp_optime=$7
	route_add
else
	echo "$0: unknown connection mode: $1"
	usage $0
fi

    killall -9 pppd
    killall -9 xl2tpd

pppd file /etc/ppp/options.pptp -detach mtu 1400 mru 1400 plugin /lib/pptp.so allow-mppe-128 \
    pptp_server $PPTP_SERVER call pptp defaultroute persist usepeerdns user $pptp_u password $pptp_p &
