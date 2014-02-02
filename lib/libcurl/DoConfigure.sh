#!/bin/bash

echo "=====================CONFIGURE-LIBCURL===================="
APROOTDIR=`pwd`
LIBDIR=$FIRMROOT/lib
BACKUPCFLAGS=$CFLAGS
BACKUPLDFLAGS=$LDFLAGS
ZLIB=$LIBDIR/include_shared
LIBSSL=$FIRMROOT/user/openssl

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
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

CONFOPTS="$CONFOPTS --without-ssl --disable-debug --disable-curldebug --disable-manual --without-random"
CONFOPTS="$CONFOPTS --disable-dependency-tracking --disable-verbose --disable-rtsp"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
CFLAGS="-Os"
CPPFLAGS="$BACKUPCFLAGS -I$ZLIB -I$LIBSSL"
LDFLAGS="$BACKUPLDFLAGS -L$ZLIB -L$LIBSSL"

export CFLAGS LDFLAGS CPPFLAGS
./configure $CONFOPTS
