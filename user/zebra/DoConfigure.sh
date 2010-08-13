#!/bin/bash

APROOTDIR=`pwd`
CONFOPTS="--host=mipsel-linux"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-ipv6 --disable-bgpd --disable-ripngd --disable-ospfd --disable-ospf6d"
./configure $CONFOPTS
