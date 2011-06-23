#!/bin/sh

#include global config
. /etc/scripts/global.sh

echo ">>>>> RECONFIGURE WIFI <<<<<<<<<<"

########################################ALLMODE param##########################
HiPower=`nvram_get 2860 HiPower`
if [ "$HiPower" = "1" ]; then
    iwpriv ra0 set HiPower=1
else
    iwpriv ra0 set HiPower=0
fi

########################################STAMODE param##########################
AutoConnect=`nvram_get 2860 AutoConnect`
if [ "$opmode" = "2" ]; then
    if [ "$AutoConnect" = "1" ]; then
	iwpriv ra0 set AutoReconnect=1
    fi
  #in sta mode exit
  exit 0
fi

#########################################ON/OFF param##########################
radio_off=`nvram_get 2860 RadioOff`
if [ "$radio_off" = "1" ]; then
    iwpriv ra0 set RadioOn=0
    echo ">>>> WIFI DISABLED <<<<"
    exit 0
else
    iwpriv ra0 set RadioOn=1
fi

########################################MULTICAST param###########################
if [ "$CONFIG_RT2860V2_AP_IGMP_SNOOP" != "" ]; then
    if [ "$CONFIG_RT2860V2_MCAST_RATE_SPECIFIC" != "" ]; then
	McastPhyMode=`nvram_get 2860 McastPhyMode`
        if [ "$McastPhyMode" != "" ]; then
	    iwpriv ra0 set McastPhyMode=$McastPhyMode
	fi
	McastMcs=`nvram_get 2860 McastMcs`
	if [ "$McastMcs" != "" ]; then
    	    iwpriv ra0 set  McastMcs="$McastMcs"
	fi
    fi
    m2uenabled=`nvram_get 2860 M2UEnabled`
    if [ "$m2uenabled" != "" ]; then
	iwpriv ra0 set IgmpSnEnable="$m2uenabled"
    fi
fi

########################################MESH mode param###########################
if [ "$CONFIG_RT2860V2_STA_MESH" != "" ] || [ "$CONFIG_RT2860V2_AP_MESH" != "" ]; then
    meshenabled=`nvram_get 2860 MeshEnabled`
    if [ "$meshenabled" = "1" ]; then
        meshhostname=`nvram_get 2860 MeshHostName` 
	iwpriv mesh0 set  MeshHostName="$meshhostname"
    fi
fi

###############################################Others#############################
AutoChannelSelect=`nvram_get AutoChannelSelect`
if [ "$AutoChannelSelect" = "0" ]; then
    Channel=`nvram_get Channel`
    iwpriv ra0 set Channel=$Channel
fi

###########################################ALWAYS END#############################
AP2040Rescan=`nvram_get AP2040Rescan`
if [ "$AP2040Rescan" != "0" ]; then
    iwpriv ra0 set AP2040Rescan=1
fi
