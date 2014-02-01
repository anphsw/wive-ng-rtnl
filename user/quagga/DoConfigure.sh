#!/bin/bash

echo "==================CONFIGURE-QUAGGA=============================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake --add-missing
    automake
fi

CONFOPTS="--host=mipsel-linux"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
CONFOPTS="$CONFOPTS --disable-bgpd --disable-babeld --disable-ripngd"
CONFOPTS="$CONFOPTS --disable-ospfapi --disable-ospfclient --disable-ospfd --disable-ospf6d"
CONFOPTS="$CONFOPTS --disable-dependency-tracking"

./configure $CONFOPTS
