#!/bin/sh

# This scipt restart needed services

. /sbin/global.sh

MODE=$1

# stop all
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ]; then 
    service dhcpd stop
    service pppoe-relay stop
fi
service ntp stop
service dnsserver stop
service upnp stop
service radvd stop
if [ "$MODE" != "pppd" ]; then 
    service udpxy stop
    service igmpproxy stop
    service lld2d stop
    service stp stop
fi
service iptables stop

#start all
service iptables start
service hostname start
service radvd start
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ]; then 
    service dhcpd start
    service pppoe-relay start
fi
if [ "$MODE" != "pppd" ]; then 
    service stp start
    service lld2d start
    service igmpproxy start
    service udpxy start
fi
service upnp start
service resolv start
service dnsserver start
service ntp start
