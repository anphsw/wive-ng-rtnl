#!/bin/bash

APROOTDIR=`pwd`
CONFOPTS="--host=mipsel-linux --disable-zlib --disable-openpty"
CONFOPTS="$CONFOPTS --disable-shadow --disable-pam"
CONFOPTS="$CONFOPTS --disable-lastlog --disable-utmp --disable-utmpx"
CONFOPTS="$CONFOPTS --disable-wtmp --disable-wtmpx --disable-loginfunc"
CONFOPTS="$CONFOPTS --disable-pututline --disable-pututxline"
CONFOPTS="$CONFOPTS --disable-largefile --prefix=$APROOTDIR/filesystem"
./configure $CONFOPTS
