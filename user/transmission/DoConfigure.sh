#!/bin/bash

echo "=====================CONFIGURE-TORRENT========================="
APROOTDIR=`pwd`
ROOTDIR="/opt/Wive-RTNL"
INCLUDES="-I$ROOTDIR/lib/libevent/include -I$ROOTDIR/lib/libevent/include/event2 -I$ROOTDIR/user/openssl/include -I$ROOTDIR/lib/libcurl/include"

#prevent replace
touch *

PREFIX="--prefix=$APROOTDIR/filesystem"
CONFOPTS="--host=mipsel-linux --build=i686-pc-linux-gnu \
	    $PREFIX --disable-gtk --disable-nls --enable-lightweight --disable-utp \
	    --disable-dependency-tracking --with-inotify \
	    LIBCURL_LIBS=$ROOTDIR/lib/libcurl/filesystem/lib/libcurl.so \
	    LIBEVENT_LIBS=$ROOTDIR/lib/libevent/filesystem/lib/libevent.so \
	    OPENSSL_LIBS=$ROOTDIR/user/openssl/libcrypto.so.0.9.8 $ROOTDIR/user/openssl/libssl.so.0.9.8"

export CFLAGS="$CFLAGS $INCLUDES"
./configure $CONFOPTS
