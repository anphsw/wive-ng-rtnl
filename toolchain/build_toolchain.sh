#!/bin/sh

#need lib`s
#urpmi -a libgmpxx-devel --download-all --allow-force
#urpmi -a libmpc- --download-all --allow-force
#urpmi -a mpfr- --download-all --allow-force
#urpmi -a texinfo- --download-all --allow-force
#urpmi -a gcc-gfortran --download-all --allow-force

DIR=`pwd`
ROOTDIR=$DIR

KERNELHDRS=kernel-headers
BINUTILVER=binutils-2.20.1
UCLIBCVER=uClibc-0.9.28
GCCVER=gcc-4.5.2

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
export LC_ALL=C

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

##################################TUNE FOR CURRENT VERSION GCC BUILD####################################

HOSTGCCVER=`gcc --version | grep "(GCC)" | awk {' print $3 '}`
if [ "$HOSTGCCVER" = "4.6.0" ]; then
    export CFLAGS="-Wno-pointer-sign -Wno-unused-but-set-variable -Wno-trigraphs -Wno-format-security"
fi

EXT_OPT=""
if [ "$GCCVER" = "gcc-4.6.0" ]; then                              
    EXT_OPT="--disable-libquadmath"                               
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
    (../$BINUTILVER/configure --target=$TARGET --prefix=$PREFIX --includedir=$KERNEL_HEADERS \
	--with-sysroot=$PREFIX --with-build-sysroot=$PREFIX && \
    make KERNEL_HEADERS=$TARGET_DIR/include && \
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
	--enable-languages=c && \
    make -j3 && \
    make install) || exit 1
    cd ..
fi

if [ "$UCLIB" = "YES" ]; then
    echo "=====================BUILD-C-HEADERS===================="
    cp -fv uclibc-config-all $UCLIBCVER/.config
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
	--enable-languages=c++ && \
    make -j3 all-host all-target-libgcc && \
    make install-host install-target-libgcc) || exit 1
    cd ..
fi
echo "====================All IS DONE!========================"

