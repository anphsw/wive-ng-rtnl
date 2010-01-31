#!/bin/bash

ROOTDIR=`pwd`
CONFOPTS="--host=mipsel-linux"
CONFOPTS="$CONFOPTS --prefix=$ROOTDIR/filesystem"
CFLAGS="-Os -pipe -mips32 -mtune=mips32 -Wall -Wno-unused -s -fpack-struct -Wpadded  -D__LITTLE_ENDIAN"
CPPFLAGS="-Os"
export $CFLAGS $CPPFLAGS
./configure $CONFOPTS
