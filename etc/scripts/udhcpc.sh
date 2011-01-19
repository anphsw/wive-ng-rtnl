#!/bin/sh

# Wive-NG  udhcpc script

#include kernel config
. /etc/scripts/config.sh
#include global config
. /etc/scripts/global.sh

[ -z "$1" ] && echo "Error: should be called from udhcpc" && exit 1

LOG="logger -t udhcpc"
RESOLV_CONF="/etc/resolv.conf"
STATICDNS=`nvram_get 2860 wan_static_dns`
ROUTELIST=""

[ -n "$broadcast" ] && BROADCAST="broadcast $broadcast"
[ -n "$subnet" ] && NETMASK="netmask $subnet"

case "$1" in
    deconfig)
    vpn_deadloop_fix
    ip addr flush dev $interface
    if [ "$CONFIG_IPV6" != "" ]; then
	ip -6 addr flush dev $interface
    fi
    ip link set $interface up
    ;;

    leasefail)
    # Try reconnect at lease failed
    # Workaround for infinite OFFER wait
    vpn_deadloop_fix
    if [ "$stamode" = "y" ]; then
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
	    resetLanWan
	    if [ "$opmode" = "1" ]; then
		# In gate mode configure vlans
		setLanWan
	    fi
	fi
    fi
    ;;

    renew|bound)
    #no change routes if pppd is started
    $LOG "Renew ip adress $ip and $NETMASK for $interface from dhcp"
    OLD_IP=`ip addr show dev $interface | awk '/inet / {print $2}'`
    ifconfig $interface $ip $BROADCAST $NETMASK
    CUR_IP=`ip addr show dev $interface | awk '/inet / {print $2}'`

	#Get default gateway
	if [ -n "$router" ]; then
	    #default route with metric 0 is through $iface?
	    def_iface=`ip route | grep "default" | grep -v "metric" | sed 's,.*dev \([^ ]*\) .*,\1,g'`
	    if [ -n "$def_iface" -a "$def_iface" != "$interface" ]; then
		metric=1
		smetric=1
	    else
		metric=0
		smetric=0
	    fi
	    $LOG "Deleting default route"
	    while ip route del default dev $interface ; do
		:
	    done

	    for i in $router ; do
		$LOG "Add default route $i dev $interface metric $metric"
		ROUTELIST="$ROUTELIST default:$router:$interface:$metric"
		#save first dgw with metric=1 to use in corbina hack
		if [ "$metric" = "$smetric" ]; then
		    echo $i > /tmp/default.gw
		    first_dgw="$i"
		fi
    		metric=`expr $metric + 1`
	    done
	fi

	#classful routes
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

	#MSSTATIC ROUTES AND STATIC ROUTES (rfc3442)
	ROUTES="$staticroutes $msstaticroutes"
	
	if [ "$ROUTES" != " " ]; then
	    set $ROUTES
	    while [ -n "$1" ]; do
		NW=$1
		GW=$2
		shift 2
		if [ "$GW" = "0.0.0.0" ] || [ -z "$GW" ]; then
		    ip route replace $NW dev $interface
		else
		    ROUTELIST="$ROUTELIST $NW:$GW:$interface:"
		fi
	    done
	fi

	for i in `echo $ROUTELIST | sed 's/ /\n/g' | sort | uniq`; do
		IPCMD=`echo $i|awk '{split($0,a,":"); \
		    printf " %s via %s dev %s", a[1], a[2], a[3]; \
		    if (a[4]!="") printf " metric %s", a[4]}'`
		ip route replace $IPCMD
	done

	#Get DNS servers
	if [ "$STATICDNS" != "on" ] && [ "$dns" ]; then
	    $LOG "Renew DNS from dhcp"
	    rm -f $RESOLV_CONF
            #get domain name
    	    [ -n "$domain" ] && echo domain $domain >> $RESOLV_CONF
	    #parce dnsservers
	    for i in $dns ; do
	        $LOG "DNS= $i"
	        echo nameserver $i >> $RESOLV_CONF
		ROUTE_NS=`ip route get "$i" | grep dev | cut -f -3 -d " "`
		if [ "$ROUTE_NS" != "" ] && [ "$i" != "$first_dgw" ]; then
		    $LOG "Add static route to DNS $ROUTE_NS dev $interface"
		    REPLACE="ip route replace $ROUTE_NS dev $interface"
		    $REPLACE
		fi
	    done
	else
	    $LOG "Use static DNS."
	fi
        if [ "$OLD_IP" != "$CUR_IP" ]; then
		$LOG "Restart needed services"
		services_restart.sh dhcp
	fi
	$LOG "Renew OK.."
    ;;
esac
