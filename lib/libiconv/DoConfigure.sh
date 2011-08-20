#!/bin/bash

echo "=====================CONFIGURE-LIBICONV===================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    sh ./autogen.sh
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake
fi

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem --disable-debug-mode --disable-dependency-tracking"

this small workaround
cp -f ./inc/*.h ./lib/

./configure $CONFOPTS
