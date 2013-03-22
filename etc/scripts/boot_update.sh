#!/bin/sh

###################################################################################
# Boot auto update wive script only RT3052 with NOR and >= 32Mb 32bit RAM support #
###################################################################################

url="http://wive-ng.sf.net/downloads/ralink/boots/"

# get cpu type and flash type
syst=`cat /proc/cpuinfo | grep "system type" | awk {' print $4 '}`
flst=`cat /proc/cpuinfo | grep "flash type" | awk {' print $4 '}`

# check system
if [ "$syst" = "RT3052" ] && [ "$flst" = "NOR" ]; then
    boot_file="uboot-3052-nor.bz2"
else
    echo "You system not supported."
    echo "Only RT305* with 32bit 32Mb/64Mb mem support"
    exit 1
fi

# check need usb
if [ -d /proc/bus/usb ]; then
    usb="usb-"
else
    usb=""
fi

full_url="$url$usb$boot_file"

# download and unpack
wget "$full_url" -P /tmp -O /tmp/boot.bin.bz2
cd /tmp
bzip2 -d boot.bin.bz2

# if unpack correct - flash
if [ -f /tmp/boot.bin ]; then
    echo "OK. Start flash $usb$boot_file"
    mtd_write -r write /tmp/boot.bin Bootloader
else
    echo "Downloaded files is incorrect."
fi
