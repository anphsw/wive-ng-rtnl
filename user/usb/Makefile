all:

clean:

romfs:
	mkdir -p $(ROMFSDIR)/opt
	mkdir -p $(ROMFSDIR)/etc/scripts
#install scripts
	$(ROMFSINST) -S optware-install.sh /etc/scripts/optware-install.sh
	$(ROMFSINST) -S automount.sh /etc/scripts/automount.sh
	$(ROMFSINST) -S storage.sh /etc/scripts/storage.sh
	$(ROMFSINST) -S usbctrl.sh /etc/scripts/usbctrl.sh
#create symlinks
	$(ROMFSINST) -s /etc/scripts/optware-install.sh /bin/optware-install.sh
	$(ROMFSINST) -s /etc/scripts/automount.sh /bin/automount.sh
	$(ROMFSINST) -s /etc/scripts/storage.sh /bin/storage.sh
	$(ROMFSINST) -s /etc/scripts/usbctrl.sh /bin/usbctrl.sh
	$(ROMFSINST) -s /etc/scripts/prnctrl.sh /bin/prnctrl.sh
