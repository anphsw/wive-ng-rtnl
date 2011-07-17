#!/bin/bash

echo "==================CONFIGURE-LIBUPNP============================"
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake
fi

#to stop reconfigure fix me
touch *

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem --disable-dependency-tracking"
./configure $CONFOPTS
