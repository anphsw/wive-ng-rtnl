#!/bin/bash

# standart eject
for i in `ls usb_modeswitch.d`
do
    sed '/StandardEject=1/s/StandardEject=1/MessageContent="5553424312345678000000000000061b000000020000000000000000000000"/g' usb_modeswitch.d/$i > usb_modeswitch.d/${i}.bak
    mv usb_modeswitch.d/${i}.bak usb_modeswitch.d/$i
    echo $i
done

# huaway eject
for i in `ls usb_modeswitch.d`
do
    sed '/HuaweiNewMode=1/s/HuaweiNewMode=1/MessageContent="55534243123456780000000000000011062000000101000100000000000000"/g' usb_modeswitch.d/$i > usb_modeswitch.d/${i}.bak
    mv usb_modeswitch.d/${i}.bak usb_modeswitch.d/$i
    echo $i
done

# huaway eject
for i in `ls usb_modeswitch.d`
do
    sed '/HuaweiNewMode=1/s/HuaweiNewMode=1/MessageContent="55534243123456780000000000000011062000000101000100000000000000"/g' usb_modeswitch.d/$i > usb_modeswitch.d/${i}.bak
    mv usb_modeswitch.d/${i}.bak usb_modeswitch.d/$i
    echo $i
done

# Configuration 2 eject
for i in `ls usb_modeswitch.d`
do
    sed '/Configuration=2/s/Configuration=2/MessageContent="5553424312345678000000000000061b000000020000000000000000000000"/g' usb_modeswitch.d/$i > usb_modeswitch.d/${i}.bak
    mv usb_modeswitch.d/${i}.bak usb_modeswitch.d/$i
    echo $i
done

# QuantaMode 1 eject
for i in `ls usb_modeswitch.d`
do
    sed '/QuantaMode=1/s/QuantaMode=1/MessageContent="5553424312345678000000000000061b000000020000000000000000000000"/g' usb_modeswitch.d/$i > usb_modeswitch.d/${i}.bak
    mv usb_modeswitch.d/${i}.bak usb_modeswitch.d/$i
    echo $i
done
