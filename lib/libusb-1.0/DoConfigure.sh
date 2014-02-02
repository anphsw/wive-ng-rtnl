#!/bin/bash

echo "=====================CONFIGURE-LIBUSB===================="
APROOTDIR=`pwd`
LIBDIR=$FIRMROOT/lib
BACKUPCFLAGS=$CFLAGS
BACKUPLDFLAGS=$LDFLAGS
LIBDIR=$FIRMROOT/lib
NETLINKLIB=$LIBDIR/include_shared

if [ ! -f $APROOTDIR/configure ]; then
    autoreconf -fi
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake --add-missing
    automake
fi

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

#arch options
CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"

CONFOPTS="$CONFOPTS --disable-dependency-tracking"
CONFOPTS="$CONFOPTS --enable-shared --enable-static"
CONFOPTS="$CONFOPTS --disable-timerfd --disable-examples-build --disable-tests-build"
CONFOPTS="$CONFOPTS --enable-log --enable-debug-log"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"

CFLAGS="$BACKUPCFLAGS -I$NETLINKLIB"

export CFLAGS LDFLAGS
./configure $CONFOPTS
