#!/bin/sh
#
# $Id: lan.sh,v 1.22.6.1 2008-10-02 12:57:42 winfred Exp $
#
# usage: wan.sh
#

. /sbin/global.sh

# stop all
service dnsserver stop
service dhcpd stop
service igmpproxy stop
service upnp stop
service udpxy stop
service radvd stop
service lld2d stop
service pppoe-relay stop
service stp stop

#start all
service hostname start
service stp start
service lld2d start
service pppoe-relay start
service radvd start
service udpxy start
service igmpproxy start
service upnp start
service dhcpd start
service dnsserver start
