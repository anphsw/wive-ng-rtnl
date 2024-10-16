#!/bin/sh

# Description: Megakill script..
# Try some methods for free ram before firmware update.
# Workaround over workaround. Need clean in the future.
# In 16Mb devices also reconfigure network and unload wifi modules for memsave.

# include global
. /etc/scripts/global.sh

stop_serv="watchdog radvd vpnhelper shaper crontab transmission pppoe-relay ddns wscd lld2d zebra udpxy upnp \
	    igmp_proxy ntp dnsserver snmpd xupnp syslog inetd samba hotplug dhcpd"

kill_apps="transmission-daemon smbd nmbd pppd xl2tpd udhcpd udhcpc crond lld2d igmpproxy inetd syslogd klogd \
	    ntpclient ntpd zebra ripd inadyn ftpd scp miniupnpd iwevent telnetd wscd rt2860apd rt61apd dnsmasq cdp-send snmpd xupnpd"

rmmod_mod="hw_nat ppp_mppe pppol2tp pptp pppoe pppox ppp_generic imq ipt_account ipt_TTL ipt_IMQ ipt_tos \
	    ipt_REDIRECT ipt_ttl ipt_TOS xt_string xt_webstr xt_connmark xt_CONNMARK xt_conntrack \
	    act_gact act_police cls_tcindex em_cmp em_u32 sch_gred sch_red act_ipt cls_fw cls_u32 \
	    nf_nat_ftp nf_nat_h323 nf_nat_pptp nf_nat_proto_gre nf_nat_sip nf_nat_rtsp \
	    nf_conntrack_ftp nf_conntrack_proto_gre nf_conntrack_h323 nf_conntrack_sip nf_conntrack_pptp nf_conntrack_rtsp \
	    em_nbyte sch_esfq sch_htb sch_sfq ts_fsm ts_kmp ts_bm \
	    usblp usb-storage usbserial hso ext2 ext3 cifs"

unload_modules() {
    echo "Unload modules"
    # unload modules all unused
    rmmod -a
    # all others
    for mod in $rmmod_mod
    do
        rmmod $mod > /dev/null 2>&1
    done

    # unload full
    rmmod_mod=`lsmod | awk {' print $1'}`
    for mod in $rmmod_mod
    do
	if [ "$mod" = "rt2860v2_ap" -o "$mod" = "rt2860v2_sta" ]; then
	    # skip wifi modules
	    mod=
	fi
	if [ "$mod" != "" ]; then
    	    rmmod $mod > /dev/null 2>&1
	fi
    done
    rmmod -a
}

unload_apps() {
    echo "Stop services..." # first step stop services
    for serv in $stop_serv
    do
	service $serv stop > /dev/null 2>&1
    done
    echo "Kill aplications..." # second step terminate and kill application
    for apps in $kill_apps
    do
	(killall -q $apps && usleep 20000 && killall -q -SIGKILL $apps) > /dev/null 2>&1
    done
    # remove web pages from tmpfs and link to rootfs
    if [ -d /tmp/web ]; then
	echo "Remove web pages from tmpfs before firmware burn to flash"
	rm -rf /tmp/web
	ln -sf /web /tmp/web
    fi
}

free_mem_cahce() {
    # small workaround for defrag ane clean mem
    sysctl -wq vm.min_free_kbytes=2048
    sync
    sysctl -wq vm.min_free_kbytes=1024
}

# unload all applications
unload_apps

# disable hotplug
if [ -f /proc/sys/kernel/hotplug ]; then
    echo > /proc/sys/kernel/hotplug
fi

# umount all particions and disable swap
if [ -f /etc/scripts/umount_all.sh ]; then
    /etc/scripts/umount_all.sh
fi

# unload all modules this is need after unmont
unload_modules

# disable forward
sysctl -wq net.ipv4.ip_forward=0
sysctl -wq net.ipv4.conf.all.forwarding=0
sysctl -wq net.ipv4.conf.all.mc_forwarding=0
sysctl -wq net.ipv4.conf.default.forwarding=0
sysctl -wq net.ipv4.conf.default.mc_forwarding=0
sysctl -wq net.ipv6.conf.all.forwarding=0
sysctl -wq net.ipv6.conf.default.forwarding=0

# clear conntrack and routes tables/caches
flush_net_caches

# This drop unneded caches to free more ram.
free_mem_cahce
