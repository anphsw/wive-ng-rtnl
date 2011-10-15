#!/bin/bash

echo "=====================CONFIGURE-LIBUPNP===================="
APROOTDIR=`pwd`
LIBDIR=$FIRMROOT/lib
BACKUPCFLAGS=$CFLAGS
BACKUPLDFLAGS=$LDFLAGS
ZLIB=$LIBDIR/zlib
LIBSSL=$FIRMROOT/user/openssl

if [ ! -f $APROOTDIR/configure ]; then
    #workaround for aclocal issue
    cp -fv configure.3052 configure
    cp -fv configure.ac.3052 configure.ac
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake
fi

CONFOPTS="--host=mipsel-linux --build=i686-pc-linux-gnu"
CONFOPTS="$CONFOPTS --disable-blocking_tcp_connection --disable-samples --disable-debug"
CONFOPTS="$CONFOPTS --disable-dependency-tracking --enable-shared --enable-static"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
CFLAGS="$BACKUPCFLAGS -I$ZLIB -I$LIBSSL" 
LDFLAGS="$BACKUPLDFLAGS -L$ZLIB -L$LIBSSL"

export CFLAGS LDFLAGS
./configure $CONFOPTS
