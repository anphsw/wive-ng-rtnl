#!/bin/bash

echo "==================CONFIGURE-PPPD==============================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake --add-missing
    automake
fi

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem"

./configure $CONFOPTS
