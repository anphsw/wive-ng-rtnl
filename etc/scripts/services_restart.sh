#!/bin/sh

# This scipt restart needed services

#include global
. /etc/scripts/global.sh

LOG="logger -t services"
MODE=$1

$LOG "Restart needed services and scripts. Mode $MODE"
# stop needed services
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ] && [ "$MODE" != "misc" ]; then 
    service dhcpd stop
    service pppoe-relay stop
fi
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ]; then 
    service stp stop
fi
if [ "$MODE" != "pppd" ]; then 
    service udpxy stop
    service igmp_proxy stop
    service lld2d stop
    service samba stop
fi

##########################################################
# This is services restart always                       #
##########################################################
service resolv start
$LOG "Reload iptables rules..."
service iptables restart
$LOG "Reload shaper rules..."
service shaper restart

#start needed services
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ] && [ "$MODE" != "misc" ]; then 
    service dhcpd start
    service pppoe-relay start
fi
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ]; then 
    service stp start
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
service radvd restart
service ripd restart
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

#call vpn helper...
if [ "$MODE" = "all" ]; then
    $LOG "Call vpnhelper restart..."
    service vpnhelper restart &
fi

$LOG "All OK!. Mode $MODE"
