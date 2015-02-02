#!/bin/bash

echo "\$# = $#"

if [ $# -lt 1 ]
then
	echo "Usage: `basename $0` <png file>"
	echo "       Convert png to ppm"
	exit
fi

# to install Netpbm on Ubuntu:
# $ sudo apt-get install netpbm

pngtopnm $1 > logo.pnm

pnmquant 224 logo.pnm > logo_clut224.pnm

pnmtoplainpnm logo_clut224.pnm > logo_clut224.ppm 
