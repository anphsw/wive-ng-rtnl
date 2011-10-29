#
# Makefile for the GoAhead web server reference source base
#  for the uClinux OS
#
# Copyright (c) GoAhead Software, Inc. 1995-2000
#
# $Id: Makefile,v 1.21.2.2 2009-04-17 03:40:21 chhung Exp $
#

CONF	= $(ROOTDIR)/$(LINUXDIR)
CONF_H  = $(CONF)/include/linux/config.h
CRC_L	= $(ROOTDIR)/lib/libnvram/crc32.o

UPLOAD_CGI 		= upload.cgi
UPLOAD_SETTINGS		= upload_settings.cgi

ALL_EXE = $(UPLOAD_CGI) $(UPLOAD_SETTINGS)

all: $(ALL_EXE)

$(UPLOAD_CGI): upload.cgi.o 
	$(CC) $(CFLAGS) $(CRC_L) upload.cgi.o -o $@ $(LDFLAGS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(UPLOAD_SETTINGS): upload_settings.cgi.o
	$(CC) $(CFLAGS) upload_settings.cgi.o -o $@ $(LDFLAGS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

romfs:
	$(ROMFSINST) $(ROOT_DIRECTORY)/cgi-bin/$(UPLOAD_CGI)
	$(ROMFSINST) reboot.sh $(ROOT_DIRECTORY)/cgi-bin/reboot.sh
	$(ROMFSINST) ExportSettings.sh $(ROOT_DIRECTORY)/cgi-bin/ExportSettings.sh
	$(ROMFSINST) upload_settings.cgi  $(ROOT_DIRECTORY)/cgi-bin/upload_settings.cgi
	$(ROMFSINST) history.sh  $(ROOT_DIRECTORY)/cgi-bin/history.sh
	$(ROMFSINST) $(ROOTDIR)/History  $(ROOT_DIRECTORY)/cgi-bin/History

#They are not cgi but cgi-related.
clean:
	rm -f $(ALL_EXE) $(ARCH) *.o

#Dependencies
upload.cgi.o: upload.cgi.c $(CONF_H)
	$(CC) $(CFLAGS) -DUPLOAD_FIRMWARE_SUPPORT -I$(CONF) -c upload.cgi.c -o $@ $(LDFLAGS)

upload_settings.cgi.o: upload_settings.cgi.c
	$(CC) $(CFLAGS) -c upload_settings.cgi.c -o $@ $(LDFLAGS)

