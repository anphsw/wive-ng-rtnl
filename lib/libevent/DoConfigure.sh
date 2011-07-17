#!/bin/bash

echo "=====================CONFIGURE-LIBEVENT===================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake
fi

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem --disable-debug-mode --disable-dependency-tracking"
./configure $CONFOPTS
