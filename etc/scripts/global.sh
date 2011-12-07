#!/bin/sh

############################################################
# global.sh - correct enviroment helper for automatization #
############################################################

# include kernel config
. /etc/scripts/config.sh

# include profile variables
. /etc/profile

# set default variables
wan_if="eth2.2"
real_wan_if="eth2.2"
lan_if="br0"
lan2_if="br0:9"
vpn_if="ppp0"

# set some constatns
mcast_net="224.0.0.0/4"
upmpm_net="239.0.0.0/8"

# first get operation mode and wan mode  dns mode and relay mode vpn mode and type
eval `nvram_buf_get 2860 OperationMode wanConnectionMode dnsPEnabled wan_ipaddr wan_static_dns \
	vpnEnabled vpnPurePPPOE vpnType	IPv6_Enable QoSEnable simple_qos`

# get wireless, wan and lan mac adresses
getMacIf()
{
    eval `nvram_buf_get 2860 WLAN_MAC_ADDR WAN_MAC_ADDR LAN_MAC_ADDR`
}

# LAN interface name -> $lan_if
getLanIfName()
{
    if [ "$OperationMode" = "2" ]; then
	lan_if="eth2"
	lan2_if="eth2:9"
    else
	lan_if="br0"
	lan2_if="br0:9"
    fi
}

# WAN interface name -> $wan_if
getWanIfName()
{
    # real wan name
    if [ "$OperationMode" = "0" ]; then
	wan_if="br0"
    elif [ "$OperationMode" = "1" ] || [ "$OperationMode" = "4" ]; then
	wan_if="eth2.2"
    elif [ "$OperationMode" = "2" ]; then
	wan_if="ra0"
    elif [ "$OperationMode" = "3" ]; then
	if [ "$CONFIG_RT2860V2_AP_APCLI" != "" ]; then
	    wan_if="apcli0"
	else
	    echo "Driver not support APCLI mode."
	    wan_if="eth2.2"
	fi
    elif [ "$OperationMode" = "4" ]; then
	    wan_if="eth2.2"
    fi

    # upnp wan name
    if [ "$vpnEnabled" = "on" ]; then
	get_wan_if=`ls /proc/sys/net/ipv4/conf/ | grep ppp | tail -q -n1`
	if [ "$get_ppp_wan_if" != "" ]; then
    	    real_wan_if="$get_ppp_wan_if"
	else
    	    real_wan_if="$vpn_if"
	fi
    else
        real_wan_if=$wan_if
    fi
}

getWanIpaddr()
{
    # always return physical wan ip
    if [ "$wanConnectionMode" != "STATIC" ] || [ "$wan_ipaddr" = "" ]; then
	wan_ipaddr=`ip -4 addr show dev $wan_if | awk '/inet / {print $2}' | cut -f1 -d"/"` > /dev/null 2>&1
    fi

    # return vpn or physical wan ip
    real_wan_ipaddr=`ip -4 addr show dev $real_wan_if | awk '/inet / {print $2}' | cut -f1 -d"/"` > /dev/null 2>&1
    if [ "$real_wan_ipaddr" = "" ]; then
	real_wan_ipaddr=$wan_ipaddr
    fi

}

get_txqlen()
{
    if [ "$QoSEnable" != "" -a "$QoSEnable" != "0" ] || [ "$simple_qos" = "1" ]; then
	# QoS Enabled
	# For memory save 16m device uses txqueuelen=100
	if [ -f /tmp/is_16ram_dev ]; then
	    txqueuelen="100"
	else
	    txqueuelen="1000"
	fi
    else
	# QoS Disabled
	# For memory save 16m device uses txqueuelen=50
	if [ -f /tmp/is_16ram_dev ]; then
	    txqueuelen="50"
	else
	    txqueuelen="500"
	fi
    fi
}

# wait connect to AP
wait_connect()
{
    if [ "$OperationMode" = "2" ]; then
	connected=`iwpriv ra0 connStatus | grep Connected -c`
	if [ "$connected" = "0" ] || [ ! -f /tmp/sta_connected ]; then
	    staCur_SSID=""
    	    exit 0
	fi
	staCur_SSID=`iwpriv ra0 connStatus | grep ra0 | awk ' { print $3 } ' | cut -f1 -d[`
    fi
}

