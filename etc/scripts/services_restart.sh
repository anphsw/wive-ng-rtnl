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
if [ -f /etc/init.d/radvd ] && [ -d /proc/sys/net/ipv6 ]; then
    service radvd restart
fi

if [ -f /etc/init.d/zebra ]; then
    service ripd restart
    service zebra restart
fi

    service dnsserver reload

##########################################################
# Need restart this servieces only:                    	 #
# 1) if not VPN enable                               	 #
# 2) if VPN enable and this scripts called from ip-up	 #
# 3) if DHCP full renew					 #
# 4) if restart mode = all				 #
##########################################################
if [ "$MODE" = "pppd" ] || [ "$MODE" = "dhcp" ] || [ "$MODE" = "all" ] || [ "$vpnEnabled" != "on" ]; then
    service ddns restart
    service ntp restart
    service miniupnpd restart
fi

##########################################################
# Need restart this servieces only:			 #
# 1) if call not from ip-up				 #
# 2) if call not from dhcp script			 #
##########################################################
if [ "$MODE" != "pppd" ] && [ "$MODE" != "dhcp" ]; then
    service kext start
    # only misc reply
    if [ "$MODE" = "misc" ]; then
	if [ -e /etc/init.d/snmpd ]; then
	    service snmpd restart
	fi
	if [ -e /etc/init.d/inetd ]; then
	    service inetd restart
	fi
    else
	# exclude misc reply
	if [ -e /etc/init.d/pppoe-relay ]; then
	    service pppoe-relay restart
	fi
	if [ -e /etc/init.d/chillispot ]; then
	    service chillispot restart
	fi
    fi
    if [ -e /etc/init.d/parprouted ]; then
	service parprouted restart
    fi
    if [ -e /etc/init.d/lld2d ]; then
	service lld2d restart
    fi
    if [ -e /etc/init.d/transmission ]; then
	service transmission restart
    fi
    if [ -e /etc/init.d/xupnpd ]; then
	service xupnpd restart
    fi
    if [ -e /etc/init.d/udpxy ]; then
	service udpxy restart
    fi
    if [ -e /etc/init.d/igmp_proxy ]; then
        service igmp_proxy restart
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
