#!/bin/bash

echo "=====================CONFIGURE-TORRENT========================="
APROOTDIR=`pwd`
PREFIX="--prefix=$APROOTDIR/filesystem"
CONFOPTS="--host=mipsel-linux --build=i686-pc-linux-gnu \
	    $PREFIX --disable-gtk --disable-nls --enable-lightweight --disable-utp \
	    LIBCURL_LIBS=/opt/Wive-RTNL/lib/libcurl/filesystem/lib/libcurl.so \
	    LIBEVENT_LIBS=/opt/Wive-RTNL/lib/libevent/filesystem/lib/libevent.so \
	    OPENSSL_LIBS=/opt/Wive-RTNL/user/openssl-0.9.8e/libcrypto.a"
#	    --with-inotify --disable-dependency-tracking"

./configure $CONFOPTS
