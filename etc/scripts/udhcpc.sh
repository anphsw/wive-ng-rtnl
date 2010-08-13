#!/bin/sh

# udhcpc script edited by Tim Riker <Tim@Rikers.org>

#include kernel config                                                                                                                      
. /bin/config.sh

[ -z "$1" ] && echo "Error: should be called from udhcpc" && exit 1

LOG="logger -t udhcpc"
RESOLV_CONF="/etc/resolv.conf"
STARTEDPPPD=`ip link show up | grep ppp -c` > /dev/null 2>&1
STATICDNS=`nvram_get 2860 wan_static_dns`

set_classless_routes() {
       local max=128
       local type
       while [ -n "$1" -a -n "$2" -a $max -gt 0 ]; do
               [ ${1##*/} -eq 32 ] && type=host || type=net
               $LOG "adding route for $type $1 via $2"
               route add -$type "$1" gw "$2" dev "$interface"
               max=$(($max-1))
               shift 2
       done
}

[ -n "$broadcast" ] && BROADCAST="broadcast $broadcast"
[ -n "$subnet" ] && NETMASK="netmask $subnet"

case "$1" in
    deconfig)
        ip addr flush dev $interface
if [ "$CONFIG_IPV6" != "" ] ; then
        ip -6 addr flush dev $interface
fi
	ip link set $interface up
        ;;

    renew|bound)
    #force renew if needed
    if [ -f /var/tmp/is_up/force_renew ]; then
	STARTEDPPPD=0
	rm -rf /var/tmp/is_up
    fi
    #no change routes if pppd is started
    if [ "$STARTEDPPPD" != "0" ] || [ -f /var/tmp/is_up/$ip ]; then
            $LOG "PPPD is start or no change parametrs. No renew needed."
    else
	    $LOG "Renew ip adress $ip and $NETMASK for $interface from dhcp"
            ifconfig $interface $ip $BROADCAST $NETMASK
	    nvram_set 2860 wan_ipaddr $ip
	    nvram_set 2860 wan_netmask $NETMASK
	    rm -rf /var/tmp/is_up
	    mkdir -p /var/tmp/is_up
	    echo $ip $NETMASK > /var/tmp/is_up/$ip
    	    if [ -n "$router" ] ; then
        	$LOG "Deleting default route"
        	while route del default gw 0.0.0.0 dev $interface ; do
            	    :
        	done
        	metric=0
        	for i in $router ; do
        	    $LOG "Add default route $i dev $interface metric $metric"
            	    route add default gw $i dev $interface metric $metric
		    #save first dgw with metric=1 to use in corbina hack
		    if [ "$metric" = "0" ]; then
			echo $i > /etc/default.gw
		    fi
            	    metric=`expr $metric + 1`
        	done
    	    fi
	# DNS
	if [ "$STATICDNS" != "on" ]; then
	 if [ "$dns" ]; then 
	    $LOG "Renew DNS from dhcp"
	    count=0                                                                                                         
	    rm -f $RESOLV_CONF                                                                                                          
	    for i in $dns
    	    do
		$LOG "DNS= $i"
		echo nameserver $i >> $RESOLV_CONF
		    if [ "$count" = "0" ]; then
			nvram_set 2860 wan_primary_dns $i
		    fi
		    if [ "$count" = "1" ]; then
			nvram_set 2860 wan_secondary_dns $i
		    fi
		    let "count=$count+1"
	    done
	 fi
	fi      

	# CIDR STATIC ROUTES (rfc3442)
	[ -n "$staticroutes" ] && set_classless_routes $staticroutes
	[ -n "$msroutes" ] && set_classless_routes $msroutes
	[ -n "$msstaticroutes" ] && set_classless_routes $msstaticroutes

        # notify goahead when the WAN IP has been acquired. --yy
	killall -SIGUSR2 goahead
    	$LOG "Restart needed services"
	services_restart.sh dhcp
    fi
	$LOG "Renew OK.."
        ;;
esac

exit 0
