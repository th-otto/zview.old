#!/bin/sh

echo rvm_autoupdate_flag=0 >> ~/.rvmrc

sudo apt-get update
sudo apt-get install -y pkg-config curl xz-utils libjson-perl libwww-perl

URL=http://tho-otto.de/download
SYSROOT=/usr/m68k-atari-mint/sys-root
sudo mkdir -p $SYSROOT/usr

PKG_CONFIG_LIBDIR=/usr/m68k-atari-mint/lib/pkgconfig
sudo mkdir -p $PKG_CONFIG_LIBDIR

mkdir -p tmp
cd tmp

#
# get & install the sharedlibs
#
for f in bzip2106.zip exif0621.zip freetype2101.zip lzma524.zip tiff4010.zip zlib1211.zip jpeg8d.tbz png1637.tbz
do
	wget $URL/$f || exit 1
	case $f in
	*.zip)
		unzip $f >/dev/null
		;;
	*)
		tar xf $f >/dev/null
		;;
	esac
done

sudo cp -pr */usr/. $SYSROOT/usr/

#
# get & install a package with pkg-config files
# These are only needed to pass the configure script of xpdf
#
for f in shared-pkgconfigs.tar.bz2; do
	wget $URL/$f || exit 1
	tar xf $f >/dev/null
done

sudo cp -a shared-pkgconfigs/usr/m68k-atari-mint/lib/pkgconfig/* $PKG_CONFIG_LIBDIR

#
# Get & install binutils, compiler & libraries
#
URL=$URL/mint

for f in binutils-2.32-mint-20190223-bin-linux.tar.xz \
	gcc-9.1.1-mint-20190606-bin-linux.tar.xz \
	mintlib-0.60.1-mint-20181123-dev.tar.xz \
	fdlibm-20190823-mint-dev.tar.xz \
	gemlib-0.44.0-mint-20181123-dev.tar.xz \
	windom-2.0.1-mint-dev.tar.xz \
	windom1-1.21.3-mint-dev.tar.xz \
	giflib-5.1.4-mint-dev.tar.xz
do
	wget $URL/$f || exit 1
	sudo tar -C / -xf $f >/dev/null
done

cd ..
