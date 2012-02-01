#!/bin/bash

echo "=====================CONFIGURE-LIBNET===================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    libtoolize --force
    aclocal
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake
fi

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem \
	  --disable-samples --disable-dependency-tracking --enable-shared --disable-static \
	  --with-link-layer=linux ac_cv_libnet_endianess=lil CFLAGS=-D__linux__ libnet_cv_have_packet_socket=y"

./configure $CONFOPTS
