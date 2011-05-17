#!/bin/bash

echo "=====================CONFIGURE-LIBEVENT===================="
APROOTDIR=`pwd`
CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem --disable-debug-mode --disable-dependency-tracking"
./configure $CONFOPTS
