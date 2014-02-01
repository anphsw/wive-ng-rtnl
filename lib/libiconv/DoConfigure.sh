#!/bin/bash

echo "=====================CONFIGURE-LIBICONV===================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    sh ./autogen.sh --skip-gnulib
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake --add-missing
    automake
fi

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

#arch options
CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-debug-mode --disable-dependency-tracking"

#this small workaround
cp -f $APROOTDIR/inc/*.h $APROOTDIR/lib/

./configure $CONFOPTS

echo "=====================CONFIGURE-LIBCHARSET===================="
cd $APROOTDIR/libcharset
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    sh ./autogen.sh --skip-gnulib
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake --add-missing
    automake
fi

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem"

./configure $CONFOPTS

cd ..
