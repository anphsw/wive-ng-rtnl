#!/bin/bash

APROOTDIR=`pwd`
CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem"
./configure $CONFOPTS
