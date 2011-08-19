#!/bin/sh

RO_ROOT=romfs
TOOLSPREFIX=./toolchain/bin/mipsel-linux-uclibc
STRIP="$TOOLSPREFIX-strip --strip-debug --strip-unneeded"
OBJCOPY="$TOOLSPREFIX-objcopy --strip-debug --strip-unneeded"
SSTRIP=./tools/sstrip/sstrip
MODULES=`find romfs/lib/modules -type f | grep ".ko"`;

echo --------------------------------GENERATE CONFIG-----------------------------
. linux/.config
[ ! -f $RO_ROOT/etc/scripts/config.sh ] || rm -f $RO_ROOT/etc/scripts/config.sh
touch $RO_ROOT/etc/scripts/config.sh
echo "Search parameters used"
PARAMS=`find $RO_ROOT/etc -type f -print -exec cat {} \; | awk -v RS='"' '!(NR%2)' | sed '/$CONFIG_/!d; s///' | sort | uniq`
for i in $PARAMS; do
    var=$(eval "echo \$CONFIG_$i")
    if [ $var ]; then
	echo "Found CONFIG_$i"
	echo "CONFIG_$i=$var" >> $RO_ROOT/etc/scripts/config.sh
    fi
done
chmod 777 $RO_ROOT/etc/scripts/config.sh

echo -------------------------------FIND FILES TO STRIP-----------------------------
NON_STRIPS_BIN=`find $RO_ROOT/bin -type f -print -exec file {} \; | grep -v "modules" | grep -v "icon" | grep -v "start" | grep -v "rc" | grep -v ".sh" | cut -d":" -f1`
NON_STRIPS_LIB=`find $RO_ROOT/lib -type f -print -exec file {} \; | grep -v "modules" | grep -v ".a" | grep -v ".la" | grep -v ".pc" | cut -d":" -f1`
echo -----------------------------------STRIP BIN----------------------------------
for i in $NON_STRIPS_BIN; do
    echo $i;
    $OBJCOPY $i $i
done
if [ "$NON_STRIPS_BIN" != "" ]; then
  echo BIN: $NON_STRIPS_BIN
  $STRIP $NON_STRIPS_BIN
  $SSTRIP $NON_STRIPS_BIN
fi
echo -----------------------------------STRIP LIB----------------------------------
for i in $NON_STRIPS_LIB; do
    echo $i;
    $OBJCOPY $i $i
done
if [ "$NON_STRIPS_LIB" != "" ]; then
  echo LIB: $NON_STRIPS_LIB
  $STRIP $NON_STRIPS_LIB
  $SSTRIP $NON_STRIPS_LIB
fi
echo -----------------------------------STRIP MOD----------------------------------
for i in $MODULES; do
    echo $i;
    $OBJCOPY $i $i
done

echo -----------------------------------SYNC!!-------------------------------------
sync
echo ----------------------------APP STRIP AND COPY OK-----------------------------
