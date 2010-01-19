#!/bin/bash

ROOTDIR=`pwd`
CONFOPTS="--host=mipsel-linux --prefix=$ROOTDIR/filesystem"
./configure $CONFOPTS
