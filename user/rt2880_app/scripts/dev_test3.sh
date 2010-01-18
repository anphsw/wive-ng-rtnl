#!/bin/sh

#. /sbin/config.sh
#. /sbin/global.sh

lan_ip=`nvram_get 2860 lan_ipaddr`
stp_en=`nvram_get 2860 stpEnabled`
nat_en=`nvram_get 2860 natEnabled`
bssidnum=`nvram_get 2860 BssidNum`
radio_off=`nvram_get 2860 RadioOff`

if [ "$lan_ip" = "" ]; then
    lan_ip="192.168.1.1"
    nvram_set 2860 lan_ipaddr 192.168.1.1
fi

echo "dev init done"
