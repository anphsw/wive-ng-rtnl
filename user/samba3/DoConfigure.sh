#!/bin/bash

echo "==================CONFIGURE-SAMBA3=============================="
APROOTDIR=`pwd`

cd $APROOTDIR/source

if [ ! -f configure ]; then
    sh ./autogen.sh
fi

#arch options
CONFOPTS="--host=mipsel-linux --build=i686-pc-linux-gnu --target=mipsel"
#some function direct enable
CONFOPTS="$CONFOPTS --with-syslog --enable-largefile --enable-shared-libs"
#disable some function
CONFOPTS="$CONFOPTS --disable-iprint --disable-pie --disable-relro --disable-fam --disable-cups --disable-debug"
CONFOPTS="$CONFOPTS --disable-dnssd --disable-avahi --disable-swat --disable-dmalloc --disable-nss-wrapper"
CONFOPTS="$CONFOPTS --disable-developer --disable-krb5developer --disable-picky-developer"
CONFOPTS="$CONFOPTS --without-ldap --without-ads --without-cifsmount"
CONFOPTS="$CONFOPTS --without-cifsupcall --without-sys-quotas --without-quotas"
CONFOPTS="$CONFOPTS --without-utmp --without-cluster-support"
CONFOPTS="$CONFOPTS --without-acl-support --without-automount --without-sendfile-support"
CONFOPTS="$CONFOPTS --without-libtalloc --without-libtdb --without-libaddns --without-libnetapi"
CONFOPTS="$CONFOPTS --without-libsmbclient --without-winbind --without-libsmbsharemodes"
#path options
CONFOPTS="$CONFOPTS --with-logfilebase=/var/log --with-piddir=/var/lock --with-lockdir=/var/lock --sysconfdir=/etc"
CONFOPTS="$CONFOPTS --with-libiconv=$FIRMROOT/lib/lib $FIRMROOT/lib/libiconv/include --prefix=/"

./configure $CONFOPTS
