#!/bin/sh

tar -cp --gzip etc > romfs/rwfs.gz
tar -zxvf dev.tgz
cp -vf dev.tgz romfs/dev.gz
cp -rfv dev/* romfs/dev
rm -fr dev
