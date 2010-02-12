#!/bin/sh

# udhcpc script edited by Tim Riker <Tim@Rikers.org>

[ -z "$1" ] && echo "Error: should be called from udhcpc" && exit 1

LOG="logger -t udhcpc"
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
                $LOG "PPPD IS STARTED!!! No new ip or routers change"
        else
            $LOG "Renew ip adress and other parametrs from dhcp"
    	    ifconfig $interface $ip $BROADCAST $NETMASK
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
	fi
	# DNS
        echo -n > $RESOLV_CONF
        #[ -n "$domain" ] && echo search $domain >> $RESOLV_CONF
	$LOG "Renew DNS from dhcp"
        for i in $dns ; do
            echo adding dns $i
            echo nameserver $i >> $RESOLV_CONF
        done
	# CIDR STATIC ROUTES (rfc3442)
	[ -n "$staticroutes" ] && {
		$LOG "Add static routes from dhcpd"
		# This defines how many CIDR Routes can be assigned so that we do not enter
		# an endless loop on malformed data
		MAXstaticroutesS=24;
		while [ ${MAXstaticroutesS} -gt "0" ]; do
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
			let MAXstaticroutesS=$MAXstaticroutesS-1;

			# Leave the loop if staticroutess is empty (we've parsed everything)
			[ ! -n "$staticroutes" ] && break

		done
	}

	    # notify goahead when the WAN IP has been acquired. --yy
	    killall -SIGUSR2 goahead
	    # restart needed services
    	    if [ "$STARTEDPPPD" != "0" ]; then
                $LOG "No need restart setvices"
    	    else
		$LOG "Restart needed services"
		services_restart.sh dhcp
	    fi
	    $LOG "Renew OK.."
        ;;
esac

exit 0

