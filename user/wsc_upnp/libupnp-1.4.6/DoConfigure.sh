#!/bin/bash

ROOTDIR=`pwd`
CONFOPTS="--host=mipsel-linux"
CONFOPTS="$CONFOPTS --prefix=$ROOTDIR/filesystem"
CFLAGS="-Os -mips32 -mtune=mips32"
CPPFLAGS="-Os"
export $CFLAGS $CPPFLAGS
./configure $CONFOPTS
