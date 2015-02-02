#!/bin/bash

if (( $# < 1 )) ; then
    echo "$0 vmlinux [ramdisk [uImage]]"
    exit 1
fi

TARGET=$1
IMAGE=uImage
if [ $# == 2 ] ; then
    RDISK=$2
fi
if [ $# == 3 ] ; then
    RDISK=$2
    IMAGE=$3
fi

UTYPE=kernel
if [ -n "$RDISK" ] ; then
    RDISK=:$RDISK
    UTYPE=multi
fi

PREFIX=mipsel-linux-

OBJCOPY=${PREFIX}objcopy
NM=${PREFIX}nm

BIN=${TARGET}.bin

DATE=`date "+%y%m%d%H%M"`

drop_sections=".reginfo .mdebug .comment .note .pdr .options .MIPS.options"
for i in $drop_section ; do
    strip_flags="$strip_flags --remove-section=$i"
done

$OBJCOPY -O binary $strip_flags $TARGET $BIN

entry=`readelf -l $TARGET | grep "Entry point" | gawk '{print $3;}'`
loadaddr=`readelf -l $TARGET | grep "LOAD" | gawk '{print $3;}'`

if [ -f $BIN.gz ] ; then
    rm -f $BIN.gz
fi

echo $UTYPE $BIN.gz$RDISK $IMAGE

gzip -9 $BIN
mkimage -A mips -O linux -T $UTYPE -C gzip \
    -a $loadaddr -e $entry \
    -n "$TARGET-$DATE" \
    -d $BIN.gz$RDISK $IMAGE
echo "   Image: $IMAGE is ready"

rm -f $BIN $BIN.gz
