#!/bin/bash

echo "==================CONFIGURE-SAMBA3=============================="
APROOTDIR=`pwd`

cd $APROOTDIR/source

if [ ! -f $APROOTDIR/source/configure ]; then
    autogen.sh
fi
if [ ! -f $APROOTDIR/source/Makefile.in ]; then
    automake
fi

CONFOPTS="--host=mipsel-linux --build=i686-pc-linux-gnu --target=mipsel"
CONFOPTS="$CONFOPTS --disable-iprint --disable-pie --disable-relro --disable-fam --disable-cups --disable-debug"
CONFOPTS="$CONFOPTS --disable-dnssd --disable-avahi --disable-swat --disable-dmalloc --disable-nss-wrapper"
CONFOPTS="$CONFOPTS --disable-developer --disable-krb5developer --disable-picky-developer"
CONFOPTS="$CONFOPTS --without-ldap --without-ads --without-cifsmount"
CONFOPTS="$CONFOPTS --without-cifsupcall --with-syslog --without-sys-quotas"
CONFOPTS="$CONFOPTS --without-utmp --without-cluster-support --without-winbind"
CONFOPTS="$CONFOPTS --without-libtalloc --without-libtdb --without-libnetapi"
CONFOPTS="$CONFOPTS --without-libsmbclient --without-libsmbsharemodes --without-libaddns"
CONFOPTS="$CONFOPTS --with-logfilebase=/var/log --with-lockdir=/var/lock --sysconfdir=/etc"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"

./configure $CONFOPTS
