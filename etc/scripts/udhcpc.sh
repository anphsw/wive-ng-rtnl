#!/bin/sh

# Wive-NG  udhcpc script

# include global config
. /etc/scripts/global.sh

[ -z "$1" ] && echo "Error: should be called from udhcpc" && exit 1

LOG="logger -t udhcpc"
RESOLV_CONF="/etc/resolv.conf"

wan_manual_mtu=`nvram_get 2860 wan_manual_mtu`

# Full route list
ROUTELIST=""
# Default gateway list
ROUTELIST_DGW=""
# Stub for first hop`s dgw
ROUTELIST_FGW=""

# Get VPN DGW mode
vpnDGW=`nvram_get 2860 vpnDGW`
# If pppoe mode and dgw in pppoe no need replace default gw
replace_dgw=1
if [ "$vpnEnabled" = "on" ] && [ "$vpnDGW" = "1" ] && [ "$vpnType" = "0" ]; then
    replace_dgw=0
fi

[ -n "$broadcast" ] && BROADCAST="broadcast $broadcast"
[ -n "$subnet" ] && NETMASK="netmask $subnet"

case "$1" in
    deconfig)
    vpn_deadloop_fix
    ip addr flush dev $interface
    if [ -d /proc/sys/net/ipv6 ]; then
	ip -6 addr flush dev $interface
    fi
    ip link set $interface up
    ;;

    leasefail)
    # Try reconnect at lease failed
    # Workaround for infinite OFFER wait
    vpn_deadloop_fix
    if [ "$opmode" = "2" ]; then
	# Wait connect and get SSID
	wait_connect
	if [ "$staCur_SSID" != "" ]; then
	    # Reconnect
	    $LOG "Reconnect to STA $staCur_SSID"
	    iwconfig ra0 essid "$staCur_SSID"
	fi
	# Full reinit network and services
	internet.sh
    else
	dhcpSwReset=`nvram_get 2860 dhcpSwReset`
	if [ "$dhcpSwReset" = "1" ]; then
	    $LOG "Reinit switch"
	    # Reset switch
	    /etc/scripts/config-switch.sh
	fi
    fi
    ;;

    renew|bound)
    OLD_IP=`ip -4 addr show dev $interface | awk '/inet / {print $2}'`
    NEW_IP="$ip"

	# MTU is default for all session time.
	if [ "$OLD_IP" != "$NEW_IP" ]; then
	    $LOG "Renew ip adress $NEW_IP and $NETMASK for $interface from dhcp"
	    ifconfig $interface $NEW_IP $BROADCAST $NETMASK
	    # Get MTU from dhcp server
	    if [ "$mtu" ] && [ "$wan_manual_mtu" = "0" ]; then
		$LOG "Set MTU to $mtu bytes from dhcp server"
		ip link set mtu $mtu dev $interface
	    fi
	fi

	# Get default gateway
	if [ -n "$router" ]; then
	    # default route with metric 0 is through $iface?
	    dgw_otherif=`ip route | grep "default" | grep -v "dev $interface " | sed 's,.*dev \([^ ]*\) .*,\1,g'`
	    if [ -z "$dgw_otherif" ]; then
		# if ip not changed not need delete old default route
		# this is workaroud for ppp used tunnels up over not default routes
    		if [ "$OLD_IP" != "$NEW_IP" ]; then
		    if [ "$replace_dgw" = "1" ]; then
			$LOG "Deleting default route dev $interface"
			while ip route del default dev $interface ; do
			    :
			done
		    fi
		fi

		metric=0
		for i in $router ; do
		    $LOG "Add default route $i dev $interface metric $metric"
		    ROUTELIST_FGW="$ROUTELIST_FGW $i/32:0.0.0.0:$interface:"
		    if [ "$replace_dgw" = "1" ]; then
			ROUTELIST_DGW="$ROUTELIST_DGW default:$i:$interface:$metric"
		    fi
		    # save first dgw with metric=1 to use in corbina hack
		    if [ "$metric" = "0" ]; then
			echo $i > /tmp/default.gw
			first_dgw="$i"
		    fi
    		    metric=`expr $metric + 1`
		done
	    fi
	fi

	# classful routes
	if [ -n "$routes" ]; then
	    for i in $routes; do
		NW=`echo $i | sed 's,/.*,,'`
		GW=`echo $i | sed 's,.*/,,'`
		shift 1
		MASK=`echo $NW | awk '{w=32; split($0,a,"."); \
		    for (i=4; i>0; i--) {if (a[i]==0) w-=8; else {\
		    while (a[i]%2==0) {w--; a[i]=a[i]/2}; break}\
		    }; print w }'`
		if [ "$GW" = "0.0.0.0" ] || [ -z "$GW" ]; then
		    ip route replace $NW/$MASK dev $interface
		else
		    ROUTELIST="$ROUTELIST $NW/$MASK:$GW:$interface:"
		fi
	    done
	fi

	# MSSTATIC ROUTES AND STATIC ROUTES (rfc3442)
	ROUTES="$staticroutes $msstaticroutes"

	if [ "$ROUTES" != " " ]; then
	    set $ROUTES
	    while [ -n "$1" ]; do
		NW="$1"
		GW="$2"
		shift 2
		if [ "$GW" = "0.0.0.0" ] || [ -z "$GW" ]; then
		    ip route replace $NW dev $interface
		else
		    ROUTELIST="$ROUTELIST $NW:$GW:$interface:"
		fi
	    done
	fi

	# first add stub for routesm next add static routes and
	# default gateways need replace/add at end route parces
	if [ "$replace_dgw" = "1" ]; then
	    ROUTELIST="$ROUTELIST_FGW $ROUTELIST $ROUTELIST_DGW"
	else
	    ROUTELIST="$ROUTELIST_FGW $ROUTELIST"
	fi
	for i in `echo $ROUTELIST | sed 's/ /\n/g' | sort | uniq`; do
		IPCMD=`echo $i|awk '{split($0,a,":"); \
		    printf " %s via %s dev %s", a[1], a[2], a[3]; \
		    if (a[4]!="") printf " metric %s", a[4]}'`
		ip route replace $IPCMD
	done

	# add routes configured in web
	if [ -f /etc/routes_replace ]; then
	    $LOF "Add user routes."
	    /etc/routes_replace replace $lan_if $wan_if
	fi

        if [ "$OLD_IP" != "$NEW_IP" ]; then
	    # Get DNS servers
	    if [ "$wan_static_dns" != "on" ]; then
		if [ "$dns" ]; then
		    $LOG "Renew DNS from dhcp $dns $domain"
		    rm -f $RESOLV_CONF
        	    # get domain name
    		    [ -n "$domain" ] && echo domain $domain >> $RESOLV_CONF
		    # parce dnsservers
		    for i in $dns ; do
	    		echo nameserver $i >> $RESOLV_CONF
			ROUTE_NS=`ip route get "$i" | grep dev | cut -f -3 -d " "`
			if [ "$ROUTE_NS" != "" ] && [ "$i" != "$first_dgw" ]; then
			    $LOG "Add static route to DNS $ROUTE_NS dev $interface"
			    REPLACE="ip route replace $ROUTE_NS dev $interface"
			    $REPLACE
			fi
		    done
		else
		    $LOG "Server not send DNS. Please manual set DNS in wan config."
		fi
	    else
		$LOG "Use static DNS."
	    fi
		# read for all write by root
		chmod 644 "$RESOLV_CONF" > /dev/null 2>&1
		$LOG "Restart needed services"
		services_restart.sh dhcp
	fi

	# if dhcp disables restart must from internet.sh
	# this is restart vpn and others if need
    	if [ "$OLD_IP" != "$NEW_IP" ]; then
	    # send Cisco Discovery request
	    if [ -f /bin/cdp-send ] && [ -f /etc/scripts/config-cdp.sh ]; then
		config-cdp.sh &
	    fi
	    # restart vpn if ip changed
	    if [ "$vpnEnabled" = "on" ]; then
		PPPD=`pidof pppd`
		XL2TPD=`pidof xl2tpd`
		service vpnhelper stop
		# wait ip-down script work
		if [ "$PPPD" != "" ] || [ "$XL2TPD" != "" ]; then
		    sleep 10
		fi
		$LOG "Restart vpnhelper.."
		service vpnhelper start
	    fi
	fi
	echo 1 > "/proc/sys/net/ipv4/conf/$interface/forwarding"
	if [ "$OLD_IP" != "$NEW_IP" ]; then
	    $LOG "End renew procedure..."
	fi
    ;;
esac
