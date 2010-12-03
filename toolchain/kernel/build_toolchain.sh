#!/bin/sh

#need lib`s
#urpmi -a libgmpxx-devel --download-all --allow-force
#urpmi -a libmpc- --download-all --allow-force
#urpmi -a mpfr- --download-all --allow-force

DIR="/opt/toolchain-mipsel"
BINUTILVER=binutils-2.20.1
GCCVER=gcc-4.2.4

export WDIR=$DIR/tmp
export TARGET=mipsel-kernel-linux-gnu
export PREFIX=$DIR
export PATH="${PATH}":${PREFIX}/bin

mkdir -p $WDIR
cd $WDIR
mkdir -p ${TARGET}-toolchain  && cd ${TARGET}-toolchain

tar xjf $BINUTILVER.tar.bz2
mkdir -p build-binutils && cd build-binutils
../$BINUTILVER/configure --target=$TARGET --prefix=$PREFIX
make
make install
cd ..

tar xjf $GCCVER.tar.bz2
mkdir -p build-gcc-bootstrap && cd build-gcc-bootstrap
../$GCCVER/configure --target=$TARGET --prefix=$PREFIX \
    --enable-languages=c --without-headers \
    --with-gnu-ld --with-gnu-as \
    --disable-shared --disable-threads 
make -j2
make install
cd ..
