#
# Router rc control scripts Makefile
#
# Copyright 2004, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile,v 1.1.1.1 2007/01/25 12:52:21 jiahao_jhou Exp $
#

-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)
-include $(MODULES_CONFIG)

SHDIR = $(ROOTDIR)/user/shared
HTTPDIR = $(ROOTDIR)/user/asus/httpd
NETDIR = $(ROOTDIR)/user/asus/netconf
NVDIR = $(ROOTDIR)/user/asus/nvram
DISKDIR = $(ROOTDIR)/user/asus/libdisk
MTDDIR = $(ROOTDIR)/user/asus/mtd_utils
INSTALLDIR = $(ROOTDIR)/romfs

CFLAGS += -s -DASUS_NVRAM -DRPPPPOE -DBTN_SETUP -DWL500GP -DDHCP_PPTP -DQOS -DASUS_EXT -DGPQOS \
	-DASUS_DDNS -DWSC -DGUEST_ACCOUNT -DWEB_REDIRECT -DWEBSTRFILTER -DUSB_SUPPORT -DDLM
CFLAGS += -I. -I$(SHDIR)/include -I$(ROOTDIR)/linux-2.6.21.x/include -I$(ROOTDIR)/linux-2.6.21.x/drivers/flash \
	-I$(HTTPDIR)/ -I$(ROOTDIR)/linux-2.6.21.x/drivers/char -I$(ROOTDIR)/linux-2.6.21.x/drivers/net/raeth \
	-I$(MTDDIR) -I$(ROOTDIR)/user/busybox/include -I$(SHDIR) -I$(DISKDIR)
LDFLAGS = -L. -L$(SHDIR)/prebuilt -lcrypto_asus -L$(SHDIR) -lshared -L$(NETDIR) -lnetconf -lm

EXEC = rc

OBJS = rc.o init.o mtd.o services.o watchdog.o network_ex.o manu_ex.o firewall_ex.o common_ex.o \
	network.o services_ex.o	bigpond.o ppp.o	qosutils.o qos.o speedtest.o ntp.o udhcpc.o udhcpc_ex.o \
	ra3052.o ralink.o rtl8366s.o btnsetup.o pspfix.o $(ROOTDIR)/user/wireless_tools/iwlib.o monitor.o \
	usdsvr.o ping.o


