#!/bin/bash

APROOTDIR=`pwd`
CONFOPTS="--host=mipsel-linux --disable-dependency-tracking"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
export LDFLAGS=-L../../lib/libflex
./configure $CONFOPTS
