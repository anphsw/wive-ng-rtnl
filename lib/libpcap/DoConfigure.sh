#!/bin/bash

echo "=====================CONFIGURE-LIBPCAP===================="
APROOTDIR=`pwd`
CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem --without-flex --without-bison --with-pcap=linux"
./configure $CONFOPTS
