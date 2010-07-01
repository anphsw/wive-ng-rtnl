#!/bin/sh

# udhcpc script edited by Tim Riker <Tim@Rikers.org>

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
        ip -6 addr flush dev $interface
	ip link set $interface up
        ;;

    renew|bound)
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
        	$LOG "Add default route"
        	for i in $router ; do
            	    metric=`expr $metric + 1`
            	    route add default gw $i dev $interface metric $metric
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
    fi
	    $LOG "Renew OK.."
        ;;
esac

exit 0
