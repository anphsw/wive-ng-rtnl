#!/bin/sh

# This scipt restart needed services

# include global config
. /etc/scripts/global.sh

LOG="logger -t services"
MODE="$1"

$LOG "Restart needed services and scripts. Mode $MODE"

##########################################################
# MODES:						 #
# all -  call from internel.sh - full reconfigure	 #
# dhcp - call from dhcp script after full renew		 #
# pppd - call from ip-up/down scripts			 #
# misc - call from goahead by apply change in misc page	 #
##########################################################

##########################################################
# Always reload shaper and netfilter rules		 #
##########################################################
    service shaper restart
    service iptables restart

##########################################################
# Always reload some services				 #
##########################################################
    service dnsserver reload
    if [ -e /etc/init.d/zebra ]; then
	service ripd restart
        service zebra restart
    fi
    if [ -e /etc/init.d/radvd ] && [ -d /proc/sys/net/ipv6 ]; then
	service radvd restart
    fi
    service ddns restart
    service ntp restart
    service miniupnpd restart
    if [ -e /etc/init.d/parprouted ]; then
	service parprouted restart
    fi

##########################################################
# Need restart this:					 #
# 1) if physycal wan ip adress change			 #
# 2) if apply changes from web				 #
# 3) if full reconfigure				 #
##########################################################
if [ "$MODE" = "dhcp" ] || [ "$MODE" = "misc" ] || [ "$MODE" = "all" ]; then
    if [ -e /etc/init.d/igmp_proxy ]; then
        service igmp_proxy restart
    fi
    if [ -e /etc/init.d/xupnpd ]; then
	service xupnpd restart
    fi
    if [ -e /etc/init.d/udpxy ]; then
	service udpxy restart
    fi
fi

##########################################################
# Need restart this:					 #
# 1) if misc chane apply				 #
# 2) full reconfigure					 #
##########################################################
if [ "$MODE" = "misc" ] || [ "$MODE" = "all" ]; then
    service kext restart
    if [ -e /etc/init.d/lld2d ]; then
	service lld2d restart
    fi
    if [ -e /etc/init.d/snmpd ]; then
        service snmpd restart
    fi
    if [ -e /etc/init.d/inetd ]; then
        service inetd restart
    fi
    if [ -e /etc/init.d/chillispot ]; then
        service chillispot restart
    fi
    if [ -e /etc/init.d/transmission ]; then
	service transmission restart
    fi
fi

##########################################################
# Reconfigure and restart samba:			 #
# 1) if recive wins server adress from dhcp		 #
##########################################################
if [ "$MODE" = "dhcp" ] && [ -f /tmp/wins.dhcp ]; then
    service samba restart
fi

# renew /etc/udhcpd.conf and restart dhcp server
if [ "$dnsPEnabled" = "1" -o "$wan_static_dns" = "on" ] && [ "$MODE" = "misc" -o "$MODE" = "all" ]; then
    # if dnsmasq or static dns enabled and aplly at web or full reconfigure
    service dhcpd restart
elif [ "$dnsPEnabled" != "1" -a "$wan_static_dns" != "on" ] && [ "$MODE" = "pppd" -o "$MODE" = "dhcp" ]; then
    # if dnsmasq or static dns disabled and mode=pppd/dhcp (renew/reconnect ISP)
    sleep 3
    service dhcpd restart
fi

###########################################################
# need restart L2TP server every netfilter rules replaced
# or ppp session to uplink restarted or new adress recieved
###########################################################
service vpnserver restart
