#!/bin/sh

# udhcpc script edited by Tim Riker <Tim@Rikers.org>

[ -z "$1" ] && echo "Error: should be called from udhcpc" && exit 1

RESOLV_CONF="/etc/resolv.conf"
STARTEDPPPD=`ip link show up | grep ppp -c`

[ -n "$broadcast" ] && BROADCAST="broadcast $broadcast"
[ -n "$subnet" ] && NETMASK="netmask $subnet"

case "$1" in
    deconfig)
        /sbin/ifconfig $interface 0.0.0.0
        ;;

    renew|bound)
        #no change routes if pppd is started
        if [ "$STARTEDPPPD" != "0" ]; then
                echo "PPPD IS STARTED!!! No deleting or routers and adresses"
        else
    	    ifconfig $interface $ip $BROADCAST $NETMASK
    	    if [ -n "$router" ] ; then
        	echo "deleting routers"
        	while route del default gw 0.0.0.0 dev $interface ; do
            	    :
        	done

        	metric=0
        	for i in $router ; do
            	    metric=`expr $metric + 1`
            	    route add default gw $i dev $interface metric $metric
        	done
    	    fi
	fi
        echo -n > $RESOLV_CONF
        [ -n "$domain" ] && echo search $domain >> $RESOLV_CONF
        for i in $dns ; do
            echo adding dns $i
            echo nameserver $i >> $RESOLV_CONF
	    service dns stop
	    service upnp stop
	    service dns start
	    service upnp start
        done
		# notify goahead when the WAN IP has been acquired. --yy
		killall -SIGUSR2 goahead
		service igmpproxy stop
		service igmpproxy start
        ;;
esac

exit 0

