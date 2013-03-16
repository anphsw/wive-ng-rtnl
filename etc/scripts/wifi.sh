#!/bin/sh

# include kernel config
. /etc/scripts/config.sh

echo ">>>>> RECONFIGURE WIFI IF = $1 <<<<<<<<<<"

########################################ALLMODE param##########################
eval `nvram_buf_get 2860 HT_MIMOPSMode HiPower AutoConnect OperationMode`
########################################LNA param##############################
# Disable increase LNA gain
if [ "$CONFIG_RALINK_RT3052_MP2" = "y" ]; then
    if [ "$HiPower" = "1" ]; then
	iwpriv "$1" set HiPower=1
    else
	iwpriv "$1" set HiPower=0
    fi
fi
# Enable MIMO PowerSave and set LNA gain to default
if [ "$HT_MIMOPSMode" = "1" ]; then
    iwpriv "$1" set HtMimoPs=1
else
    iwpriv "$1" set HtMimoPs=0
fi
########################################STAMODE param##########################
if [ "$OperationMode" = "2" ]; then
    if [ "$AutoConnect" = "1" ]; then
	iwpriv "$1" set AutoReconnect=1
    fi
  # in sta mode exit
  exit 0
fi
########################################APMODE param###########################
eval `nvram_buf_get 2860 AutoChannelSelect Channel AP2040Rescan RadioOff \
			    GreenAP HT_OpMode DfsEnable`
#########################################ON/OFF param##########################
if [ "$RadioOff" = "1" ]; then
    iwpriv "$1" set RadioOn=0
    echo ">>>> WIFI DISABLED <<<<"
    exit 0
else
    iwpriv "$1" set RadioOn=1
fi
########################################MULTICAST param########################
if [ "$CONFIG_RT2860V2_AP_IGMP_SNOOP" != "" ]; then
    eval `nvram_buf_get 2860 McastPhyMode McastMcs M2UEnabled`
    if [ "$CONFIG_RT2860V2_MCAST_RATE_SPECIFIC" != "" ]; then
        if [ "$McastPhyMode" != "" ]; then
	    iwpriv "$1" set McastPhyMode=$McastPhyMode
	fi
	if [ "$McastMcs" != "" ]; then
    	    iwpriv "$1" set McastMcs="$McastMcs"
	fi
    fi
    if [ "$M2UEnabled" != "" ]; then
	iwpriv "$1" set IgmpSnEnable="$M2UEnabled"
    fi
fi
########################################DFS Setup #############################
if [ "$CONFIG_RT2860V2_AP_DFS" != "" ] && [ "$OperationMode" = "1" ] && [ "$1" != "rai0" ]; then
    if [ "$DfsEnable" = "1" ]; then
	iwpriv "$1" set RadarStart=1
    else
	iwpriv "$1" set RadarStop=1
    fi
fi
########################################Channel select#########################
if [ "$AutoChannelSelect" = "1" ]; then
    # rescan and select optimal channel
    # first need scan
    iwpriv "$1" set SiteSurvey=1
    # second select channel
    iwpriv "$1" set AutoChannelSel=1
else
    if [ "$1" != "rai0" ]; then
	# set channel manual
	iwpriv "$1" set Channel=$Channel
    fi
fi
########################################GREEN mode#############################
if [ "$CONFIG_RT2860V2_AP_GREENAP" != "" ]; then
    if [ "$HT_OpMode" = "1" ] || [ "$GreenAP" = "1" ]; then
	iwpriv "$1" set GreenAP=1
    else
	iwpriv "$1" set GreenAP=0
    fi
fi
###########################################ALWAYS END##########################
if [ "$AP2040Rescan" = "1" ]; then
    iwpriv "$1" set AP2040Rescan=1
fi
