#!/bin/bash

echo "==================CONFIGURE-LIBUPNP============================"
APROOTDIR=`pwd`

cp -f configure.3052 configure
touch configure.ac

if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake
fi

#to stop reconfigure fix me
touch *

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem --disable-dependency-tracking"

./configure $CONFOPTS
