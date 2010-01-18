#!/bin/sh

. /sbin/config.sh
. /sbin/global.sh

lan_ip=`nvram_get 2860 lan_ipaddr`
stp_en=`nvram_get 2860 stpEnabled`
nat_en=`nvram_get 2860 natEnabled`
bssidnum=`nvram_get 2860 BssidNum`
radio_off=`nvram_get 2860 RadioOff`

if [ "$lan_ip" = "" ]; then
    lan_ip="192.168.1.1"
    nvram_set 2860 lan_ipaddr 192.168.1.1
fi

genSysFiles()
{
        echo "genSysFiles"
        admID=`nvram_get 2860 Login`
        if [ "$admID" = "" ]; then
            admID="Admin"
            nvram_set 2860 Login Admin
        fi
            admPW=`nvram_get 2860 Password`
        if [ "$admPW" = "" ]; then
            admPW="Admin"
            nvram_set 2860 Password Admin
        fi

	login=`nvram_get 2860 Login`
	pass=`nvram_get 2860 Password`
	echo "$login::0:0:Adminstrator:/:/bin/sh" > /etc/passwd
	echo "$login:x:0:$login" > /etc/group
	chpasswd.sh $login $pass
}

genSysFiles

#modprobe lm
#modprobe dwc_otg

#modprobe bridge
#modprobe mii
#modprobe raeth
#ifconfig eth2 0.0.0.0

#ifRaxWdsxDown
#rmmod rt2860v2_ap
#rmmod rt2860v2_sta
ralink_init make_wireless_config rt2860

#modprobe rt2860v2_ap
vpn-passthru.sh

#ifconfig lo 127.0.0.1
#ifconfig br0 down
#brctl delbr br0

#iptables --flush
#iptables --flush -t nat
#iptables --flush -t mangle

echo "dev init done"
