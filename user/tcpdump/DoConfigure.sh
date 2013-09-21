#!/bin/bash

echo "==================CONFIGURE-TCPDUMP=============================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    cp -f $APROOTDIR/Makefile.in.tmpl $APROOTDIR/Makefile.in
fi

CONFOPTS="--host=mipsel-linux"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
CONFOPTS="$CONFOPTS --enable-ipv6"
CONFOPTS="$CONFOPTS --disable-smb"
CONFOPTS="$CONFOPTS --without-smi --without-crypto"

./configure $CONFOPTS
