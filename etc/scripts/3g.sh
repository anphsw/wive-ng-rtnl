#!/bin/sh

if [ "$1" != "" ]; then
        dev=$1
else
        dev=`nvram_get 2860 wan_3g_dev`
fi


sleep 10
#change 3G dongle state to modem
if [ "$dev" = "MU-Q101" ]; then
        usb_modeswitch -c /etc_ro/usb/usb_modeswitch_MU-Q101.conf
#elif [ "$dev" = "HUAWEI-ET128" ]; then
#        usb_modeswitch -c /etc_ro/usb/usb_modeswitch_HUAWEI-ET128.conf
elif [ "$dev" = "HUAWEI-E169" ]; then
        usb_modeswitch -c /etc_ro/usb/usb_modeswitch_HUAWEI-E169.conf
elif [ "$dev" = "HUAWEI-E220" ]; then
        usb_modeswitch -c /etc_ro/usb/usb_modeswitch_HUAWEI-E220.conf
elif [ "$dev" = "BandLuxe-C270" ]; then
	sdparm --command=eject /dev/sr0
	sdparm --command=eject /dev/sg0
elif [ "$dev" = "OPTION-ICON225" ]; then
	usb_modeswitch -c /etc_ro/usb/usb_modeswitch_OPTION-ICON225.conf
#else
        #other supported devcies
fi

#wait for state change
sleep 10

#modprobe usbserial module
if [ "$dev" = "MU-Q101" ]; then
        vend_id="0x0408"
        prod_id="0xea02"
#elif [ "$dev" = "HUAWEI-ET128" ]; then
#        vend_id="0x12d1"
#        prod_id="0x1da1"
elif [ "$dev" = "HUAWEI-E169" ]; then
        vend_id="0x12d1"
        prod_id="0x1001"
elif [ "$dev" = "HUAWEI-E220" ]; then
        vend_id="0x12d1"
        prod_id="0x1003"
elif [ "$dev" = "BandLuxe-C270" ]; then
        vend_id="0x1a8d"
        prod_id="0x1009"
#else
        #other supported devcies
fi

if [ "$dev" != "OPTION-ICON225" ]; then
	rmmod usbserial
	rmmod hso
	sleep 3
	modprobe usbserial vendor=$vend_id product=$prod_id
	sleep 3
elif [ "$dev" = "OPTION-ICON225" ]; then
	rmmod usbserial
	rmmod hso
	sleep 3
	modprobe hso
	sleep 3
fi

#create ppp call script for 3G connection
if [ "$dev" = "MU-Q101" ]; then
        modem_f=ttyUSB0
#elif [ "$dev" = "HUAWEI-ET128" ]; then
#        modem_f=ttyUSB0
elif [ "$dev" = "HUAWEI-E169" ]; then
        modem_f=ttyUSB0
elif [ "$dev" = "HUAWEI-E220" ]; then
        modem_f=ttyUSB0
elif [ "$dev" = "BandLuxe-C270" ]; then
        modem_f=ttyUSB0
else
        #other supported devcies
        modem_f=ttyUSB0
fi

if [ "$dev" != "OPTION-ICON225" ]; then
	config-3g-ppp.sh -p pass -u user -m $modem_f -c Generic_conn.scr -d Generic_disconn.scr
	pppd call 3g
elif [ "$dev" = "OPTION-ICON225" ]; then
	echo "APN=internet" > /etc/conninfo.ini
	#echo "PIN=internet >" /etc/conninfo.ini
	#echo "USER="  > /etc/conninfo.ini
	#echo "PASS="  > /etc/conninfo.ini
	hso_connect.sh up
fi

