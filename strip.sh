#!/bin/sh

RO_ROOT=romfs
STRIP=./toolchain/bin/mipsel-linux-strip
SSTRIP=./tools/sstrip/sstrip
MODULES=`find romfs/lib/modules -type f`;

echo ---------------------------------STRIP-LIB-SDK-------------------------------
#$STRIP $RO_ROOT/lib/*.a
$STRIP $RO_ROOT/lib/*.so

echo --------------------------------STRIP AND SSTRIP-----------------------------
echo Strip binary files
NON_STRIPS_BIN=`find $RO_ROOT/bin -type f -print -exec file {} \; | grep "not stripped" | grep -v "modules" | cut -d":" -f1`
NON_STRIPS_LIB=`find $RO_ROOT/lib -type f -print -exec file {} \; | grep "not stripped" | grep -v "modules" | cut -d":" -f1`

if [ "$NON_STRIPS_BIN" != "" ]; then
  echo BIN: $NON_STRIPS_BIN
  $STRIP $NON_STRIPS_BIN
  $SSTRIP $NON_STRIPS_BIN
fi
if [ "$NON_STRIPS_LIB" != "" ]; then
  echo LIB: $NON_STRIPS_LIB
  $STRIP $NON_STRIPS_LIB
  $SSTRIP $NON_STRIPS_LIB
fi
for i in $MODULES; do
  echo $i;
  mips-linux-objcopy --strip-debug --strip-unneeded $i $i
done

echo -----------------------------------SYNC!!-------------------------------------
sync
echo ----------------------------APP STRIP AND COPY OK-----------------------------
