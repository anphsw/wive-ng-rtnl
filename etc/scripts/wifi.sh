#!/bin/sh

#include kernel config and global
. /etc/scripts/config.sh
. /etc/scripts/global.sh

if [ "$ethconv" != "n" ]; then
    exit 0
fi

echo ">>>>> RECONFIGURE WIFI <<<<<<<<<<"
########################################get param#########################
radio_off=`nvram_get 2860 RadioOff`
CountryRegion=`nvram_get 2860 CountryRegion`
WirelessMode=`nvram_get 2860 WirelessMode`
HT_BW=`nvram_get 2860 HT_BW`
HT_GI=`nvram_get 2860 HT_GI`
HT_PROTECT=`nvram_get 2860 HT_PROTECT`
meshenabled=`nvram_get 2860 MeshEnabled`
meshhostname=`nvram_get 2860 MeshHostName` 
bssidnum=`nvram_get 2860 BssidNum`
m2uenabled=`nvram_get 2860 M2UEnabled`

########################################set param#########################

if [ "$radio_off" = "1" ]; then
    iwpriv ra0 set RadioOn=0
else
    iwpriv ra0 set RadioOn=1
fi

iwpriv ra0 set CountryRegion=$CountryRegion
iwpriv ra0 set WirelessMode=$WirelessMode
iwpriv ra0 set HtBw=$HT_BW
iwpriv ra0 set HtGi=$HT_GI
iwpriv ra0 set HtProtect=$HT_PROTECT

if [ "$meshenabled" = "1" ]; then
    iwpriv mesh0 set  MeshHostName="$meshhostname"
fi
if [ "$m2uenabled" = "1" ]; then
    iwpriv ra0 set IgmpSnEnable=1
fi
if [ "$ethconv" = "y" ]; then
    iwpriv ra0 set EthConvertMode=dongle
fi
