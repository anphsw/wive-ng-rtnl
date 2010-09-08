#!/bin/sh

# udhcpc script edited by Tim Riker <Tim@Rikers.org>

#include kernel config                                                                                                                      
. /bin/config.sh

[ -z "$1" ] && echo "Error: should be called from udhcpc" && exit 1

LOG="logger -t udhcpc"
RESOLV_CONF="/etc/resolv.conf"
STARTEDPPPD=`ip link show up | grep ppp -c` > /dev/null 2>&1
STATICDNS=`nvram_get 2860 wan_static_dns`

[ -n "$broadcast" ] && BROADCAST="broadcast $broadcast"
[ -n "$subnet" ] && NETMASK="netmask $subnet"

case "$1" in
    deconfig)
        ip addr flush dev $interface
if [ "$CONFIG_IPV6" != "" ] ; then
        ip -6 addr flush dev $interface
fi
	touch /var/tmp/is_up/force_renew
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
	if [ "$ip" != "" ]; then
	    nvram_set 2860 wan_ipaddr $ip
	fi
	if [ "$subnet" != "" ]; then
	    nvram_set 2860 wan_netmask $subnet
	fi
	rm -rf /var/tmp/is_up
	mkdir -p /var/tmp/is_up
	echo $ip $NETMASK > /var/tmp/is_up/$ip
        
	#Get default gateway
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

	#Get DNS servers
	if [ "$STATICDNS" != "on" ] && [ "$dns" ]; then
	    $LOG "Renew DNS from dhcp"
	    count=0                                                                                                         
	    rm -f $RESOLV_CONF                                                                                                          
	    for i in $dns ; do
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
				route add $NW gw $GW dev $interface
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

        # notify goahead when the WAN IP has been acquired. --yy
	killall -SIGUSR2 goahead

    	$LOG "Restart needed services"
	services_restart.sh dhcp

	#remove force renew flag
	rm -f /var/tmp/is_up/force_renew
    fi
	$LOG "Renew OK.."
        ;;
esac

exit 0
