#!/bin/bash

APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    autoconf
fi

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem"
./configure $CONFOPTS
