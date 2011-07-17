#!/bin/bash

APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    autoconf
fi

CONFOPTS="--host=mipsel-linux"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-ipv6 --disable-bgpd --disable-ripngd --disable-ospfd --disable-ospf6d --disable-dependency-tracking"
./configure $CONFOPTS
