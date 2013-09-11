#!/bin/bash

echo "==================CONFIGURE-QUAGGA=============================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake
fi

CONFOPTS="--host=mipsel-linux"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-ipv6"
CONFOPTS="$CONFOPTS --disable-bgpd --disable-babeld --disable-ripngd "
CONFOPTS="$CONFOPTS --disable-ospfapi --disable-ospfclient--disable-ospf6d --disable-ospfd"
CONFOPTS="$CONFOPTS --disable-dependency-tracking"

./configure $CONFOPTS
