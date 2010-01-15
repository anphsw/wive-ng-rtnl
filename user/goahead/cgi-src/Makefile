#
# Makefile for the GoAhead web server reference source base
#  for the uClinux OS
#
# Copyright (c) GoAhead Software, Inc. 1995-2000
#
# $Id: Makefile,v 1.15 2008-02-21 08:51:00 yy Exp $
#


CONF	= $(ROOTDIR)/$(LINUXDIR)
CONF_H  = $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h

#
#  As you can see, RRDTool(traffic graphic) is obsoleted.
#
UPLOAD_CGI = upload.cgi 
UPLOAD_BOOTLOADER_CGI = upload_bootloader.cgi
UPLOAD_SETTINGS = upload_settings.cgi

#RRDTOOL_CGIWARPPER = rrdtool_cgiwrapper.cgi
#CPU_MEM_RRD_UPDATER	= cpu_mem_updater
#NETWORK_RRD_UPDATER	= network_updater

ALL_EXE = $(UPLOAD_CGI) $(UPLOAD_BOOTLOADER_CGI) $(UPLOAD_SETTINGS) $(RRDTOOL_CGIWARPPER) $(CPU_MEM_RRD_UPDATER) $(NETWORK_RRD_UPDATER)

all:	$(ALL_EXE)

$(UPLOAD_CGI): upload.cgi.o crc32.o
	$(CC) crc32.o upload.cgi.o -o $@
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(UPLOAD_BOOTLOADER_CGI): upload_bootloader.cgi.o
	$(CC) upload_bootloader.cgi.o -o $@
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(UPLOAD_SETTINGS): upload_settings.cgi.o
	$(CC) upload_settings.cgi.o -o $@
	$(STRIP) --remove-section=.note --remove-section=.comment $@

#$(RRDTOOL_CGIWARPPER): rrdtool_cgiwrapper.o
#	$(CC) -o $@	rrdtool_cgiwrapper.o
#	$(STRIP) --remove-section=.note --remove-section=.comment $@
#
#$(CPU_MEM_RRD_UPDATER): cpu_mem_updater.o
#	$(CC) -o $@	cpu_mem_updater.o
#	$(STRIP) --remove-section=.note --remove-section=.comment $@
#
#$(NETWORK_RRD_UPDATER): network_updater.o
#	$(CC) -o $@	network_updater.o
#	$(STRIP) --remove-section=.note --remove-section=.comment $@
	
romfs:
	$(ROMFSINST) $(ROOT_DIRECTORY)/cgi-bin/$(UPLOAD_CGI)
	$(ROMFSINST) $(ROOT_DIRECTORY)/cgi-bin/$(UPLOAD_BOOTLOADER_CGI)
	$(ROMFSINST) reboot.sh $(ROOT_DIRECTORY)/cgi-bin/reboot.sh
#	$(ROMFSINST) $(RRDTOOL_CGIWARPPER) $(ROOT_DIRECTORY)/cgi-bin/$(RRDTOOL_CGIWARPPER)
	$(ROMFSINST) ExportSettings.sh $(ROOT_DIRECTORY)/cgi-bin/ExportSettings.sh
	$(ROMFSINST) upload_settings.cgi  $(ROOT_DIRECTORY)/cgi-bin/upload_settings.cgi	
	$(ROMFSINST) history.sh  $(ROOT_DIRECTORY)/cgi-bin/history.sh	
	$(ROMFSINST) $(ROOTDIR)/History  $(ROOT_DIRECTORY)/cgi-bin/History

#they are not cgi but cgi-related.
#	$(ROMFSINST) $(CPU_MEM_RRD_UPDATER) $(ROOT_DIRECTORY)/cgi-bin/$(CPU_MEM_RRD_UPDATER)
#	$(ROMFSINST) $(NETWORK_RRD_UPDATER) $(ROOT_DIRECTORY)/cgi-bin/$(NETWORK_RRD_UPDATER)
	
clean:
	rm -f $(ALL_EXE) $(ARCH) *.o

#
#	Dependencies
#
upload.cgi.o: upload.cgi.c $(CONF_H)
	$(CC) -DUPLOAD_FIRMWARE_SUPPORT -I$(CONF) -c upload.cgi.c -o $@

crc32.o: crc32.c $(CONF_H)
	$(CC) -DUSE_HOSTCC -I$(CONF) -c crc32.c -o $@

upload_bootloader.cgi.o: upload.cgi.c $(CONF_H)
	$(CC) -DUPLOAD_BOOTLOADER_SUPPORT -I$(CONF) -c upload.cgi.c -o $@

upload_settings.cgi.o: upload_settings.cgi.c
	$(CC) -c upload_settings.cgi.c -o $@

#rrdtool_cgiwrapper.o: rrdtool_cgiwrapper.c
#	$(CC) -c rrdtool_cgiwrapper.c -o $@
#
#cpu_mem_updater.o: cpu_mem_updater.c
#	$(CC) -c cpu_mem_updater.c -o $@
#
#network_updater.o: network_updater.c
#	$(CC) -c network_updater.c -o $@

