#!/bin/sh

# This scipt restart needed services

#include global config
. /etc/scripts/global.sh

LOG="logger -t services"
MODE=$1

$LOG "Restart needed services and scripts. Mode $MODE"

##########################################################
# Regenerate resolv only if !ppp or wan_static_dns on    #
##########################################################
if [ "$vpnEnabled" != "on" ] || [ "$MODE" != "pppd" ] || [ "$wan_static_dns" = "on" ]; then
    $LOG "Resolv config generate..."
    service resolv start
fi

##########################################################
# This is services restart always                        #
##########################################################
    $LOG "Reload iptables rules..."
    service iptables restart
    $LOG "Reload shaper rules..."
    service shaper restart
if [ -f /bin/radvd ] && [ -d /proc/sys/net/ipv6 ]; then
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
    service kext start
    service parprouted restart
    service lld2d restart
    #if dnsmasq enabled or static dns
    if [ "$dnsPEnabled" = "1" ] || [ "$wan_static_dns" = "on" ]; then
	service dhcpd restart
    fi
    service samba restart
    if [ "$MODE" != "misc" ]; then 
	service pppoe-relay restart
	service chillispot restart
    fi
    service udpxy restart
    service igmp_proxy restart
fi

##########################################################
# Need restart this servieces only:                    	 #
# 1) if not VPN enable                               	 #
# 2) if VPN enable and this scripts called from ip-up	 #
# 3) if restart mode = all				 #
##########################################################
if [ "$MODE" = "pppd" ] || [ "$MODE" = "all" ] || [ "$vpnEnabled" != "on" ]; then
    service ddns restart
    service ntp restart
    service upnp restart
fi

# if dnsmasq disabled need update udhcpd.conf always
if [ "$dnsPEnabled" != "1" ] && [ "$wan_static_dns" != "on" ]; then
    if [ "$MODE" = "pppd" ] || [ "$MODE" = "dhcp" ]; then 
	service dhcpd restart
    fi
fi
