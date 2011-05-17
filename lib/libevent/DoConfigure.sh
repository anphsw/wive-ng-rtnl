#!/bin/bash

APROOTDIR=`pwd`
CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem --disable-debug-mode --disable-dependency-tracking"
./configure $CONFOPTS
