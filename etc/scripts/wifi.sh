#!/bin/sh

# include kernel config
. /etc/scripts/config.sh

echo ">>>>> RECONFIGURE WIFI <<<<<<<<<<"

########################################ALLMODE param##########################
eval `nvram_buf_get 2860 HiPower AutoConnect OperationMode`
########################################LNA param##############################
if [ "$HiPower" = "1" ]; then
# Disable MIMO PowerSave and increase LNA gain
    if [ "$CONFIG_RALINK_RT3052_MP2" = "y" ]; then
	iwpriv ra0 set HiPower=1
    fi
    iwpriv ra0 set HtMimoPs=0
else
# Enable MIMO PowerSave and set LNA gain to default
    if [ "$CONFIG_RALINK_RT3052_MP2" = "y" ]; then
	iwpriv ra0 set HiPower=0
    fi
    iwpriv ra0 set HtMimoPs=1
fi
########################################STAMODE param##########################
if [ "$OperationMode" = "2" ]; then
    if [ "$AutoConnect" = "1" ]; then
	iwpriv ra0 set AutoReconnect=1
    fi
  # in sta mode exit
  exit 0
fi
########################################APMODE param###########################
eval `nvram_buf_get 2860 AutoChannelSelect Channel AP2040Rescan RadioOff \
			    GreenAP HT_OpMode DfsEnable`
#########################################ON/OFF param##########################
if [ "$RadioOff" = "1" ]; then
    iwpriv ra0 set RadioOn=0
    echo ">>>> WIFI DISABLED <<<<"
    exit 0
else
    iwpriv ra0 set RadioOn=1
fi
########################################MULTICAST param########################
if [ "$CONFIG_RT2860V2_AP_IGMP_SNOOP" != "" ]; then
    eval `nvram_buf_get 2860 McastPhyMode McastMcs M2UEnabled`
    if [ "$CONFIG_RT2860V2_MCAST_RATE_SPECIFIC" != "" ]; then
        if [ "$McastPhyMode" != "" ]; then
	    iwpriv ra0 set McastPhyMode=$McastPhyMode
	fi
	if [ "$McastMcs" != "" ]; then
    	    iwpriv ra0 set  McastMcs="$McastMcs"
	fi
    fi
    if [ "$M2UEnabled" != "" ]; then
	iwpriv ra0 set IgmpSnEnable="$M2UEnabled"
    fi
fi
########################################DFS Setup #############################
if [ "$CONFIG_RT2860V2_AP_DFS" != "" ] && [ "$OperationMode" = "1" ]; then
    if [ "$DfsEnable" = "1" ]; then
	iwpriv ra0 set RadarStart=1
    else
	iwpriv ra0 set RadarStop=1
    fi
fi
########################################Channel select#########################
if [ "$AutoChannelSelect" = "1" ]; then
    # rescan and select optimal channel
    # first need scan
    iwpriv ra0 set SiteSurvey=1
    # second select channel
    iwpriv ra0 set AutoChannelSel=1
else
    # set channel manual
    iwpriv ra0 set Channel=$Channel
fi
########################################GREEN mode#############################
if [ "$CONFIG_RT2860V2_AP_GREENAP" != "" ]; then
    if [ "$HT_OpMode" = "1" ] || [ "$GreenAP" = "1" ]; then
	iwpriv ra0 set GreenAP=1
    else
	iwpriv ra0 set GreenAP=0
    fi
fi
#######################################APCLIMODE param ########################
if [ "$OperationMode" = "3" ]; then
    eval `nvram_buf_get 2860 ApCliAutoConnect ApCliClientOnly`
    if [ "$ApCliClientOnly" = "1" ]; then
	echo "APCLI Only client mode enable shutdown ra0..."
	ip link set ra0 down > /dev/null 2>&1
    fi
    if [ "$ApCliAutoConnect" = "1" ]; then
	iwpriv apcli0 set ApCliAutoConnect=1
    fi
fi
###########################################ALWAYS END##########################
if [ "$AP2040Rescan" = "1" ]; then
    iwpriv ra0 set AP2040Rescan=1
fi
