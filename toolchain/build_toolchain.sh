#!/bin/sh

DIR=`pwd`
ROOTDIR=$DIR

KERNELHDRS=kernel-headers
BINUTILVER=binutils-2.21
UCLIBCVER=uClibc-0.9.28
GCCVER=gcc-4.4.7

INSTALL_DEP=YES
UNPACK=YES
HEADERS=YES
BINUTILS=YES
GCC=YES
UCLIB=YES
GCCCPP=YES

export LC_PAPER=en_EN.UTF-8
export LC_ADDRESS=en_EN.UTF-8
export LC_MONETARY=en_EN.UTF-8
export LC_TELEPHONE=en_EN.UTF-8
export LC_IDENTIFICATION=en_EN.UTF-8
export LC_MEASUREMENT=en_EN.UTF-8
export LANGUAGE=en_EN.UTF-8:en
export LC_NAME=en_EN.UTF-8

export LC_COLLATE=
export LC_NUMERIC=
export LC_MESSAGES=
export LC_CTYPE=
export LC_TIME=

export LANG=en_US
export LC_ALL=POSIX

export WDIR=$DIR/tmp
export TARGET=mipsel-linux-uclibc
export PREFIX=$DIR
export ROOTDIR=$PREFIX
export PATH="${PATH}":${PREFIX}/bin:${PREFIX}/lib
export TARGET_DIR=$WDIR/$TARGET-toolchain
export KERNEL_HEADERS=$TARGET_DIR/include
export CC=gcc

#install need lib`s
if [ -f /etc/mandriva-release ] && [ "$INSTALL_DEP" = "YES" ]; then
    urpmi --auto -a glibc-
    urpmi --auto  -a libgmpxx-devel --download-all --allow-force
    urpmi --auto  -a libmpc- --download-all --allow-force
    urpmi --auto  -a mpfr- --download-all --allow-force
    urpmi --auto  -a gcc-gfortran --download-all --allow-force
    urpmi --auto  -a texinfo- --download-all --allow-force
fi

mkdir -p $WDIR

cd $WDIR
mkdir -p ${TARGET}-toolchain  && cd ${TARGET}-toolchain

##################################TUNE FOR CURRENT VERSION GCC BUILD####################################

HOSTGCCVER=`gcc --version | grep "(GCC)" | awk {' print $3 '} | cut -f -2 -d .`
if [ "$HOSTGCCVER" = "4.5" ] || [ "$HOSTGCCVER" = "4.6" ] || [ "$HOSTGCCVER" = "4.7" ]; then
    export CFLAGS="-g -O2 -Wno-pointer-sign -Wno-unused-but-set-variable -Wno-trigraphs -Wno-format-security"
fi

EXT_OPT="--disable-sanity-checks --disable-werror"
EXT_OPT="$EXT_OPT --disable-lto --enable-ld=yes --enable-gold=no"
if [ "$GCCVER" = "gcc-4.6.0" ] || [ "$GCCVER" = "gcc-4.6.1" ] || [ "$GCCVER" = "gcc-4.6.2" ] || [ "$GCCVER" = "gcc-4.6.3" ]; then
    EXT_OPT="$EXT_OPT --disable-biendian --disable-softfloat"
    EXT_OPT="$EXT_OPT --disable-libquadmath --disable-libquadmath-support"
fi

#########################################################################################################

if [ "$UNPACK" = "YES" ]; then
    echo "=================REMOVE-OLD-BUILD-TREE=================="
    rm -rf build-*
fi

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
    echo "=====================INSTALL-C-HEADERS===================="
    mkdir -p $DIR/usr
    rm -rf $DIR/usr/include
    cp -rf $KERNEL_HEADERS $DIR/usr
    ln -sf $DIR/usr/include $DIR/include
fi

if [ "$BINUTILS" = "YES" ]; then
    echo "=====================BUILD-BINUTILS====================="
    mkdir -p build-binutils && cd build-binutils
    (../$BINUTILVER/configure --target=$TARGET --prefix=$PREFIX --includedir=$KERNEL_HEADERS \
	--with-sysroot=$PREFIX --with-build-sysroot=$PREFIX && \
    make -j3 KERNEL_HEADERS=$TARGET_DIR/include && \
    make install) || exit 1
    cd ..
fi

if [ "$GCC" = "YES" ]; then
    echo "=====================BUILD-GCC-C========================"
    mkdir -p build-gcc-bootstrap && cd build-gcc-bootstrap
    (../$GCCVER/configure \
	--target=$TARGET --prefix=$PREFIX --includedir=$KERNEL_HEADERS \
	--with-gnu-ld --with-gnu-as \
	--disable-shared \
	--disable-tls --disable-libmudflap --disable-libssp $EXT_OPT \
	--disable-libgomp --disable-threads \
	--with-sysroot=$PREFIX \
	--enable-version-specific-runtime-libs --enable-languages=c && \
    make -j3 && \
    make install) || exit 1
    cd ..
fi

if [ "$UCLIB" = "YES" ]; then
    echo "=====================BUILD-C-HEADERS===================="
    cp -fv uclibc-config $UCLIBCVER/.config
    cd $UCLIBCVER
    (make oldconfig && \
    make -j3 && \
    make install) || exit 1
    cd ..
fi

if [ "$GCCCPP" = "YES" ]; then
    echo "====================BUILD-GCC-CPP======================="
    mkdir -p build-gcc-bootstrap-cpp && cd build-gcc-bootstrap-cpp
    (../$GCCVER/configure \
	--target=$TARGET --prefix=$PREFIX --includedir=$KERNEL_HEADERS \
	--with-gnu-ld --with-gnu-as \
	--disable-tls --disable-libmudflap --disable-libssp $EXT_OPT \
	--disable-libgomp --disable-threads \
	--with-sysroot=$PREFIX \
	--enable-version-specific-runtime-libs --enable-languages=c++ && \
    make -j3 all-host all-target-libgcc all-target-libstdc++-v3  && \
    make install-host install-target-libgcc install-target-libstdc++-v3) || exit 1
    cd ..
fi

echo "====================All IS DONE!========================"
