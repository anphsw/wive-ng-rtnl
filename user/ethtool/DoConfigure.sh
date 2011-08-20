#!/bin/bash

echo "==================CONFIGURE-ETHTOOL============================"
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    sh ./autogen.sh
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake
fi

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem --disable-dependency-tracking"

./configure $CONFOPTS
