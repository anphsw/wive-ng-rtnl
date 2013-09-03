#!/bin/bash

echo "=====================CONFIGURE-LIBUSB===================="
APROOTDIR=`pwd`
LIBDIR=$FIRMROOT/lib
BACKUPCFLAGS=$CFLAGS
BACKUPLDFLAGS=$LDFLAGS
LIBDIR=$FIRMROOT/lib
NETLINKLIB=$LIBDIR/include_shared

if [ ! -f $APROOTDIR/configure ]; then
    autoreconf
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake
fi

CONFOPTS="--host=mipsel-linux --build=i686-pc-linux-gnu"
CONFOPTS="$CONFOPTS --disable-dependency-tracking --disable-udev --disable-netlink"
CONFOPTS="$CONFOPTS --enable-shared --enable-static"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"

CFLAGS="$BACKUPCFLAGS -I$NETLINKLIB"

export CFLAGS LDFLAGS
./configure $CONFOPTS
