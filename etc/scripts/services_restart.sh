#!/bin/sh

# This scipt restart needed services

#include global config
. /etc/scripts/global.sh

LOG="logger -t services"
MODE=$1

$LOG "Restart needed services and scripts. Mode $MODE"

##########################################################
# This is services restart always                        #
##########################################################
    $LOG "Resolv config generate..."
    service resolv start
    $LOG "Reload iptables rules..."
    service iptables restart
    $LOG "Reload shaper rules..."
    service shaper restart
if [ -d /proc/sys/net/ipv6 ]; then
    service radvd restart
fi
    service ripd restart
    service zebra restart
    service inetd restart
    service dnsserver restart

##########################################################
# Need restart this servieces only:			 #
# 1) if call not from ip-up				 #
# 2) if call not from dhcp script			 #
##########################################################
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ]; then 
    service lld2d restart
    service igmp_proxy restart
    service udpxy restart
    service samba restart
    if [ "$MODE" != "misc" ]; then 
	service dhcpd restart
	service pppoe-relay restart
	service chillispot restart
    fi
    $LOG "Fastpath, passthrouth, stp and othes mode set..."
    service kext start
fi

##########################################################
# Need restart this servieces only:                    	 #
# 1) if not VPN enable                               	 #
# 2) if VPN enable and this scripts called from ip-up	 #
# 3) if restart mode = all				 #
##########################################################
if [ "$MODE" = "pppd" ] || [ "$MODE" = "all" ] || [ "$vpnEnabled" != "on" ]; then
    service upnp restart
    service ddns restart
    service ntp restart
fi
