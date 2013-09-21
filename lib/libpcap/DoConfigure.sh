#!/bin/bash

echo "=====================CONFIGURE-LIBPCAP===================="
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
    cp -f $APROOTDIR/Makefile.in.tmpl $APROOTDIR/Makefile.in
fi

CONFOPTS="--host=mipsel-linux --build=i686-pc-linux-gnu"
CONFOPTS="$CONFOPTS --enable-shared --enable-ipv6 --with-pcap=linux"
CONFOPTS="$CONFOPTS --disable-bluetooth --disable-canusb --disable-can --without-flex --without-bison --without-libnl"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"

CFLAGS="$BACKUPCFLAGS -I$NETLINKLIB"

export CFLAGS LDFLAGS
./configure $CONFOPTS
