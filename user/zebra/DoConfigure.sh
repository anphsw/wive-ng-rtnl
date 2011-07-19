#!/bin/bash

echo "==================CONFIGURE-ZEBRA=============================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake
fi

CONFOPTS="--host=mipsel-linux"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-ipv6 --disable-bgpd --disable-ripngd --disable-ospfd --disable-ospf6d --disable-dependency-tracking"
./configure $CONFOPTS
