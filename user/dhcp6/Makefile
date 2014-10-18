####################################################
#	workaround for build wide dhcp		   #
#	this fedore patched version 2014-01-16     #
####################################################

all:
	@set -e;
	cd wide-dhcpv6 && make -j1

clean:
	@set -e;
	cd wide-dhcpv6 && make clean &&	make distclean

romfs:
	@set -e;
	cd wide-dhcpv6 && make romfs
