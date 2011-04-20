#!/bin/sh

# This scipt restart needed services

#include global and kernel
. /etc/scripts/config.sh
. /etc/scripts/global.sh

LOG="logger -t services"
MODE=$1

$LOG "Restart needed services and scripts. Mode $MODE"

#Stop needed services
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ] && [ "$MODE" != "misc" ]; then 
    service dhcpd stop
    service pppoe-relay stop
    service chillispot stop
fi
if [ "$MODE" != "pppd" ]; then 
    service udpxy stop
    service igmp_proxy stop
    service lld2d stop
    service samba stop
fi

#Configure kernel Extensions
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ]; then 
    $LOG "Fastpath, passthrouth, stp and othes mode set..."
    kernel_ext_en
fi

##########################################################
# This is services restart always                       #
##########################################################
$LOG "Resolv config generate..."
service resolv start
$LOG "Reload iptables rules..."
service iptables restart
$LOG "Reload shaper rules..."
service shaper restart

#Start needed services
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ] && [ "$MODE" != "misc" ]; then 
    service dhcpd start
    service pppoe-relay start
    service chillispot start
fi
if [ "$MODE" != "pppd" ]; then 
    service lld2d start
    service igmp_proxy start
    service udpxy start
    service samba start
fi

##########################################################
# This is services restart always                       #
##########################################################
if [ "$CONFIG_IPV6" != "" ]; then
    service radvd restart
fi
    service ripd restart
    service zebra restart
    service dnsserver restart

##########################################################
# Need restart this servieces only:                    	#
# 1) if not VPN enable                               	#
# 2) if VPN enable and this scripts called from ip-up	#
##########################################################
if [ "$MODE" = "pppd" ] || [ "$vpnEnabled" != "on" ]; then
    service upnp restart
    service ntp restart
    service ddns restart
fi
