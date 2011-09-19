#!/bin/sh

#include global config
. /etc/scripts/global.sh

echo ">>>>> RECONFIGURE WIFI <<<<<<<<<<"

########################################ALLMODE param##########################
HiPower=`nvram_get 2860 HiPower`
if [ "$HiPower" = "1" ]; then
#Disable MIMO PowerSave and increase LNA gain
    iwpriv ra0 set HiPower=1
    iwpriv ra0 set HtMimoPs=0
else
#Enable MIMO PowerSave and set LNA gain to default
    iwpriv ra0 set HiPower=0
    iwpriv ra0 set HtMimoPs=1
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
RadioOff=`nvram_get 2860 RadioOff`
if [ "$RadioOff" = "1" ]; then
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
    M2UEnabled=`nvram_get 2860 M2UEnabled`
    if [ "$M2UEnabled" != "" ]; then
	iwpriv ra0 set IgmpSnEnable="$M2UEnabled"
    fi
fi

###############################################Radar,DFS,CD########################
CarrierDetect=`nvram_get CarrierDetect`
if [ "$CarrierDetect" = "1" ]; then
    iwpriv ra0 set CarrierDetect=1
    iwpriv ra0 set RadarStart=1
else
    iwpriv ra0 set CarrierDetect=0
    iwpriv ra0 set RadarStop=1
fi
###############################################Channel select#######################
AutoChannelSelect=`nvram_get AutoChannelSelect`
if [ "$AutoChannelSelect" = "1" ]; then
    #rescan and select optimal channel
    iwpriv ra0 set SiteSurvey=1
    iwpriv ra0 set AutoChannelSel=1
else
    #set channel manual
    Channel=`nvram_get Channel`
    iwpriv ra0 set Channel=$Channel
fi

###########################################ALWAYS END#############################
AP2040Rescan=`nvram_get AP2040Rescan`
if [ "$AP2040Rescan" != "0" ]; then
    iwpriv ra0 set AP2040Rescan=1
fi
