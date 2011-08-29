#!/bin/sh

#include kernel config
. /etc/scripts/config.sh

########################################MESH mode param###########################
if [ "$CONFIG_RT2860V2_STA_MESH" != "" ] || [ "$CONFIG_RT2860V2_AP_MESH" != "" ]; then
    case $1 in
	"init")
		meshenabled=`nvram_get 2860 MeshEnabled`
		if [ "$meshenabled" = "1" ]; then
    		    meshhostname=`nvram_get 2860 MeshHostName` 
		    iwpriv mesh0 set  MeshHostName="$meshhostname"
		fi
		brctl delif br0 mesh0
		ip link set mesh0 down > /dev/null 2>&1
		ip link set ra0 down > /dev/null 2>&1
		ralink_init make_wireless_config 2860
		ip link set ra0 up
		meshenabled=`nvram_get 2860 MeshEnabled`
		if [ "$meshenabled" = "1" ]; then
			ip link set mesh0 up
			brctl addif br0 mesh0
			meshhostname=`nvram_get 2860 MeshHostName`
			iwpriv mesh0 set  MeshHostName="$meshhostname"
		fi
		;;
	"addlink")
		iwpriv mesh0 set MeshAddLink="$2"
		echo "iwpriv mesh0 set MeshAddLink="$2""
		;;
	"dellink")
		iwpriv mesh0 set MeshDelLink="$2"
		echo "iwpriv mesh0 set MeshDelLink="$2""
		;;
    esac
fi
