#!/bin/sh

#need lib`s
#urpmi -a libgmpxx-devel --download-all --allow-force
#urpmi -a libmpc- --download-all --allow-force
#urpmi -a mpfr- --download-all --allow-force

DIR=`pwd`
ROOTDIR=$DIR

KERNELHDRS=kernel-headers
BINUTILVER=binutils-2.20.1
UCLIBCVER=uClibc-0.9.28
GCCVER=gcc-4.2.4

UNPACK=YES
HEADERS=YES
BINUTILS=YES
GCC=YES
GCCCPP=YES
UCLIB=YES

export WDIR=$DIR/tmp
export TARGET=mipsel-linux-uclibc
export PREFIX=$DIR
export ROOTDIR=$PREFIX
export PATH="${PATH}":${PREFIX}/bin:${PREFIX}/lib
export TARGET_DIR=$WDIR/$TARGET-toolchain
export KERNEL_HEADERS=$TARGET_DIR/include
export CC=gcc

mkdir -p $WDIR

cd $WDIR
mkdir -p ${TARGET}-toolchain  && cd ${TARGET}-toolchain

if [ "$UNPACK" = "YES" ]; then
    echo "=================EXTRACT-KERNEL-HEADERS================="
    tar xjf $KERNELHDRS.tar.bz2
    echo "====================EXTRACT-BINUTILS===================="
    tar xjf $BINUTILVER.tar.bz2
    echo "=====================EXTRACT-UCLIBC====================="
    tar xjf $UCLIBCVER.tar.bz2
    echo "=======================EXTRACT-GCC======================"
    tar xjf $GCCVER.tar.bz2
fi

if [ "$HEADERS" = "YES" ]; then
    echo "=====================BUILD-C-HEADERS===================="
    cp -fv uclibc-config-head $UCLIBCVER/.config
    make -C $UCLIBCVER install_headers KERNEL_HEADERS=$TARGET_DIR/include
    mkdir -p $DIR/usr
    rm -rf $DIR/usr/include
    cp -rf $KERNEL_HEADERS $DIR/usr
    ln -sf $DIR/usr/include $DIR/include
fi

if [ "$BINUTILS" = "YES" ]; then
    echo "=====================BUILD-BINUTILS====================="
    mkdir -p build-binutils && cd build-binutils
    ../$BINUTILVER/configure --target=$TARGET --prefix=$PREFIX --includedir=$KERNEL_HEADERS \
	--with-sysroot=$PREFIX --with-build-sysroot=$PREFIX
    make KERNEL_HEADERS=$TARGET_DIR/include
    make install
    cd ..
fi

if [ "$GCC" = "YES" ]; then
    echo "=====================BUILD-GCC-C========================"
    mkdir -p build-gcc-bootstrap && cd build-gcc-bootstrap
    ../$GCCVER/configure \
	--target=$TARGET --prefix=$PREFIX --includedir=$KERNEL_HEADERS \
	--with-gnu-ld --with-gnu-as \
	--disable-tls --disable-libmudflap --disable-libssp \
	--disable-libgomp --disable-threads \
	--disable-shared \
	--with-sysroot=$PREFIX \
	--enable-languages=c
    make
    make install
    cd ..
fi

if [ "$UCLIB" = "YES" ]; then
    echo "=====================BUILD-C-HEADERS===================="
    cp -fv uclibc-config-all $UCLIBCVER/.config
    cd $UCLIBCVER
    make oldconfig
    make
    make install
    cd ..
fi

if [ "$GCCCPP" = "YES" ]; then
    echo "====================BUILD-GCC-CPP======================="
    mkdir -p build-gcc-bootstrap-cpp && cd build-gcc-bootstrap-cpp
    ../$GCCVER/configure \
	--target=$TARGET --prefix=$PREFIX --includedir=$KERNEL_HEADERS \
	--with-gnu-ld --with-gnu-as \
	--disable-tls --disable-libmudflap --disable-libssp \
	--disable-libgomp --disable-threads \
	--with-sysroot=$PREFIX \
	--enable-languages=c++
     make
    make install
    cd ..
fi
