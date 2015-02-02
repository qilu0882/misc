#!/bin/bash

echo "Modify the ramdisk.img"

echo "1.Inflate the image"
echo "2.Create the image"

read -p "Choose:" CHOOSE

#case ${CHOOSE} in
#1)inflate();;
#2)create() ;;
#esac


if [ "1" = ${CHOOSE} ];then
	echo "inflate()"
	cp ramdisk.img ramdisk.cpio.gz
	gzip -d ramdisk.cpio.gz 
	[ -e "tmp" ] || { echo "mkdir tmp"; mkdir tmp;}
	mv -f ramdisk.cpio tmp/
	cd tmp
	cpio -i -F ramdisk.cpio 
elif [ "2" = ${CHOOSE} ];then
	echo "create()"
	cd tmp
	cpio -i -t -F ramdisk.cpio | cpio -o -H newc -O ../ramdisk_new.cpio
	cd ..
	gzip ramdisk_new.cpio
	mv ramdisk_new.cpio.gz ramdisk_new.img
	echo "Create ramdisk_new.img finished "
fi