all:	$(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(LDFLAGS)
	$(STRIP) $(EXEC)

c.o:
	$(CC) -c $*.c $(CFLAGS)

clean:
	rm -f *.o rc

romfs:
	$(ROMFSINST) /sbin/$(EXEC)
	#cd $(INSTALLDIR)/sbin && ln -sf rc init
	cd $(INSTALLDIR) && rm -f init && ln -sf sbin/rc init
	cd $(INSTALLDIR)/sbin && ln -sf rc init	# disable for tmp
	cd $(INSTALLDIR)/sbin && ln -sf rc erase
	cd $(INSTALLDIR)/sbin && ln -sf rc nvram_restore
	cd $(INSTALLDIR)/sbin && ln -sf rc wphy
	cd $(INSTALLDIR)/sbin && ln -sf rc startWan
	cd $(INSTALLDIR)/sbin && ln -sf rc watchdog
	cd $(INSTALLDIR)/sbin && ln -sf rc write
	cd $(INSTALLDIR)/sbin && ln -sf rc hotplug
	cd $(INSTALLDIR)/sbin && ln -sf rc stopservice
	cd $(INSTALLDIR)/sbin && ln -sf rc ddns_updated
	cd $(INSTALLDIR)/sbin && ln -sf rc start_ddns
	cd $(INSTALLDIR)/sbin && ln -sf rc ntp
	cd $(INSTALLDIR)/sbin && ln -sf rc gpio
	cd $(INSTALLDIR)/sbin && ln -sf rc radioctrl
	cd $(INSTALLDIR)/sbin && ln -sf rc wsrom
	cd $(INSTALLDIR)/sbin && ln -sf rc rsrom
	cd $(INSTALLDIR)/sbin && ln -sf rc wmac
	cd $(INSTALLDIR)/sbin && ln -sf rc ots
	cd $(INSTALLDIR)/sbin && ln -sf rc gbe
	cd $(INSTALLDIR)/sbin && ln -sf rc gbr
	cd $(INSTALLDIR)/sbin && ln -sf rc restore
	cd $(INSTALLDIR)/sbin && ln -sf rc ip-up
	cd $(INSTALLDIR)/sbin && ln -sf rc ip-down
	cd $(INSTALLDIR)/sbin && ln -sf rc wan-up
	cd $(INSTALLDIR)/sbin && ln -sf rc wan-down
	cd $(INSTALLDIR)/sbin && ln -sf rc getCountryCode
	cd $(INSTALLDIR)/sbin && ln -sf rc setCountryCode
	cd $(INSTALLDIR)/sbin && ln -sf rc getMAC
	cd $(INSTALLDIR)/sbin && ln -sf rc setMAC
	cd $(INSTALLDIR)/sbin && ln -sf rc getPIN
	cd $(INSTALLDIR)/sbin && ln -sf rc setPIN
	cd $(INSTALLDIR)/sbin && ln -sf rc getBootV
	cd $(INSTALLDIR)/sbin && ln -sf rc setBootV
	cd $(INSTALLDIR)/sbin && ln -sf rc gen_ralink_config
	cd $(INSTALLDIR)/sbin && ln -sf rc getSSID
	cd $(INSTALLDIR)/sbin && ln -sf rc getChannel
	cd $(INSTALLDIR)/sbin && ln -sf rc asuscfe
	cd $(INSTALLDIR)/sbin && ln -sf rc ateshow
	cd $(INSTALLDIR)/sbin && ln -sf rc atehelp
	cd $(INSTALLDIR)/sbin && ln -sf rc wps_pin
	cd $(INSTALLDIR)/sbin && ln -sf rc wps_pbc
	cd $(INSTALLDIR)/sbin && ln -sf rc wps_oob
	cd $(INSTALLDIR)/sbin && ln -sf rc wps_start
	cd $(INSTALLDIR)/sbin && ln -sf rc wps_stop
	cd $(INSTALLDIR)/sbin && ln -sf rc pspfix
	cd $(INSTALLDIR)/sbin && ln -sf rc getSiteSurvey
	cd $(INSTALLDIR)/sbin && ln -sf rc getWPSAP
	cd $(INSTALLDIR)/sbin && ln -sf rc getBSSID
	cd $(INSTALLDIR)/sbin && ln -sf rc setDisassociate
	cd $(INSTALLDIR)/sbin && ln -sf rc getCurrentAddress
	cd $(INSTALLDIR)/sbin && ln -sf rc getStaConnectionSSID
	cd $(INSTALLDIR)/sbin && ln -sf rc sta_wps_pbc
	cd $(INSTALLDIR)/sbin && ln -sf rc sta_wps_stop
	cd $(INSTALLDIR)/sbin && ln -sf rc getApCliInfo
	cd $(INSTALLDIR)/sbin && ln -sf rc apcli_connect
	cd $(INSTALLDIR)/sbin && ln -sf rc apcli_monitor
	cd $(INSTALLDIR)/sbin && ln -sf rc ping_keep_alive
	cd $(INSTALLDIR)/sbin && ln -sf rc restart_dns
# 2008.10 magic {
	cd $(INSTALLDIR)/sbin && ln -sf rc speedtest
	cd $(INSTALLDIR)/sbin && ln -sf rc restart_qos
	cd $(INSTALLDIR)/sbin && ln -sf rc convert_asus_values
	#cd $(INSTALLDIR)/sbin && ln -sf rc wanduckmain
	cd $(INSTALLDIR)/sbin && ln -sf rc FWRITE
# 2008.10 magic}
	cd $(INSTALLDIR)/sbin && ln -sf rc hotplug
	cd $(INSTALLDIR)/sbin && ln -sf rc hotplug_usb_mass
	cd $(INSTALLDIR)/sbin && ln -sf rc run_apps
	#cd $(INSTALLDIR)/sbin && ln -sf rc run_ftpsamba
	cd $(INSTALLDIR)/sbin && ln -sf rc run_ftp
	cd $(INSTALLDIR)/sbin && ln -sf rc stop_ftp
	cd $(INSTALLDIR)/sbin && ln -sf rc eject_usb1
	cd $(INSTALLDIR)/sbin && ln -sf rc umount2
	cd $(INSTALLDIR)/sbin && ln -sf rc ejusb
	cd $(INSTALLDIR)/sbin && ln -sf rc usbtpt
	cd $(INSTALLDIR)/sbin && ln -sf rc start_telnetd
	cd $(INSTALLDIR)/sbin && ln -sf rc run_upnp
	cd $(INSTALLDIR)/sbin && ln -sf rc start_ots
	cd $(INSTALLDIR)/sbin && ln -sf rc start_ntp
	cd $(INSTALLDIR)/sbin && ln -sf rc get_sw
	cd $(INSTALLDIR)/sbin && ln -sf rc tracktest
	cd $(INSTALLDIR)/sbin && ln -sf rc chkalltask
	cd $(INSTALLDIR)/sbin && ln -sf rc dhcpc_apply_delayed
	cd $(INSTALLDIR)/sbin && ln -sf rc usdsvr_broadcast
	cd $(INSTALLDIR)/sbin && ln -sf rc usdsvr_unicast
	cd $(INSTALLDIR)/sbin && ln -sf rc disktest

