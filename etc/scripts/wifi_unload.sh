#!/bin/sh

# Description: Megakill script..
# Try some methods for free ram before firmware update.
# Workaround over workaround. Need clean in the future.

# include global
. /etc/scripts/global.sh

stop_serv="transmission vpnhelper inetd shaper crontab pppoe-relay ddns wscd dhcpd lld2d radvd syslog \
	    zebra udpxy upnp hotplug igmp_proxy ntp samba dnsserver snmpd"

kill_apps="transmission-daemon smbd nmbd pppd xl2tpd udhcpd udhcpc crond lld2d igmpproxy inetd syslogd klogd \
	    ntpclient ntpd zebra ripd inadyn ftpd scp \
	    iwevent telnetd wscd rt2860apd rt61apd dnsmasq cdp-send snmpd"

rmmod_mod="hw_nat ppp_mppe pppol2tp pptp pppoe pppox ppp_generic imq ipt_TTL ipt_IMQ ipt_tos \
	    ipt_REDIRECT ipt_ttl ipt_TOS xt_string xt_webstr xt_connmark xt_CONNMARK xt_conntrack \
	    act_gact act_police cls_tcindex em_cmp em_u32 sch_gred sch_red act_ipt cls_fw cls_u32 \
	    em_nbyte sch_esfq sch_htb sch_sfq ts_fsm ts_kmp ts_bm \
	    usblp usb-storage usbserial hso ext2 ext3 cifs"

# disable forward
sysctl -w net.ipv4.ip_forward=0
echo 0 > /proc/sys/net/ipv4/conf/all/mc_forwarding
echo 0 > /proc/sys/net/ipv4/conf/default/mc_forwarding

# clear conntrack tables
echo 1 > /proc/sys/net/nf_conntrack_flush

# disable hotplug
if [ -f /proc/sys/kernel/hotplug ]; then
    echo > /proc/sys/kernel/hotplug
fi

# clear route cache
ip route flush cache

unload_ra0()
{
    ip link set ra0 down > /dev/null 2>&1
    ip link set eth2.2 down > /dev/null 2>&1
}

unload_ra0br0()
{
    br0_mac=`ifconfig br0 | sed -n '/HWaddr/p' | sed -e 's/.*HWaddr \(.*\)/\1/'`
    br0_ip=`ifconfig br0 | sed -n '/inet addr:/p' | sed -e 's/ *inet addr:\(.*\)  Bcast.*/\1/'`
    br0_netmask=`ifconfig br0 | sed -n '/inet addr:/p' | sed -e 's/.*Mask:\(.*\)/\1/'`
    ra0_mac=`ifconfig ra0 | sed -n '/HWaddr/p' | sed -e 's/.*HWaddr\ \(.*\)/\1/'`

    if [ "$ra0_mac" = "$br0_mac" ]; then
	# destory br0
	ip link set br0 down > /dev/null 2>&1
	brctl delbr br0 > /dev/null 2>&1

	# disable WAN and WLAN
	unload_ra0

	if [ "$1" != "" ]; then
	    # mirror br0 to eth2x
	    ip link set "$1" down > /dev/null 2>&1
	    ifconfig "$1" hw ether $br0_mac
	    ifconfig "$1" $br0_ip netmask $br0_netmask
	    ip link set "$1" up
	fi
    else
	unload_ra0
    fi
}

unload_modules()
{
    echo "Unload modules"
    # unload modules all unused
    rmmod -a
    # all others
    for mod in $rmmod_mod
    do
        rmmod $mod > /dev/null 2>&1
    done
    # unload wifi modules
    service modules stop
    rmmod -a
}

unload_apps()
{
    echo "Stop services..." # first step stop services
    for serv in $stop_serv
    do
	service $serv stop > /dev/null 2>&1
    done
    echo "Wait 3 seconds."
    sleep 3
    echo "Kill aplications..." # second step terminate and kill application
    for apps in $kill_apps
    do
	(killall -q $apps && usleep 20000 && killall -q -SIGKILL $apps) > /dev/null 2>&1
    done
}

unload_apps

is_ra0_in_br0=`brctl show | sed -n '/ra0/p'`
is_eth21_in_br0=`brctl show | sed -n '/eth2\.1/p'`

# unload wifi driver
if [ "$is_ra0_in_br0" == "" ]; then
    unload_ra0
elif [ "$is_eth21_in_br0" != "" ]; then
    unload_ra0br0 eth2.1
fi

if [ -f /bin/swapoff ]; then
    echo "Disable swaps."
    swapoff -a
fi

mounted=`mount | grep "/dev/sd" | cut -f1 -d" " | cut -f3 -d "/"`
if [ -n "$mounted" ]; then
	for disk in $mounted; do
	    echo "Umount external drive /dev/$disk."
	    (sync && umount -l /dev/$disk ) &
	done
    sleep 2
fi

# unload all modules this is need after unmont
unload_modules

# This drop unneded caches to free more ram.
sysctl -w vm.min_free_kbytes=3192
drop_disk_caches
sysctl -w vm.min_free_kbytes=1024
