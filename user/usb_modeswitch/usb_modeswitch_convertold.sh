#!/bin/bash

for i in `ls usb_modeswitch.d`
do
    sed '/StandardEject=1/s/StandardEject=1/MessageContent="5553424312345678000000000000061b000000020000000000000000000000"/g' usb_modeswitch.d/$i > usb_modeswitch.d/${i}.bak
    mv usb_modeswitch.d/${i}.bak usb_modeswitch.d/$i
    echo $i
done
