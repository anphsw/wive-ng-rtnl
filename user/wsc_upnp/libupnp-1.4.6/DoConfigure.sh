#!/bin/bash

APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    autoconf
fi

#to stop reconfigure fix me
touch *

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem --disable-dependency-tracking"
./configure $CONFOPTS
