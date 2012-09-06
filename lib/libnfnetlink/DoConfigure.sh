#!/bin/bash

echo "=====================CONFIGURE-LIBNFNETLINK===================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    libtoolize --force
    aclocal
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake --foreign
fi

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem \
	  --disable-dependency-tracking --enable-shared --disable-static \
	  CFLAGS=-D__linux__ libnet_cv_have_packet_socket=y"

./configure $CONFOPTS
