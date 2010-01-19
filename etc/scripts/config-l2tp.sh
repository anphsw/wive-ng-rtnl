#!/bin/sh
#
# $Id: config-l2tp.sh,v 1.5 2008-08-21 09:42:47 chhung Exp $
#
# usage: config-l2tp.sh <mode> <mode_params> <server> <user> <password>
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

if [ "$5" = "" ]; then
	echo "$0: insufficient arguments"
	usage $0
fi

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
	l2tp_srv=$6
	l2tp_u=$7
	l2tp_pw=$8
	l2tp_opmode=$9
	l2tp_optime=${10}
	route_add
elif [ "$1" = "dhcp" ]; then
	killall -q udhcpc
	udhcpc -i $2 -s /sbin/udhcpc.sh -p /var/run/udhcpd.pid &
	l2tp_srv=$3
	l2tp_u=$4
	l2tp_pw=$5
	l2tp_opmode=$6
	l2tp_optime=$7
	route_add
else
	echo "$0: unknown connection mode: $1"
	usage $0
fi

#clear all configs
ppp=/etc/ppp
echo > $ppp/l2tpd.conf
echo > $ppp/options.l2tp
echo > $ppp/chap-secrets

    printf "[global]
    access control = yes
    [lac $l2tp_srv]\n
    redial = yes
    redial timeout = 20
    require chap = yes
    require authentication = no
    lns = $l2tp_srv
    name = $l2tp_u
    require pap = no
    autodial = yes
    pppoptfile = $ppp/options.l2tp
    " >> $ppp/l2tpd.conf

    printf "
    connect /bin/true
    idle 0
    maxfail 0
    refuse-pap
    refuse-eap
    noipx
    noproxyarp
    mtu 1400
    mru 1400
    defaultroute
    usepeerdns
    " >> $ppp/options.l2tp

    printf "$l2tp_u * $l2tp_p" >> $ppp/chap-secrets
    printf "$l2tp_u * $l2tp_p" >> $ppp/pap-secrets

    echo "Starting VPN network l2tp..."
    echo > /etc/ppp/connect-errors

    killall -9 pppd 
    killall -9 xl2tpd

    xl2tpd -c /etc/ppp/l2tpd.conf -s /etc/ppp/chap-secrets -p /var/lock/l2tpd.pid &
