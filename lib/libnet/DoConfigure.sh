#!/bin/bash

echo "=====================CONFIGURE-LIBNET===================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    libtoolize
    aclocal
    automake
fi

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem \
	  --disable-samples --disable-dependency-tracking --enable-shared --disable-static \
	  --with-link-layer=linux  ac_cv_libnet_endianess=lil CFLAGS=-D__linux__"

./configure $CONFOPTS
