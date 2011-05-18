#!/bin/bash

echo "=====================CONFIGURE-TORRENT========================="
APROOTDIR=`pwd`

#prevent replace
touch *

PREFIX="--prefix=$APROOTDIR/filesystem"
CONFOPTS="--host=mipsel-linux --build=i686-pc-linux-gnu \
	    $PREFIX --disable-gtk --disable-nls --enable-lightweight --disable-utp \
	    --disable-dependency-tracking \
	    LIBCURL_LIBS=/opt/Wive-RTNL/lib/libcurl/filesystem/lib/libcurl.so \
	    LIBEVENT_LIBS=/opt/Wive-RTNL/lib/libevent/filesystem/lib/libevent.so \
	    OPENSSL_LIBS=/opt/Wive-RTNL/user/openssl/libcrypto.so.0.9.8 /opt/Wive-RTNL/user/openssl/libssl.so.0.9.8"
#	    --with-inotify"

./configure $CONFOPTS
