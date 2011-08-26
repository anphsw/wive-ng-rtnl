#!/bin/sh

ping_check_time=`nvram_get 2860 ping_check_time`
ping_check_interval=`nvram_get 2860 ping_check_interval`

if [ "$ping_check_time" = "" ] || [ "$ping_check_time" = "0" ] || \
   [ "$ping_check_interval" = "" ] || [ "$ping_check_interval" = "0" ]; then
    exit 0
fi

while "true"; do
    ##################################RADIO###########################################
    RadioOff=`nvram_get 2860 RadioOff`
    if [ "$RadioOff" != "1" ]; then
	#Unsolicited ARP mode, update your neighbors
	arping -U 255.255.255.255 -w1 -Ibr0 -b -c1 -q
	arping -A 255.255.255.255 -w1 -Ibr0 -b -c1 -q

	#arping for client wakeup - from arp table
	grep -v "IP" < /proc/net/arp | awk '{ print $1 }' | while read test_ip; do
	    arping "$test_ip" -I br0 -f -q -w1
	done

	sleep $ping_check_interval

	#arping for client wakeup - from dhcp lease table
	dumpleases | grep -v "IP" | awk '{ print $2 }' | while read test_ip; do
	    arping "$test_ip" -I br0 -f -q -w1
	done
    fi
    ###################################UPLINK#############################################
    sleep $ping_check_interval

    DNS=`cat /etc/resolv.conf | grep -v "domain" | awk {' print $2 '}`
    TGW=`route -n | grep -v "Dest" | grep -v "IP" | grep -v "0\.0\.0\.0" | awk {' print $2 '} | uniq`
    GOG="8.8.8.8"
    ALL="$DNS $TGW $GOG"
    for addr in "$ALL"; do
        ping -4 -s8 -c1 -w30 -q "$addr" > /dev/null 2>&1
    done
done
