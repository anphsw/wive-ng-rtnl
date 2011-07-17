#!/bin/bash

echo "==================CONFIGURE-RADVD=============================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake
fi

CONFOPTS="--host=mipsel-linux --disable-dependency-tracking"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
export LDFLAGS=-L../../lib/libflex
./configure $CONFOPTS
