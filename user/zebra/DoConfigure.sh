#!/bin/bash

ROOTDIR=`pwd`
CONFOPTS="--host=mipsel-linux"
CONFOPTS="$CONFOPTS --prefix=$ROOTDIR/filesystem"
./configure $CONFOPTS
