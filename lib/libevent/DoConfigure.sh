#!/bin/bash

echo "=====================CONFIGURE-LIBEVENT===================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    autoconf
fi

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem --disable-debug-mode --disable-dependency-tracking"
./configure $CONFOPTS
