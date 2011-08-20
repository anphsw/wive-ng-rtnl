#!/bin/bash

echo "==================CONFIGURE-TRANSMISSION======================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    sh ./autogen.sh
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake
fi

./configure --host=mipsel-linux --build=i686-pc-linux-gnu \
	    --prefix=$APROOTDIR/filesystem --disable-gtk \
	    --disable-nls --enable-lightweight --disable-utp \
	    --enable-libnotify \
	    --with-zlib=$FIRMROOT/lib/zlib/ \
	    --with-zlib-includes=$FIRMROOT/lib/zlib/ \
	    LIBCURL_CFLAGS="-I$FIRMROOT/lib/libcurl/include" \
	    LIBCURL_LIBS="-L$FIRMROOT/lib/libcurl/filesystem/lib -lcurl" \
	    LIBEVENT_CFLAGS="-I$FIRMROOT/lib/libevent/include" \
	    LIBEVENT_LIBS="-L$FIRMROOT/lib/libevent/filesystem/lib -levent" \
	    OPENSSL_CFLAGS="-I$FIRMROOT/user/openssl/include" \
	    OPENSSL_LIBS="-L$FIRMROOT/user/openssl -lcrypto -lssl" \
	    --disable-dependency-tracking

