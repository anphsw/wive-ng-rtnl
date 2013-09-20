#!/bin/sh

#include kernel config
. /etc/scripts/config.sh

#include global config
. /etc/scripts/global.sh

########################################MESH mode param###########################
if [ "$first_wlan_mesh" != "" ]; then
    case $1 in
	"init")
		meshenabled=`nvram_get 2860 MeshEnabled`
		if [ "$meshenabled" = "1" ]; then
    		    meshhostname=`nvram_get 2860 MeshHostName` 
		    iwpriv $first_wlan_mesh set  MeshHostName="$meshhostname"
		fi
		brctl delif br0 $first_wlan_mesh
		ip link set $first_wlan_mesh down > /dev/null 2>&1
		ip link set $first_wlan_root_if down > /dev/null 2>&1
		service modules gen_wifi_config
		ip link set $first_wlan_root_if up
		meshenabled=`nvram_get 2860 MeshEnabled`
		if [ "$meshenabled" = "1" ]; then
			ip link set $first_wlan_mesh up
			brctl addif br0 $first_wlan_mesh
			meshhostname=`nvram_get 2860 MeshHostName`
			iwpriv $first_wlan_mesh set  MeshHostName="$meshhostname"
		fi
		;;
	"addlink")
		iwpriv $first_wlan_mesh set MeshAddLink="$2"
		echo "iwpriv $first_wlan_mesh set MeshAddLink="$2""
		;;
	"dellink")
		iwpriv $first_wlan_mesh set MeshDelLink="$2"
		echo "iwpriv $first_wlan_mesh set MeshDelLink="$2""
		;;
    esac
fi
