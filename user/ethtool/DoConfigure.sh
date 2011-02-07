#!/bin/bash

APROOTDIR=`pwd`
CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem --disable-dependency-tracking"
./configure $CONFOPTS
