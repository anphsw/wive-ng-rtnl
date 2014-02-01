#!/bin/bash

echo "=====================CONFIGURE-LIBUPNP===================="
APROOTDIR=`pwd`
LIBDIR=$FIRMROOT/lib
BACKUPCFLAGS=$CFLAGS
BACKUPLDFLAGS=$LDFLAGS
ZLIB=$LIBDIR/include_shared
LIBSSL=$FIRMROOT/user/openssl

if [ ! -f $APROOTDIR/configure ]; then
    autoreconf
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake --add-missing
    automake
fi

CONFOPTS="--host=mipsel-linux --build=i686-pc-linux-gnu"
CONFOPTS="$CONFOPTS --disable-dependency-tracking"
CONFOPTS="$CONFOPTS --disable-samples --disable-debug --without-documentation"
CONFOPTS="$CONFOPTS --enable-shared --enable-static"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
CFLAGS="$BACKUPCFLAGS -I$ZLIB -I$LIBSSL"
LDFLAGS="$BACKUPLDFLAGS -L$ZLIB -L$LIBSSL"

export CFLAGS LDFLAGS
./configure $CONFOPTS
