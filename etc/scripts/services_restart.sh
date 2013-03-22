#!/bin/sh

# This scipt restart needed services

# include global config
. /etc/scripts/global.sh

LOG="logger -t services"
MODE="$1"

$LOG "Restart needed services and scripts. Mode $MODE"

##########################################################
# Always reload shaper and netfilter rules		 #
##########################################################
    service shaper restart
    service iptables restart

##########################################################
# This is services restart always                        #
##########################################################
if [ -f /bin/radvd ] && [ -d /proc/sys/net/ipv6 ]; then
    service radvd restart
fi
if [ -f /bin/zebra ]; then
    service ripd restart
    service zebra restart
fi
    service dnsserver reload

##########################################################
# Need restart this servieces only:                    	 #
# 1) if not VPN enable                               	 #
# 2) if VPN enable and this scripts called from ip-up	 #
# 3) if restart mode = all				 #
##########################################################
if [ "$MODE" = "pppd" ] || [ "$MODE" = "all" ] || [ "$vpnEnabled" != "on" ]; then
    service ddns restart
    service ntp restart
    service miniupnpd restart
    service xupnpd restart
fi

##########################################################
# Need restart this servieces only:			 #
# 1) if call not from ip-up				 #
# 2) if call not from dhcp script			 #
##########################################################
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ]; then
    service kext start
    if [ "$MODE" = "misc" ]; then
	# only misc reply
	service snmpd restart
	service inetd restart
    else
	# exclude misc reply
	if [ -f /bin/pppoe-relay ]; then
	    service pppoe-relay restart
	fi
	if [ -f /bin/chilli ]; then
	    service chillispot restart
	fi
    fi
    service parprouted restart
    service igmp_proxy restart
    if [ -f /bin/lld2d ]; then
	service lld2d restart
    fi
    if [ -f /bin/udpxy ]; then
	service udpxy restart
    fi
    if [ -f /bin/transmission-daemon ]; then
	service transmission restart
    fi
fi

# renew /etc/udhcpd.conf and restart dhcp server
if [ "$dnsPEnabled" = "1" -o "$wan_static_dns" = "on" ] && [ "$MODE" != "pppd" -a "$MODE" != "dhcp" ]; then
	# if dnsmasq or static dns enabled and mode=!pppd/!dhcp (aplly at web)
	service dhcpd restart
elif [ "$dnsPEnabled" != "1" -a "$wan_static_dns" != "on" ] && [ "$MODE" = "pppd" -o "$MODE" = "dhcp" ]; then
	# if dnsmasq or static dns disabled and mode=pppd/dhcp (renew/reconnect ISP)
	sleep 3
	service dhcpd restart
fi
