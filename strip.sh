#!/bin/sh

RO_ROOT=romfs
TOOLSPREFIX=./toolchain/bin/mipsel-linux
STRIP="$TOOLSPREFIX-strip --strip-debug --strip-unneeded"
OBJCOPY="$TOOLSPREFIX-objcopy --strip-debug --strip-unneeded"
SSTRIP=./tools/sstrip/sstrip
MODULES=`find romfs/lib/modules -type f`;

echo --------------------------------STRIP AND SSTRIP-----------------------------
echo "FIND FILES TO STRIP"
NON_STRIPS_BIN=`find $RO_ROOT/bin -type f -print -exec file {} \; | grep -v "modules" | grep -v "icon" | grep -v "start" | grep -v "rc" | grep -v ".sh" | cut -d":" -f1`
NON_STRIPS_LIB=`find $RO_ROOT/lib -type f -print -exec file {} \; | grep -v "modules" | cut -d":" -f1`
echo "STRIP LIB"
if [ "$NON_STRIPS_BIN" != "" ]; then
  echo BIN: $NON_STRIPS_BIN
  $STRIP $NON_STRIPS_BIN
  $SSTRIP $NON_STRIPS_BIN
fi
echo "STRIP BIN"
if [ "$NON_STRIPS_LIB" != "" ]; then
  echo LIB: $NON_STRIPS_LIB
  $STRIP $NON_STRIPS_LIB
  $SSTRIP $NON_STRIPS_LIB
fi
echo "STRIP MODULES"
for i in $MODULES; do
  echo $i;
  $OBJCOPY $i $i
done

echo -----------------------------------SYNC!!-------------------------------------
sync
echo ----------------------------APP STRIP AND COPY OK-----------------------------
