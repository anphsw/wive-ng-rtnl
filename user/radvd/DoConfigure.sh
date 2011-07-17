#!/bin/bash

APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    autoconf
fi

CONFOPTS="--host=mipsel-linux --disable-dependency-tracking"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
export LDFLAGS=-L../../lib/libflex
./configure $CONFOPTS
