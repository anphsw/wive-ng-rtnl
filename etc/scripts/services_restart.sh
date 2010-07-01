#!/bin/sh

# This scipt restart needed services

. /sbin/global.sh

MODE=$1

# stop all
service ntp stop
service dnsserver stop
service upnp stop
service radvd stop
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ]; then 
    service dhcpd stop
    service pppoe-relay stop
fi
if [ "$MODE" != "pppd" ]; then 
    service udpxy stop
    service igmp_proxy stop
    service lld2d stop
    service stp stop
fi

#restart iptables
if [ "$MODE" != "pppd" ]; then
    service iptables restart
fi

#start all
service hostname start
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ]; then 
    service dhcpd start
    service pppoe-relay start
fi
if [ "$MODE" != "pppd" ]; then 
    service stp start
    service lld2d start
    service igmp_proxy start
    service udpxy start
fi
service upnp start
service dnsserver start
service radvd start
service ntp start
