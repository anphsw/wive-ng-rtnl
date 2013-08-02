#!/bin/bash

echo "=====================CONFIGURE-LIBNL========================"
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    libtoolize --force
    aclocal
    autoconf
fi

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem \
	  CFLAGS=-D__linux__ libnet_cv_have_packet_socket=y"

./configure $CONFOPTS
