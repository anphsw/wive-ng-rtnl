#!/bin/sh

echo "COPY CONFIG"
cp ./linux/.config etc/scripts/config.sh
chmod 777 etc/scripts/config.sh

echo "CREATE DEV/RWFS"
tar -cp --gzip etc > romfs/rwfs.gz
tar -zxvf dev.tgz
cp -vf dev.tgz romfs/dev.gz
cp -rfv dev/* romfs/dev
rm -fr dev

echo "SOME LINKS"
cd romfs/bin
ln -fvs ../etc/scripts/* .
cd ../..
