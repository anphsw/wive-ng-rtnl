#!/bin/bash

echo "==================CONFIGURE-RADVD=============================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake
fi

CONFOPTS="--host=mipsel-linux"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-dependency-tracking"

export LDFLAGS=-L../../lib/libflex

./configure $CONFOPTS
