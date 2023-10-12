#!/bin/bash -xe

echo rvm_autoupdate_flag=0 >> ~/.rvmrc

sudo apt-get update
sudo apt-get install -y pkg-config curl xz-utils libjson-perl libwww-perl

DOWNLOAD_DIR=http://tho-otto.de/snapshots
URL=http://tho-otto.de/download
SYSROOT_DIR=/usr/m68k-atari-mint/sys-root
sudo mkdir -p $SYSROOT_DIR/usr

PKG_CONFIG_LIBDIR=/usr/m68k-atari-mint/lib/pkgconfig
sudo mkdir -p $PKG_CONFIG_LIBDIR

mkdir -p tmp
cd tmp

#
# get & install the sharedlibs
#
for f in bzip2108.zip exif0621.zip freetype2101.zip lzma524.zip tiff4010.zip zlib1213.zip jpeg8d.tbz png1637.tbz
do
	wget -q $URL/$f || exit 1
	case $f in
	*.zip)
		unzip $f >/dev/null
		;;
	*)
		tar xf $f >/dev/null
		;;
	esac
done

sudo cp -pr */usr/. $SYSROOT_DIR/usr/

#
# get & install a package with pkg-config files
# These are only needed to pass the configure script of xpdf
#
for f in shared-pkgconfigs.tar.bz2; do
	wget -q -O - $URL/$f | sudo tar --strip-components=1 -C / -xjf - || exit 1
done

#
# Get & install binutils & compiler
#
for package in binutils gcc mintbin; do
	wget -q -O - "$DOWNLOAD_DIR/${package}/${package}-latest.tar.bz2" | sudo tar -C / -xjf - || exit 1
done

#
# Get & libraries
#
URL=$URL/mint

for f in mintlib-0.60.1-mint-dev.tar.xz \
	fdlibm-mint-dev.tar.xz \
	gemlib-0.44.0-mint-20230212-dev.tar.xz \
	windom-2.0.1-mint-dev.tar.xz \
	windom1-1.21.3-mint-dev.tar.xz \
	giflib-5.1.4-mint-dev.tar.xz \
	zstd-1.5.5-mint-dev.tar.xz
do
	wget -q -O - $URL/$f | sudo tar -C / -xJf - || exit 1
done

cd ..
