#!/bin/sh

# include kernel config
. /etc/scripts/config.sh

echo ">>>>> RECONFIGURE WIFI IF = $1 <<<<<<<<<<"

#####################WORKAROUND FOR TX RING FULL IN WIFI DRIVERS#############################
iwpriv $1 set AutoFallBack=1
iwpriv $1 set LongRetry=3
iwpriv $1 set ShortRetry=5
#############################################################################################
eval `nvram_buf_get 2860 HiPower AutoConnect OperationMode TxPower`
########################################LNA param############################################
# Disable increase LNA gain
if [ "$CONFIG_RALINK_RT3052" = "y" ] || [ "$CONFIG_RALINK_RT3352" = "y" ]; then
    if [ "$HiPower" != "" ]; then
	iwpriv "$1" set HiPower="$HiPower"
    fi
fi
# Recalibrate txpower after HiPower set
if [ "$TxPower" != "" ]; then
    iwpriv "$1" set TxPower="$TxPower"
fi
########################################STAMODE param########################################
if [ "$OperationMode" = "2" ]; then
    if [ "$AutoConnect" != "" ]; then
	iwpriv "$1" set AutoReconnect="$AutoConnect"
    fi
  # in sta mode exit
  exit 0
fi
########################################APMODE param#########################################
eval `nvram_buf_get 2860 AutoChannelSelect RadioOff GreenAP HT_OpMode`
#########################################ON/OFF param########################################
if [ "$RadioOff" = "1" ]; then
    iwpriv "$1" set RadioOn=0
    echo ">>>> WIFI DISABLED <<<<"
    exit 0
else
    iwpriv "$1" set RadioOn=1
fi
########################################MULTICAST param######################################
if [ "$CONFIG_RT2860V2_AP_IGMP_SNOOP" != "" ]; then
    eval `nvram_buf_get 2860 McastPhyMode McastMcs M2UEnabled igmpEnabled ApCliBridgeOnly`
    if [ "$M2UEnabled" = "1" ] && \
	[ "$igmpEnabled" = "1" -o "$OperationMode" = "0" -o "$ApCliBridgeOnly" = "1" ]; then
	iwpriv "$1" set IgmpSnEnable=1
    else
	iwpriv "$1" set IgmpSnEnable=0
	if [ "$McastPhyMode" != "" ]; then
	    iwpriv "$1" set McastPhyMode="$McastPhyMode"
	fi
	if [ "$McastMcs" != "" ]; then
    	    iwpriv "$1" set McastMcs="$McastMcs"
	fi
    fi
fi
########################################GREEN mode###########################################
if [ "$CONFIG_RT2860V2_AP_GREENAP" != "" ]; then
    if [ "$HT_OpMode" = "1" ] || [ "$GreenAP" = "1" ]; then
	iwpriv "$1" set GreenAP=1
    else
	iwpriv "$1" set GreenAP=0
    fi
fi
########################################Channel select#######################################
if [ "$AutoChannelSelect" = "1" ]; then
    # rescan and select optimal channel
    # first need scan
    iwpriv "$1" set SiteSurvey=1
    # second select channel
    iwpriv "$1" set AutoChannelSel=1
fi
###########################################ALWAYS END########################################
# rescan coexist mode
if [ "$CONFIG_RT2860V2_AP_80211N_DRAFT3" != "" ]; then
    eval `nvram_buf_get 2860 AP2040Rescan HT_BSSCoexistence`
    if [ "$AP2040Rescan" = "1" ] && [ "$HT_BSSCoexistence" = "1" ]; then
	iwpriv "$1" set AP2040Rescan=1
    fi
fi
