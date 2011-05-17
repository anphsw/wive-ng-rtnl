#!/bin/bash

echo "=====================CONFIGURE-LIBCURL===================="
APROOTDIR=`pwd`
LIBDIR=$FIRMROOT/lib
BACKUPCFLAGS=$CFLAGS
BACKUPLDFLAGS=$LDFLAGS
LIBSSL=$LIBDIR/libssl
ZLIB=$LIBDIR/zlib

CONFOPTS="--host=mipsel-linux --build=i686-pc-linux-gnu --without-random --without-ssl --disable-debug --disable-curldebug --disable-dependency-tracking" 
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
CFLAGS="$BACKUPCFLAGS -I$ZLIB -I$LIBSSL" 
LDFLAGS="$BACKUPLDFLAGS -L$ZLIB -L$LIBSSL"

export CFLAGS LDFLAGS
./configure $CONFOPTS
