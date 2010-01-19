#!/bin/sh
#
# $Id: lan.sh,v 1.22.6.1 2008-10-02 12:57:42 winfred Exp $
#
# usage: wan.sh
#

. /sbin/global.sh

# stop all
service lld2d stop
service igmpproxy stop
service dnsserver stop
service pppoe-relay stop
service radvd stop
service dhcpd stop
service udpxy stop
service upnp stop

#start all
service lan start
service hostname start
service lld2d start
service igmpproxy start
service dnsserver start
service pppoe-relay start
service radvd start
service stp start
service dhcpd start
service udpxy start
service upnp start
