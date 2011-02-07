#!/bin/bash

APROOTDIR=`pwd`

#to stop reconfigure fix me
touch *

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem --disable-dependency-tracking"
./configure $CONFOPTS
