#!/bin/sh

. /sbin/config.sh
. /sbin/global.sh

l2tp_pt=`nvram_get 2860 l2tpPassThru`
ipsec_pt=`nvram_get 2860 ipsecPassThru`
pptp_pt=`nvram_get 2860 pptpPassThru`
