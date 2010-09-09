#!/bin/sh

# This scipt restart needed services

#include global
. /etc/scripts/global.sh

MODE=$1

# stop needed
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ] && [ "$MODE" != "misc" ]; then 
    service dhcpd stop
    service pppoe-relay stop
fi
if [ "$MODE" != "pppd" ]; then 
    service resolv start
    service udpxy stop
    service igmp_proxy stop
    service lld2d stop
    service stp stop
    service samba stop
fi

#restart iptables
service iptables restart

#start all
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ] && [ "$MODE" != "misc" ]; then 
    service dhcpd start
    service pppoe-relay start
fi
if [ "$MODE" != "pppd" ]; then 
    service stp start
    service lld2d start
    service igmp_proxy start
    service udpxy start
    service samba start
fi

#restart always
service upnp restart
service dnsserver restart
service radvd restart
service ntp restart
service ripd restart
