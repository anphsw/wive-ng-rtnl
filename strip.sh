#!/bin/sh

RO_ROOT=romfs
TOOLSPREFIX=./toolchain/bin/mipsel-linux-uclibc
STRIP="$TOOLSPREFIX-strip --strip-debug --strip-unneeded"
OBJCOPY="$TOOLSPREFIX-objcopy --strip-debug --strip-unneeded"
SSTRIP=./tools/sstrip/sstrip
MODULES=`find romfs/lib/modules -type f -name "*.ko"`;

echo --------------------------------GENERATE CONFIG-----------------------------
. linux/.config

SCONFIG_SH=$RO_ROOT/etc/scripts/config.sh
:>"$SCONFIG_SH"

echo "Search for parameters used"
PARAMS=
tmp= 
for dir in $RO_ROOT/etc/rc.d $RO_ROOT/etc/scripts
do
	tmp=$( grep -r -o "\$CONFIG_[^ \t\"]\+" "$dir"/* | sed 's/^.*$CONFIG_\(.\+\)$/\1/' | sort | uniq )
	PARAMS="$PARAMS $tmp"
done

for i in $PARAMS; do
    var=$( eval "echo \$CONFIG_$i" )
    if [ $var ]; then
	    echo "Found CONFIG_$i"
	    echo "CONFIG_$i=$var" >> "$SCONFIG_SH"
    fi
done
chmod 777 "$SCONFIG_SH"

echo -------------------------------FIND FILES TO STRIP-----------------------------
NON_STRIPS_BIN=`find $RO_ROOT/bin -type f -print -exec file {} \; | grep -vE "modules|icon|start|rc|\.sh" | cut -d":" -f1`
NON_STRIPS_LIB=`find $RO_ROOT/lib -type f -print -exec file {} \; | grep -vE "modules|\.a|\.la|\.pc" | cut -d":" -f1`
echo -----------------------------------STRIP BIN----------------------------------
for i in $NON_STRIPS_BIN; do
    echo $i;
    $OBJCOPY $i $i
done
if [ -n "$NON_STRIPS_BIN" ]; then
    echo BIN: $NON_STRIPS_BIN
    $STRIP $NON_STRIPS_BIN
    $STRIP -R .comment -R .note $NON_STRIPS_BIN
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
    $STRIP -R .comment -R .note $NON_STRIPS_LIB
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
