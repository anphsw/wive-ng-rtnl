#!/bin/bash

ROOTDIR=`pwd`
CONFOPTS="--host=mipsel-linux"
CONFOPTS="$CONFOPTS --prefix=$ROOTDIR/filesystem"
CFLAGS="-Os"
CPPFLAGS="-Os"
export $CFLAGS $CPPFLAGS
./configure $CONFOPTS