# configure and start dhcp client
udhcpc_opts()
{
    CL_SLEEP=1
    if [ "$OperationMode" = "0" ] || [ "$OperationMode" = "2" ]; then
	CL_SLEEP=5
	# disable dhcp renew from driver
	sysctl -w net.ipv4.send_sigusr_dhcpc=9
    else
	eval `nvram_buf_get 2860 ForceRenewDHCP wan_port`
	if [ "$ForceRenewDHCP" != "0" ] && [ "$wan_port" != "" ]; then
	    # configure event wait port
	    sysctl -w net.ipv4.send_sigusr_dhcpc=$wan_port
	else
	    # disable dhcp renew from driver
	    sysctl -w net.ipv4.send_sigusr_dhcpc=9
	fi

    fi
    eval `nvram_buf_get 2860 dhcpRequestIP wan_manual_mtu HostName`
    if [ "$dhcpRequestIP" != "" ]; then
	dhcpRequestIP="-r $dhcpRequestIP"
    else
	dhcpRequestIP=""
    fi
    if [ "$wan_manual_mtu" = "0" ]; then
	wan_manual_mtu="-O mtu"
    else
	wan_manual_mtu=""
    fi
    UDHCPCOPTS="-i $wan_if $dhcpRequestIP -S -R -T 5 -a \
		-s /bin/udhcpc.sh -p /var/run/udhcpc.pid \
		-O routes -O staticroutes -O msstaticroutes \
		$wan_manual_mtu -x hostname:$HostName -f &"
}

# configure and start zeroconf daemon
zero_conf()
{
    wan_is_not_null=`ip -4 addr show $wan_if | grep inet -c`
    if [ "$wan_is_not_null" = "0" ]; then
	killall -q zcip
	killall -q -SIGKILL zcip
	zcip $wan_if /etc/scripts/zcip.script > /dev/null 2>&1
	sleep 10
    fi
}

# conntrack and route table/caches flush
flush_net_caches()
{
    ip route flush cache > /dev/null 2>&1
    echo 1 > /proc/sys/net/nf_conntrack_table_flush
}

# L2TP and PPTP kernel dead-loop fix
vpn_deadloop_fix()
{
    if [ "$vpnEnabled" = "on" ]; then
	if [ "$vpnType" != "0" ] || [ "$OperationMode" = "2" ]; then
	    # First vpn stop...
	    # Auto start later renew/bound
	    service vpnhelper stop > /dev/null 2>&1
	    flush_net_caches
	fi
    fi
}

killall_vpn()
{
    # correct terminate xl2tpd daemon
    # first disconnect
    if [ -f /var/run/xl2tpd/l2tp-control ] && [ ! -f /etc/ppp/l2tpd.conf ]; then
	lac=`cat /etc/ppp/l2tpd.conf | grep lns | cut -f 2- -d =`
	if [ "$lac" != "" ]; then
	    echo "d $lac" > /var/run/xl2tpd/l2tp-control
	    sleep 2
	fi
    fi
    # second terminate
    if [ -f /var/run/xl2tpd/l2tpd.pid ]; then
	pid=`cat /var/run/xl2tpd/l2tpd.pid`
	if [ "$pid" != "" ]; then
	    #Kill daemons
	    kill $pid > /dev/null 2>&1
	    sleep 2
	    kill -SIGKILL $pid > /dev/null 2>&1
	fi
    fi

    # first send HUP for terminate connections and try some times
    # second send TERM for exit pppd process
    # if process not terminated send KILL
    # vpn client always use $vpn_if
    if [ -f /var/run/$vpn_if.pid ]; then
	pid=`cat /var/run/$vpn_if.pid`
	if [ "$pid" != "" ]; then
	    # close connection
	    kill -SIGHUP $pid
	fi
	# terminate pppd
	count=0
	while kill $pid > /dev/null 2>&1; do
	    if [ "$count" = "2" ]; then
		kill -SIGKILL $pid  > /dev/null 2>&1
		count=0
		sleep 2
	    fi
	    count="$(($count+1))"
	    sleep 2
	done
	rm -f /var/run/$vpn_if.pid
    fi
    # Remove VPN server IP file
    rm -f /tmp/vpnip
}

# get params
getLanIfName
getWanIfName
getWanIpaddr
