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

[ -n "$broadcast" ] && BROADCAST="broadcast $broadcast"
[ -n "$subnet" ] && NETMASK="netmask $subnet"

case "$1" in
    deconfig)
    # on STA mode not need deconfig. 
    # After reconnect adress replace automatically.
    if ["$ethconv" = "n" ]; then
	ip addr flush dev $interface
    fi
    if [ "$CONFIG_IPV6" != "" ]; then
	ip -6 addr flush dev $interface
    fi
    ip link set $interface up
    ;;

    renew|bound)
    #no change routes if pppd is started
    $LOG "Renew ip adress $ip and $NETMASK for $interface from dhcp"
    ifconfig $interface $ip $BROADCAST $NETMASK

	#Get default gateway
	if [ -n "$router" ]; then
    	    $LOG "Deleting default route"
    	    while ip route del default dev $interface ; do
        	:
    	    done

    	    metric=0
    	    for i in $router ; do
        	$LOG "Add default route $i dev $interface metric $metric"
		ip route replace default via $i dev $interface metric $metric
    		#save first dgw with metric=1 to use in corbina hack
    		if [ "$metric" = "0" ]; then
		    echo $i > /tmp/default.gw
		    first_dgw="$i"
		fi
    		metric=`expr $metric + 1`
	    done
	fi

	#MSSTATIC ROUTES (rfc3442)
	if [ ! -z "$msroutes" ]; then
	    set $msroutes
	    while true; do
	      case "$1" in
	        0)
	        $LOG "DO NOT set default route in this script"
	        shift 5
	        continue
	        ;;
	        1|2|3|4|5|6|7|8)
	        route="$2.0.0.0/$1"
	        shift 2
	        ;;
	        9|10|11|12|13|14|15|16)
	        route="$2.$3.0.0/$1"
	        shift 3 
	        ;;
	        17|18|19|20|21|22|23|24)
	        route="$2.$3.$4.0/$1"
	        shift 4
	        ;;
	        25|26|27|28|29|30|31|32)
	        route="$2.$3.$4.$5/$1"
	        shift 5
	        ;;
	        "")
	        break
	        ;;
	        *)
	        $LOG "strange bitmask - $1, skipping"
	        shift 5
	        continue
	        ;;
	      esac
	        router="$1.$2.$3.$4" 
	        shift 4
	      case "$router" in
	        0.0.0.0)
	        $LOG "set route to $route via interface $interface"
	        ip route replace $route dev $interface
	        ;;
	        *)
	        $LOG "set route to $route via $router"
	        ip route replace $route via $router
	        ;;
	      esac
	    done 
	fi

	#STATIC ROUTES (rfc3442)
	[ -n "$staticroutes" ] && {
		$LOG "Add static routes from dhcpd"
		# This defines how many CIDR Routes can be assigned so that we do not enter
		# an endless loop on malformed data
		MAXSTATICROUTES=24;
		while [ ${MAXSTATICROUTES} -gt "0" ]; do
			# Format is
			# $MASK $NW $GW
			# $NW == AAA.BBB.CCC.DDD
			# $GW == EEE.FFF.CCC.DDD
			# $MASK AAA.[BBB].[CCC].[DDD] EEE.FFF.GGG.HHH
			#   1    2    3     4     5    6   7   8   9
			MASK=$(echo $staticroutes | awk '{ print $1 }')
			if [ ${MASK} = "0" ] ; then
				# $MASK EEE.FFF.GGG.HHH
				#   1    2   3   5   6
				NW="0"
				GW=$(echo $staticroutes | awk '{ print $2"."$3"."$4"."$5 }' )
			elif [ ${MASK} -le "8" ] ; then
				# $MASK AAA EEE.FFF.GGG.HHH
				#   1    2   3   5   6   7
				NW=$(echo $staticroutes | awk '{ print $2 }' )
				GW=$(echo $staticroutes | awk '{ print $3"."$4"."$5"."$6 }' )
			elif [ ${MASK} -le "16" ] ; then
				# $MASK AAA.BBB EEE.FFF.GGG.HHH
				#   1    2   3   5   6   7   8
				NW=$(echo $staticroutes | awk '{ print $2"."$3 }' )
				GW=$(echo $staticroutes | awk '{ print $4"."$5"."$6"."$7 }' )
			elif [ ${MASK} -le "24" ] ; then
				# $MASK AAA.BBB.CCC EEE.FFF.GGG.HHH
				#   1    2   3   4   5   6   7   8
				NW=$(echo $staticroutes | awk '{ print $2"."$3"."$4 }' )
				GW=$(echo $staticroutes | awk '{ print $5"."$6"."$7"."$8 }' )

			else
				# $MASK AAA.BBB.CCC.DDD EEE.FFF.GGG.HHH
				#   1    2   3   4   5   6   7   8   9
				NW=$(echo $staticroutes | awk '{ print $2"."$3"."$4"."$5 }' )
				GW=$(echo $staticroutes | awk '{ print $6"."$7"."$8"."$9 }' )
			fi
			echo [$ROUTECOUNTER] Route Network: $NW/$MASK Gateway: $GW on $interface

			# TODO: Check for malformed data here to eliminate counter workaround
			# Malformed data is: ... or xxx... or xxx.yyy.. or xxx.yyy.zzz.

			[ -n "$NW" ] && [ -n "$GW" ] && {
				ip route replace $NW via $GW dev $interface
			}

			# Clear the strings incase they don't get set next time around
			if [ ${NW} = "0" ]; then
				NW=""
			fi
			TMP="$MASK $NW $GW "
			NW=""
			GW=""

			# Remove the '.' so that we can delete them from the input with sed
			TMP=$(echo $TMP | sed "s/\./ /g")

			# Remove the previous entry from staticroutes
			staticroutes=$(echo $staticroutes | sed "s/$TMP//g")

			# Add to counter
			let ROUTECOUNTER=$ROUTECOUNTER+1;
			let MAXSTATICROUTES=$MAXSTATICROUTES-1;

			# Leave the loop if staticroutess is empty (we've parsed everything)
			[ ! -n "$staticroutes" ] && break

		done
	}

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

    	$LOG "Restart needed services"
	services_restart.sh dhcp
	$LOG "Renew OK.."
    ;;
esac
